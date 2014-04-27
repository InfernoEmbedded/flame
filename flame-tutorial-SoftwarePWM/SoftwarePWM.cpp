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

/* Demonstrates how to use the software PWM driver
 */



// Bring in the FLAME IO header
#include <flame/io.h>

// Bring in the power management header
#include <avr/power.h>
#include <avr/sleep.h>

// Bring in the SoftwarePWM header
#include <flame/SoftwarePWM.h>

using namespace flame;

// The timer for PWM control
TimerImplementation<FLAME_TIMER8_2, TimerMode::REPETITIVE> pwmTimer;
FLAME_TIMER_ASSIGN_1INTERRUPT(pwmTimer, FLAME_TIMER2_INTERRUPTS);

#define PWM_LISTENER_COUNT	5
SoftwarePWM<PWM_LISTENER_COUNT> pwm(pwmTimer);

SoftwarePWMPin<FLAME_PIN_B0> led1;
SoftwarePWMPin<FLAME_PIN_B1> led2;
SoftwarePWMPin<FLAME_PIN_B2> led3;
SoftwarePWMPin<FLAME_PIN_B3> led4;
SoftwarePWMPin<FLAME_PIN_B4> led5;

MAIN {
	power_all_disable();
	power_timer2_enable();

	// Configure the PWM timer, for ~60 fps, 256 levels = ~16kHz  - 64,20
	pwmTimer.setPeriods(TimerPrescaler::PRESCALER_7_64, 20, 0);
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
