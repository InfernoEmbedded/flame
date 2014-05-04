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
#include <flame/PinChangeManager.h>

namespace flame {

/**
 * An event manager for handling pinchange events
 */

PinChangeManager::PinChangeManager() {
	for (uint8_t i = 0; i < FLAME_PC_INT_COUNT; i++) {
		_pins[i].listener = NULL;
	}
}

/**
 * Trigger for interrupt PCINT0
 */
void PinChangeManager::pinChange0() {
	pinChange(0);
}

#if FLAME_PC_INT_COUNT > 7
/**
 * Trigger for interrupt PCINT1
 */
void PinChangeManager::pinChange1() {
	pinChange(8);
}
#endif

#if FLAME_PC_INT_COUNT > 15
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
			pinChangeCaught(*pin,cur);
		}
	}
}
	void PinChangeManager::pinChangeCaught(EVENT_PIN pin,bool newval) {
		if (pin.listener != NULL) {
			pin.listener->pinChanged(pin.pcInt, newval);
		} else {
			pin.changed = 1;
		}
}

void PinChangeManager::registerListener(MHV_PIN *x, PinEventListener *listener) {
  PinChangeManager::registerListener(MHV_PIN_TO_MHVPARMS(x),listener);
}
/**
 * Register interest for pinchange events
 * @param	pin			A FLAME_PIN_* macro, must have a valid pinPinchangeInterrupt
 * @param	listener	a FLAME_PinEventListener to notify when the pin changes
 */
void PinChangeManager::registerListener(FLAME_DECLARE_PIN(pin), PinEventListener *listener) {

	setInput(FLAME_PIN_PARMS(pin));

	_pins[pinPinchangeInterrupt].port = pinIn;
	_pins[pinPinchangeInterrupt].mask = _BV(pinPin);
	_pins[pinPinchangeInterrupt].listener = listener;
	_pins[pinPinchangeInterrupt].previous = pinRead(FLAME_PIN_PARMS(pin));
	_pins[pinPinchangeInterrupt].changed = false;
	_pins[pinPinchangeInterrupt].pcInt = pinPinchangeInterrupt;

	 enablepinPinChangeInterrupt(pinPinchangeInterrupt);
}
void PinChangeManager::enablepinPinChangeInterrupt(uint8_t pinPinchangeInterrupt) {
	// Enable the interrupt
	uint8_t bit = pinPinchangeInterrupt;
#if FLAME_PC_INT_COUNT > 15
	if (pinPinchangeInterrupt > 15) {
		bit -= 16;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		PCMSK2 |= _BV(bit);
		PCICR |= _BV(PCIE2);
		}
	} else
#endif
#if FLAME_PC_INT_COUNT > 7
	if (pinPinchangeInterrupt > 7) {
		bit -= 8;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		PCMSK1 |= _BV(bit);
		PCICR |= _BV(PCIE1);
		}
	} else
#endif
#ifdef PCMSK0
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		PCMSK0 |= _BV(bit);
		PCICR |= _BV(PCIE0);
		}
	}
#else
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		PCMSK |= _BV(bit);
		GIMSK |= _BV(PCIE);
		}
	}
#endif
}

/**
 * Register interest for pinchange events
 * @param	pin			The pin, must have a valid pinPinchangeInterrupt
 * @param	listener	a FLAME_PinEventListener to notify when the pin changes
 */
void PinChangeManager::registerListener(Pin &pin, PinEventListener *listener) {

	pin.setInput();

	_pins[pin.pinchangeInterrupt()].port = pin.inputPort();
	_pins[pin.pinchangeInterrupt()].mask = pin.mask();
	_pins[pin.pinchangeInterrupt()].listener = listener;
	_pins[pin.pinchangeInterrupt()].previous = pin.read();
	_pins[pin.pinchangeInterrupt()].changed = false;

	// Enable the interrupt
	uint8_t bit = pin.pinchangeInterrupt();
#if FLAME_PC_INT_COUNT > 15
	if (pin.pinchangeInterrupt() > 15) {
		bit -= 16;
		PCMSK2 |= _BV(bit);
		PCICR |= _BV(PCIE2);
	} else
#endif
#if FLAME_PC_INT_COUNT > 7
	if (pin.pinchangeInterrupt() > 7) {
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
void PinChangeManager::deregisterListener(const int8_t pinPinchangeInterrupt) {
	if (pinPinchangeInterrupt >= FLAME_PC_INT_COUNT) {
		return;
	}

	_pins[pinPinchangeInterrupt].listener = NULL;
	_pins[pinPinchangeInterrupt].changed = false;

	disablepinPinChangeInterrupt(pinPinchangeInterrupt);
}
void PinChangeManager::disablepinPinChangeInterrupt(const uint8_t pinPinchangeInterrupt) {
	uint8_t bit = pinPinchangeInterrupt;
#if FLAME_PC_INT_COUNT > 15
	if (pinPinchangeInterrupt > 15) {
		bit -= 16;
		PCMSK2 &= ~_BV(bit);
	} else
#endif
#if FLAME_PC_INT_COUNT > 7
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
 * Deregister interest for pinchange events
 * @param	_mhv_pin an MHV_PIN* currently listening
 */
void PinChangeManager::deregisterListener(MHV_PIN *_mhv_pin) {
  deregisterListener(_mhv_pin->pcInt);
}

/**
 * Call from the main loop to handle any events
 */
void PinChangeManager::handleEvents() {
	uint8_t i;

	for (i = 0; i < FLAME_PC_INT_COUNT; i++) {
		if (_pins[i].changed) {
			_pins[i].changed = 0;
			_pins[i].listener->pinChanged(i, _pins[i].previous);
		}
	}
}

}
