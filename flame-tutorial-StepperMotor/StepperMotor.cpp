/*
 * Copyright (c) 2014, Inferno Embedded
 * All rights reserved.
 *
 *  License: GNU GPL v2 (see flame-Vusb-Keyboard/vusb/License.txt)
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

/* Demonstrates how to use the stepper driver
 */


// Bring in the FLAME IO header
#include <flame/io.h>

// Bring in the Stepper driver
#include <flame/StepperMotorUnipolar.h>

// Bring in the realtime clock driver
#include <flame/RTC.h>

// Bring in the power management header
#include <avr/power.h>
#include <avr/sleep.h>

// Bring in the timer header
#include <flame/Timer.h>

// Program space header, saves RAM by storing constants in flash
#include <avr/pgmspace.h>

using namespace flame;

#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
#define ATTINY
// A timer we will use to tick the RTC
FLAME_TimerImplementation<FLAME_TIMER8_0, FLAME_TIMER_REPETITIVE>tickTimer;
FLAME_TIMER_ASSIGN_1INTERRUPT(tickTimer, FLAME_TIMER0_INTERRUPTS);

#else
// A timer we will use to tick the RTC
TimerImplementation<FLAME_TIMER8_2, TimerMode::REPETITIVE>tickTimer;
FLAME_TIMER_ASSIGN_1INTERRUPT(tickTimer, FLAME_TIMER2_INTERRUPTS);
#endif

#define ALARM_COUNT	4
// The RTC object we will use
FLAME_RTC_CREATE (rtc, ALARM_COUNT);

/* We have a hard limit of 1000 steps/second due to the 1ms resolution of the RTC class
 * The motor & control circuitry have a lower limit due to their electrical & mechanical properties
 */
#define MAX_SPEED 500
#define MIN_SPEED 100

/* The steps per rotation
 * For the 28BYJ48 geared stepper, this is 32 steps multiplied by a gear ratio of 64,
 * multiplied by 2 since we are half stepping
 */
#define STEPS_PER_ROTATION	(2*32*64)

/* The stepper driver
 * Available modes are WAVE, FULL and HALF
 */
StepperMotorUnipolar<StepperMode::HALF, FLAME_PIN_B0> stepper(rtc);


/* A class that tells the stepper what to do next
 * Note that moveComplete() is called every time the stepper driver has completed a move
 */
class StepperInstructions : public StepperListener {
private:
	bool	_forward;		// The direction we are currently rotating
	float	_speed;			// The rotation speed (steps/second)
	bool	_speedUp;		// True if we are increasing speed, false otherwise

public:
	StepperInstructions();
	void moveComplete(int32_t position);
};

StepperInstructions::StepperInstructions() :
	_forward(true),
	_speed(1),
	_speedUp(true) {}

/**
 * Called when a motor movement is complete
 * @param position	the current position of the motor (unused)
 */
void StepperInstructions::moveComplete(UNUSED int32_t position) {
	_forward = !_forward;
	_speed += (_speedUp) ? 100 : -100;

	if (_speed > MAX_SPEED) {
		_speed = MAX_SPEED;
		_speedUp = false;
	} else if (_speed < MIN_SPEED) {
		_speed = MIN_SPEED;
		_speedUp = true;
	}

	int32_t newPosition = (_forward) ? 1 * STEPS_PER_ROTATION : 0;
	stepper.rotate(_forward, _speed, newPosition);
}

StepperInstructions stepperInstructions;


MAIN {
	// Disable all peripherals and enable just what we need
	power_all_disable();
#ifdef ATTINY
	power_timer0_enable();
#define PRESCALER	TimerPrescaler::PRESCALER_5_64
#else
	power_timer2_enable();
#define PRESCALER	TimerPrescaler::PRESCALER_7_64
#endif

	// Register the listener with the stepper driver to be notified when moves are complete
	stepper.registerListener(stepperInstructions);

	// Configure the tick timer to tick every 1ms (at 16MHz)
	tickTimer.setPeriods(PRESCALER, 249, 0);
	tickTimer.setListener1(rtc);
	tickTimer.enable();

	sei();

	/* Start with a forward rotation of 1 revolution at 100 steps/second
	 * Further instructions will be triggered from StepperInstructions::moveComplete
	 */
	stepper.rotate(true, 100, 1 * STEPS_PER_ROTATION);

	for (;;) {
		/* All the interesting things happen in events
		 */
		rtc.handleEvents();
	}

	return 0;
}
