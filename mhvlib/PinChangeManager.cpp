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
#include <mhvlib/PinChangeManager.h>

namespace mhvlib {

/**
 * An event manager for handling pinchange events
 */

PinChangeManager::PinChangeManager() {
	for (uint8_t i = 0; i < MHV_PC_INT_COUNT; i++) {
		_pins[i].listener = NULL;
	}
}

/**
 * Trigger for interrupt PCINT0
 */
void PinChangeManager::pinChange0() {
	pinChange(0);
}

#if MHV_PC_INT_COUNT > 7
/**
 * Trigger for interrupt PCINT1
 */
void PinChangeManager::pinChange1() {
	pinChange(8);
}
#endif

#if MHV_PC_INT_COUNT > 15
/**
 * Trigger for interrupt PCINT2
 */
void PinChangeManager::pinChange2() {
	pinChange(16);
}
#endif

/**
 * Trigger for pin change interrupts - scans through 8 pins starting at the offset
 * @param	offset	the number of pins to skip before scanning
 */
void PinChangeManager::pinChange(uint8_t offset) {
	EVENT_PIN *pin = _pins + offset;
	EVENT_PIN *maxPin = pin + 8;

	for (; pin < maxPin; ++pin) {
		if (NULL == pin->listener) {
			continue;
		}

		bool cur = _MMIO_BYTE(pin->port) & pin->mask;
		if (cur != pin->previous) {
			// Pin has changed
			pin->previous = cur;
			pin->changed = 1;
			pin->listener->pinChanged(pin - _pins, cur);
		}
	}
}

/**
 * Register interest for pinchange events
 * @param	pin			A MHV_PIN_* macro, must have a valid pinPinchangeInterrupt
 * @param	listener	a MHV_PinEventListener to notify when the pin changes
 */
void PinChangeManager::registerListener(MHV_DECLARE_PIN(pin), PinEventListener *listener) {

	setInput(MHV_PIN_PARMS(pin));

	_pins[pinPinchangeInterrupt].port = pinIn;
	_pins[pinPinchangeInterrupt].mask = _BV(pinPin);
	_pins[pinPinchangeInterrupt].listener = listener;
	_pins[pinPinchangeInterrupt].previous = pinRead(MHV_PIN_PARMS(pin));
	_pins[pinPinchangeInterrupt].changed = false;

	// Enable the interrupt
	uint8_t bit = pinPinchangeInterrupt;
#if MHV_PC_INT_COUNT > 15
	if (pinPinchangeInterrupt > 15) {
		bit -= 16;
		PCMSK2 |= _BV(bit);
		PCICR |= _BV(PCIE2);
	} else
#endif
#if MHV_PC_INT_COUNT > 7
	if (pinPinchangeInterrupt > 7) {
		bit -= 8;
		PCMSK1 |= _BV(bit);
		PCICR |= _BV(PCIE1);
	} else
#endif
#ifdef PCMSK0
	{
		PCMSK0 |= _BV(bit);
		PCICR |= _BV(PCIE0);
	}
#else
	{
		PCMSK |= _BV(bit);
		GIMSK |= _BV(PCIE);
	}
#endif
}

/**
 * Deregister interest for pinchange events
 * @param	pinPinchangeInterrupt			the pinPinchangeInterrupt to deregister
 */
void PinChangeManager::deregisterListener(int8_t pinPinchangeInterrupt) {
	if (pinPinchangeInterrupt >= MHV_PC_INT_COUNT) {
		return;
	}

	_pins[pinPinchangeInterrupt].listener = NULL;
	_pins[pinPinchangeInterrupt].changed = false;

// Disable the interrupt
	uint8_t bit = pinPinchangeInterrupt;
#if MHV_PC_INT_COUNT > 15
	if (pinPinchangeInterrupt > 15) {
		bit -= 16;
		PCMSK2 &= ~_BV(bit);
	} else
#endif
#if MHV_PC_INT_COUNT > 7
	if (pinPinchangeInterrupt > 7) {
		bit -= 8;
		PCMSK1 &= ~_BV(bit);
	} else
#endif
#ifdef PCMSK0
	{
		PCMSK0 &= ~_BV(bit);
	}
#else
	{
		PCMSK &= ~_BV(bit);
	}
#endif
}

/**
 * Call from the main loop to handle any events
 */
void PinChangeManager::handleEvents() {
	uint8_t i;

	for (i = 0; i < MHV_PC_INT_COUNT; i++) {
		if (_pins[i].changed) {
			_pins[i].changed = 0;
			_pins[i].listener->pinChanged(i, _pins[i].previous);
		}
	}
}

}
