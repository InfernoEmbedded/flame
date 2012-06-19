/*
 * Copyright (c) 2011, Make, Hack, Void Inc
 * All rights reserved.
 *
 *  License: GNU GPL v2 (see mhvlib-Vusb-Keyboard/vusb/License.txt)
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL MAKE, HACK, VOID BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Demonstrates how to use the software PWM driver
 */

#define MHVLIB_NEED_PURE_VIRTUAL

// Bring in the MHV IO header
#include <mhvlib/io.h>

// Bring in the power management header
#include <avr/power.h>
#include <avr/sleep.h>

// Bring in the SoftwarePWM header
#include <mhvlib/SoftwarePWM.h>

using namespace mhvlib;

// The timer for PWM control
TimerImplementation<MHV_TIMER8_2, TIMER_MODE::REPETITIVE> pwmTimer;
MHV_TIMER_ASSIGN_1INTERRUPT(pwmTimer, MHV_TIMER2_INTERRUPTS);

#define PWM_LISTENER_COUNT	5
SoftwarePWM<PWM_LISTENER_COUNT> pwm(pwmTimer);

SoftwarePWMPin<MHV_PIN_B0> led1;
SoftwarePWMPin<MHV_PIN_B1> led2;
SoftwarePWMPin<MHV_PIN_B2> led3;
SoftwarePWMPin<MHV_PIN_B3> led4;
SoftwarePWMPin<MHV_PIN_B4> led5;

MAIN {
	power_all_disable();
	power_timer2_enable();

	// Configure the PWM timer, for ~60 fps, 256 levels = ~16kHz  - 64,20
	pwmTimer.setPeriods(TIMER_PRESCALER::PRESCALER_7_64, 20, 0);
	pwmTimer.setListener1(pwm);
	pwmTimer.enable();

	pwm.addListener(led1);
	pwm.addListener(led2);
	pwm.addListener(led3);
	pwm.addListener(led4);
	pwm.addListener(led5);

	led1.setDutyCycle(1);
	led2.setDutyCycle(32);
	led3.setDutyCycle(64);
	led4.setDutyCycle(128);
	led5.setDutyCycle(255);

	sei();

	for (;;) {
		sleep_mode();
	}

	return 0;
}
