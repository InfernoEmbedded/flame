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

/* Demonstrates how to use the servo driver
 */



// Bring in the FLAME IO header
#include <flame/io.h>

// Bring in the Servo driver
#include <flame/ServoControl.h>

// Bring in the realtime clock driver
#include <flame/RTC.h>

// Bring in the power management header
#include <avr/power.h>
#include <avr/sleep.h>

// Bring in the timer header
#include <flame/Timer.h>

using namespace flame;

// A timer we will use to tick the RTC
TimerImplementation<FLAME_TIMER8_2, TimerMode::REPETITIVE>tickTimer;
FLAME_TIMER_ASSIGN_1INTERRUPT(tickTimer, FLAME_TIMER2_INTERRUPTS);

#define ALARM_COUNT	4
// The RTC object we will use
FLAME_RTC_CREATE (rtc, ALARM_COUNT);

// Create the timer that the servo controller will manage
TimerImplementation<FLAME_TIMER16_1, TimerMode::REPETITIVE>servoTimer;
FLAME_TIMER_ASSIGN_1INTERRUPT(servoTimer, FLAME_TIMER1_INTERRUPTS);

// Create the servo controller
#define SERVO_COUNT 1
ServoControl<SERVO_COUNT> servos(servoTimer);

class MoveServos: public TimerListener {
#define SERVO_INCREMENT 1000
	void alarm(UNUSED AlarmSource source) {
		static uint16_t position = 0;

// Increment the servo, we don't mind if it wraps, the motor will just move back to the start
		position += SERVO_INCREMENT;

		servos.positionServo(0, position);
	}
};

MoveServos moveServos;

MAIN {
	power_all_disable();

	power_timer1_enable();
	power_timer2_enable();

	set_sleep_mode(SLEEP_MODE_IDLE);


// Configure the tick timer to tick every 1ms (at 16MHz)
	tickTimer.setPeriods(TimerPrescaler::PRESCALER_5_64, 249, 0);
	tickTimer.setListener1(rtc);
	tickTimer.enable();

	sei();

// Set up the servo motor
	servos.addServo(0, FLAME_PIN_D6);
	servos.enable();

// Set up a job to move the servo every 1 second
	rtc.addAlarm(moveServos, 1, 0, 1, 0);

	for (;;) {
		rtc.handleEvents();
		sleep_mode();
	}

	return 0;
}
