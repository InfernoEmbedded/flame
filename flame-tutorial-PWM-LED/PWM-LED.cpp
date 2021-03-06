/*
 * Copyright (c) 2014, Inferno Embedded
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of the Inferno Embedded nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL INFERNO EMBEDDED BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Fade an LED on Arduino pin 12 (Arduino Mega) or Arduino pin 10 (others)
 * This corresponds to pin OC1B
 * Uses a 16 bit timer for PWM and an 8 bit timer for animation
 */
#define OUTPUT_PIN FLAME_PIN_TIMER_1_B

// Bring in the FLAME IO header
#include <flame/io.h>

// Bring in the FLAME timer header
#include <flame/Timer.h>

// Bring in the AVR interrupt header (needed for cli)
#include <avr/interrupt.h>

// Bring in the power management header
#include <avr/power.h>
#include <avr/sleep.h>

using namespace flame;

/* Declare an 8 bit timer - we will use Timer 2 since it is an 8 bit timer
 * on all microcontrollers used on Arduino boards
 */
TimerImplementation<FLAME_TIMER8_2, TimerMode::REPETITIVE>animationTimer;

/* Each timer module generates interrupts
 * We must assign the timer object created above to handle these interrupts
 * Since we only need one interrupt assigned, we can save some space by not
 * assigning the others
 */
FLAME_TIMER_ASSIGN_1INTERRUPT(animationTimer, FLAME_TIMER2_INTERRUPTS);

/* Declare a 16 bit timer for PWM output
 */
TimerImplementation<FLAME_TIMER16_1, TimerMode::PWM_FAST_16>pwmTimer;

/* The maximum value of the PWM
 * This defines the resolution of the PWM, as well as the frequency
 */
#define PWM_TOP 1000

// How much to move the PWM duty cycle by each time the animation timer fires
#define PWM_INCREMENT 5


/* Our animation trigger function that will be called every time the animation
 * timer is triggered
 *
 * This will fade the LED up to 50% and back down to 0
 */
class Animation : public TimerListener {
public:
	void alarm(UNUSED AlarmSource source) {
	/* static variables are initialised once at boot, and persist between calls
	 * What is the next action to take
	 */
		static bool fadeUp = true;

	/* Get the current output, and increment/decrement it based on the direction
	 * Note that we are only going to 50% duty cycle as it is difficult to see
	 * the difference between 50% & 100% duty cycle on LEDs. This could be fixed
	 * by gamma correcting the output (see FLAME_GammaCorrect)
	 */
		uint16_t current = pwmTimer.getOutput2();

		if (fadeUp && current + PWM_INCREMENT >= PWM_TOP / 2) {
			fadeUp = false;
			pwmTimer.setOutput2(PWM_TOP / 2);
		} else if (!fadeUp && current <= PWM_INCREMENT) {
			fadeUp = true;
			pwmTimer.setOutput2(0);
		} else if (fadeUp) {
			pwmTimer.setOutput2(current + PWM_INCREMENT);
		} else {
			pwmTimer.setOutput2(current - PWM_INCREMENT);
		}
	}
};


Animation animation;

MAIN {
	// Disable all peripherals and enable just what we need
	power_all_disable();
	power_timer2_enable();
	power_timer1_enable();
	set_sleep_mode(SLEEP_MODE_IDLE);

	/* Enable output on the output pin - see the declaration above
	 */
	setOutput(OUTPUT_PIN);

	/* Trigger the animation routine every 20ms
	 */
	(void) animationTimer.setTimes(20000UL, 0);

	// Tell the timer to call our trigger function
	animationTimer.setListener1(animation);

	// Set the PWM prescaler to 1 (no prescaler)
	pwmTimer.setPrescaler(TimerPrescaler::PRESCALER_5_1);

	/* Set the TOP value of the PWM timer - this defines the resolution &
	 * frequency of the PWM output
	 *
	 * PWM frequency = F_CPU / 2 / PWM_TOP
	 *               = 16,000,000 / 2 / 1000
	 *               = 8KHz
	 *
	 * PWM frequency = F_CPU / 2 / PWM_TOP
	 *               = 20,000,000 / 2 / 1000
	 *               = 10KHz
	 */
	pwmTimer.setTop(PWM_TOP);

	/* Tell the pwmTimer to use output compare 2 for PWM output
	 * It will turn off the output pin when the timer elapses, and turn it on
	 * when it is reset
	 */
	pwmTimer.connectOutput2(TimerConnect::CLEAR);

	// Start with the PWM duty cycle set to 0
	pwmTimer.setOutput2(0);

	// Start the timers
	animationTimer.enable();
	pwmTimer.enable();

	// Enable interrupts
	sei();

	/* Do nothing forever - the timer will call the animationTrigger() function
	 * periodically
	 */
	for (;;) {
		sleep_mode();
	}

	return 0;
}
