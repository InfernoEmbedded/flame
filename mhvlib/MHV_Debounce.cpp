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
MHV_Debounce::MHV_Debounce(MHV_PinChangeManager &pinChangeManager, MHV_RTC &rtc,
		uint16_t debounceTime, uint16_t heldTime, uint16_t repeatTime) :
			_rtc(rtc),
			_pinChangeManager(pinChangeManager) {
	_debounceTime.timestamp = 0;
	_debounceTime.milliseconds = debounceTime;

	_heldTime.timestamp = 0;
	_heldTime.milliseconds = 0;
	mhv_timestampIncrement(_heldTime, 0, heldTime);

	_repeatTime.timestamp = 0;
	_repeatTime.milliseconds = 0;
	mhv_timestampIncrement(_repeatTime, 0, repeatTime);

	for (uint8_t i = 0; i < MHV_PC_INT_COUNT; ++i) {
		initPin(i);
	}
}

void MHV_Debounce::initPin(uint8_t pinchangeInterrupt) {
	mhv_memClear(_pins + pinchangeInterrupt, MHV_BYTESIZEOF(*_pins), (uint8_t)1);
}

/**
 * Called periodically to check if pins have been held
 * Ideally, this should be called from the main loop, rather than the interrupt context
 */
void MHV_Debounce::checkHeld() {
	MHV_DEBOUNCE_PIN *pin = _pins;
	MHV_DEBOUNCE_PIN *maxPin = pin + MHV_PC_INT_COUNT;

	for (; pin < maxPin; ++pin) {
		if (pin->timestamp.timestamp && !pin->previous) {
// Pin is currently held down
			MHV_TIMESTAMP heldFor;
			_rtc.elapsed(pin->timestamp, heldFor);

			if (!pin->held && mhv_timestampGreaterThanOrEqual(heldFor, _heldTime)) {
				pin->held = true;
				pin->listener->heldDown(pin - _pins, &heldFor);
				_rtc.current(pin->timestamp);
			} else if (pin->held && mhv_timestampGreaterThanOrEqual(heldFor, _repeatTime)) {
				pin->listener->heldDown(pin - _pins, &heldFor);
				_rtc.current(pin->timestamp);
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
		_rtc.elapsed(_pins[pcInt].timestamp, heldFor);
		if (!_pins[pcInt].held && mhv_timestampGreaterThanOrEqual(heldFor, _debounceTime)) {
			_pins[pcInt].listener->singlePress(pcInt, &heldFor);
		}

		_pins[pcInt].held = false;
		_pins[pcInt].timestamp.timestamp = 0;
	} else {
// Pin has been pressed
		_rtc.current(_pins[pcInt].timestamp);
	}
}


/**
 * Assign a pin to debounce
 * @param	pin						An MHV_PIN_* macro
 * @param	listener				a class to call when the button is pressed or held down
 */
void MHV_Debounce::assignKey(MHV_DECLARE_PIN(pin), MHV_DebounceListener &listener) {
	_pins[pinPinchangeInterrupt].previous = VALUE(pinIn) & _BV(pinPin);
	_pins[pinPinchangeInterrupt].timestamp.milliseconds = 0;
	_pins[pinPinchangeInterrupt].timestamp.timestamp = 0;
	_pins[pinPinchangeInterrupt].listener = &listener;
	_pins[pinPinchangeInterrupt].held = false;

	if (!_pins[pinPinchangeInterrupt].previous) {
// Pin started off held down
		_rtc.current(_pins[pinPinchangeInterrupt].timestamp);
	}

	_pinChangeManager.registerListener(MHV_PIN_PARMS(pin), this);
}

/**
 * Deassign a pin
 * @param	pinPinchangeInterrupt		the pin change interrupt to deregister
 */
void MHV_Debounce::deassignKey(int8_t pinPinchangeInterrupt) {
	if (pinPinchangeInterrupt) {
		initPin(pinPinchangeInterrupt);
	}
}
