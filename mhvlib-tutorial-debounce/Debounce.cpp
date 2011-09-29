/*
 * Copyright (c) 2011, Make, Hack, Void Inc
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of the Make, Hack, Void nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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

/* Toggle an LED every time a button is pressed
 *
 */

#include <MHV_io.h>
#include <MHV_Timer8.h>
#include <MHV_RTC.h>
#include <MHV_Debounce.h>

#include <avr/power.h>
#include <avr/sleep.h>

// A timer we will use to tick the RTC
MHV_Timer8 tickTimer(MHV_TIMER8_2);
MHV_TIMER_ASSIGN_1INTERRUPT(tickTimer, MHV_TIMER2_INTERRUPTS);

/* A buffer the RTC will use to store events - this determines how many events
 * can be registered simultaneously
 */
#define ALARM_COUNT	10
MHV_ALARM alarms[ALARM_COUNT];

#define TIMEZONE 600 // UTC+10

// The RTC object we will use
MHV_RTC rtc(&tickTimer, alarms, ALARM_COUNT, TIMEZONE);

// A timer trigger that will tick the RTC
void rtcTrigger(void *data) {
	/* We can call tick1ms here because we have been careful to configure the
	 * timer for exactly 1ms. If the timer was faster, we would call tick()
	 * instead, which is slightly more expensive.
	 */
	rtc.tick1ms();
}


// Triggers for button presses
class ButtonHandler : public MHV_DebounceListener {
	void singlePress(uint8_t pcInt, MHV_TIMESTAMP *heldFor);
	void heldDown(uint8_t pcInt, MHV_TIMESTAMP *heldFor);
};

void ButtonHandler::singlePress(uint8_t pcInt, MHV_TIMESTAMP *heldFor) {
	mhv_pinToggle(MHV_ARDUINO_PIN_13);
}

void ButtonHandler::heldDown(uint8_t pcInt, MHV_TIMESTAMP *heldFor) {
	mhv_pinToggle(MHV_ARDUINO_PIN_13);
}

ButtonHandler buttonHandler;

MHV_PinChangeManager pinChangeManager;
MHV_PINCHANGE_MANAGER_ASSIGN_INTERRUPTS(pinChangeManager);

// Minimum time a button must be held to be considered a press (milliseconds)
#define DEBOUNCE_TIME	100

// Minimum time a button must be held to be considered held down (milliseconds)
#define HELD_TIME		500

// Time to repeat the held down call while the button is held down (milliseconds)
#define REPEAT_TIME		100

MHV_Debounce debouncer(&pinChangeManager, &rtc, DEBOUNCE_TIME, HELD_TIME, REPEAT_TIME);

int main(void) {
	// Disable all peripherals and enable just what we need
	power_all_disable();
	power_timer2_enable();
	// Specify what level sleep to perform
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);

	// Enable output on pin 13 of the Arduino - this normally has an LED connected
	mhv_setOutput(MHV_ARDUINO_PIN_13);

	// Configure the tick timer to tick every 1ms (at 16MHz)
	tickTimer.setPeriods(MHV_TIMER_PRESCALER_5_64, 249, 0);
	tickTimer.setTriggers(rtcTrigger, 0, 0, 0);
	tickTimer.enable();

	// B0 is pin 53 on the Arduino Mega, pin 8 on the Diecimilla
	mhv_setInputPullup(MHV_PIN_B0);
	debouncer.assignKey(MHV_PIN_B0, &buttonHandler);

	// Enable global interrupts
	sei();

	while (1) {
		pinChangeManager.handleEvents();
		debouncer.checkHeld();
// Sleep until the next interrupt
		sleep_mode();
	}

// Main must return an int, even though we never get here
	return 0;
}
