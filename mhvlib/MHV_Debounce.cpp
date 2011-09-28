/* Copyright (c) 2011, Make, Hack, Void Inc
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

#include <MHV_Debounce.h>

/**
 * Debouncing helper for buttons connected directly to PCINT capable pins
 * The user must pull the pin up, either externally (and initing the pin by calling mhv_setInput),
 * or internally by calling mhv_setInputPullup
 *
 * @param	pinChangeManager	the pin change manager
 * @param	rtc					the realtime clock we will use for timing
 * @param	debounceTime		the minimum amount of time to count as a button press (in milliseconds)
 * @param	heldTime			the minimum amount of time to consider a button held down
 * @param	repeatTime			the time after which the held call repeats
 */
MHV_Debounce::MHV_Debounce(MHV_PinChangeManager *pinChangeManager, MHV_RTC *rtc,
		uint16_t debounceTime, uint16_t heldTime, uint16_t repeatTime) {
	_pinChangeManager = pinChangeManager;
	_rtc = rtc;
	_debounceTime.timestamp = 0;
	_debounceTime.milliseconds = debounceTime;

	_heldTime.timestamp = 0;
	_heldTime.milliseconds = 0;
	mhv_timestampIncrement(&_heldTime, 0, heldTime);

	_repeatTime.timestamp = 0;
	_repeatTime.milliseconds = 0;
	mhv_timestampIncrement(&_repeatTime, 0, repeatTime);

	for (uint8_t i = 0; i < MHV_PC_INT_COUNT; ++i) {
		initPin(i);
	}
}

void MHV_Debounce::initPin(uint8_t pinchangeInterrupt) {
	_pins[pinchangeInterrupt].previous = 0;
	_pins[pinchangeInterrupt].timestamp.timestamp = 0;
	_pins[pinchangeInterrupt].timestamp.milliseconds = 0;
	_pins[pinchangeInterrupt].held = false;
}

/**
 * Called periodically to check if pins have been held
 * Ideally, this should be called from the main loop, rather than the interrupt context
 */
void MHV_Debounce::checkHeld() {
	MHV_DEBOUNCE_PIN *pin = _pins;
	MHV_DEBOUNCE_PIN *maxPin = pin + MHV_PC_INT_COUNT;

	for (; pin < maxPin; ++pin) {
		if (pin->timestamp.timestamp && pin->timestamp.milliseconds && !pin->previous) {
// Pin is currently held down
			MHV_TIMESTAMP heldFor;
			_rtc->elapsed(&(pin->timestamp), &heldFor);

			if (!pin->held && mhv_timestampGreaterThanOrEqual(&heldFor, &_heldTime)) {
				pin->held = true;
				pin->listener->heldDown(pin - _pins, &heldFor);
				_rtc->current(&(pin->timestamp));
			} else if (pin->held && mhv_timestampGreaterThanOrEqual(&heldFor, &_repeatTime)) {
				pin->listener->heldDown(pin - _pins, &heldFor);
				_rtc->current(&(pin->timestamp));
			}
		}
	}
}

/**
 * Trigger for pin change interrupts - scans through 8 pins starting at the offset
 * @param	pcInt		the pin change interrupt that was triggered
 * @param	newState	the new state of the pin
 */
void MHV_Debounce::pinChanged(uint8_t pcInt, bool newState) {
	if (newState) {
// Pin has been released
		MHV_TIMESTAMP heldFor;

/* Call the singlePress function if the time is greater than the debounce time,
 * and the button has not been held down
 */
		_rtc->elapsed(&(_pins[pcInt].timestamp), &heldFor);
		if (!_pins[pcInt].held && mhv_timestampGreaterThanOrEqual(&heldFor, &_debounceTime)) {
			_pins[pcInt].listener->singlePress(pcInt, &heldFor);
		}

		_pins[pcInt].held = false;
	} else {
// Pin has been pressed
		_rtc->current(&(_pins[pcInt].timestamp));
	}
}


/**
 * Assign a pin to debounce
 * @param dir..pinchangeInterrupt	a pin declaration (MHV_PIN_*)
 * @param singlePress				the function to call when a single press is detected
 * @param heldDown					the function to call when a button is held down (called repeatedly)
 * @param data						data to pass to the functions when called
 */
void MHV_Debounce::assignKey(volatile uint8_t *dir, volatile uint8_t *out, volatile uint8_t *in,
		uint8_t pin, int8_t pinchangeInterrupt, MHV_DebounceListener *listener) {
	_pins[pinchangeInterrupt].previous = *in & _BV(pin);
	_pins[pinchangeInterrupt].timestamp.milliseconds = 0;
	_pins[pinchangeInterrupt].timestamp.timestamp = 0;
	_pins[pinchangeInterrupt].listener = listener;
	_pins[pinchangeInterrupt].held = false;

	if (!_pins[pinchangeInterrupt].previous) {
// Pin started off held down
		_rtc->current(&(_pins[pinchangeInterrupt].timestamp));
	}

// Enable the interrupt
	uint8_t bit = pinchangeInterrupt;
#if MHV_PC_INT_COUNT > 15
	if (pinchangeInterrupt > 15) {
		bit -= 16;
		PCMSK2 |= _BV(bit);
		PCICR |= _BV(PCIE2);
	} else
#endif
#if MHV_PC_INT_COUNT > 7
	if (pinchangeInterrupt > 7) {
		bit -= 8;
		PCMSK1 |= _BV(bit);
		PCICR |= _BV(PCIE1);
	} else
#endif
	{
		PCMSK0 |= _BV(bit);
		PCICR |= _BV(PCIE0);
	}
}

void MHV_Debounce::deassignKey(volatile uint8_t *dir, volatile uint8_t *out, volatile uint8_t *in,
		uint8_t pin, int8_t pinchangeInterrupt) {
	initPin(pinchangeInterrupt);

	uint8_t bit = pinchangeInterrupt;
#if MHV_PC_INT_COUNT > 15
	if (pinchangeInterrupt > 15) {
		bit -= 16;
		PCMSK2 &= ~_BV(bit);
	} else
#endif
#if MHV_PC_INT_COUNT > 7
	if (pinchangeInterrupt > 7) {
		bit -= 8;
		PCMSK1 &= ~_BV(bit);
	} else
#endif
	{
		PCMSK0 &= ~_BV(bit);
	}
}
