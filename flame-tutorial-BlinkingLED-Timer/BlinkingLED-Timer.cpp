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

/* A blinking LED on Arduino pin 13 - uses a timer to toggle the LED
 */



// Bring in the FLAME IO header
#include <flame/io.h>

// Bring in the FLAME Pin header
#include <flame/Pin.h>

// Bring in the FLAME timer header
#include <flame/Timer.h>

// Bring in the AVR delay header (needed for _delay_ms)
#include <util/delay.h>

// Bring in the AVR interrupt header (needed for cli)
#include <avr/interrupt.h>

// Bring in the power management headers
#include <avr/power.h>
#include <avr/sleep.h>

// Bring in Arduino defines
#include <boards/Arduino.h>

using namespace flame;

// The LED is on Arduino pin 13
PinImplementation<FLAME_ARDUINO_PIN_13> led;

/* Declare an 8 bit timer - we will use Timer 2 since it is an 8 bit timer
 * on all microcontrollers used on Arduino boards
 */
TimerImplementation<FLAME_TIMER8_2, TimerMode::REPETITIVE>tickTimer;

/* Each timer module generates interrupts
 * We must assign the timer object created above to handle these interrupts
 */
FLAME_TIMER_ASSIGN_1INTERRUPT(tickTimer, FLAME_TIMER2_INTERRUPTS);

/* A trigger function that will be called every time the timer is triggered
 * Since we want a timer triggered every 333ms, but the hardware is incapable
 * of this, we will instead trigger every 1ms and maintain a counter instead
 */
class LEDBlinker : public TimerListener {
	void alarm(UNUSED AlarmSource source) {
	// static variables are initialised once at boot, and persist between calls

	// A counter to keep track of how many times we are called
		static uint16_t count = 0;

	/* Increment count, and if it is the 333rd time, toggle the LED and reset the
	 * counter
	 */
		if (333 == ++count) {
				led.toggle();

			count = 0;
		}
	}
};

// Instantiate the blinker class
LEDBlinker blinker;

MAIN {
// Disable all peripherals and enable just what we need
	power_all_disable();
	power_timer2_enable();
// Specify what level sleep to perform
//	set_sleep_mode(SLEEP_MODE_IDLE);

// Enable output on pin 13 of the Arduino - this normally has an LED connected
	setOutput(FLAME_ARDUINO_PIN_13);

/* We want the timer to trigger ever 333ms, however, the maximum an 8 bit timer
 * can count to is 32768us @ 16MHz
 * Instead, we will trigger the timer every 1 ms, and increment a counter within
 * the trigger function to give us our 333ms count
 */
	tickTimer.setTimes(1000UL, 0UL);

// Tell the timer to call our trigger function
	tickTimer.setListener1(blinker);

// Start the timer
	tickTimer.enable();

// Enable interrupts
	sei();

// Main loop - just sleep the CPU until the timer wakes it up
	for(;;) {
		sleep_mode();
	}

	return 0;
}
