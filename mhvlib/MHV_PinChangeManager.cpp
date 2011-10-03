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
#include <MHV_PinChangeManager.h>

/**
 * An event manager for handling pinchange events
 */

MHV_PinChangeManager::MHV_PinChangeManager() {
	for (uint8_t i = 0; i < MHV_PC_INT_COUNT; i++) {
		_pins[i].port = NULL;
	}
}

/**
 * Trigger for interrupt PCINT0
 */
void MHV_PinChangeManager::pinChange0() {
	pinChange(0);
}

#if MHV_PC_INT_COUNT > 7
/**
 * Trigger for interrupt PCINT1
 */
void MHV_PinChangeManager::pinChange1() {
	pinChange(8);
}
#endif

#if MHV_PC_INT_COUNT > 15
/**
 * Trigger for interrupt PCINT2
 */
void MHV_PinChangeManager::pinChange2() {
	pinChange(16);
}
#endif

/**
 * Trigger for pin change interrupts - scans through 8 pins starting at the offset
 * @param	offset	the number of pins to skip before scanning
 */
void MHV_PinChangeManager::pinChange(uint8_t offset) {
	MHV_EVENT_PIN *pin = _pins + offset;
	MHV_EVENT_PIN *maxPin = pin + 8;

	for (; pin < maxPin; ++pin) {
		if (NULL == pin->port) {
			continue;
		}

		bool cur = *(pin->port) & pin->mask;
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
 * @param	pin*		an MHV_PIN declaration
 * @param	listener	a MHV_PinEventListener to notify when the pin changes
 */
void MHV_PinChangeManager::registerListener(volatile uint8_t *pinDir, volatile uint8_t *pinOut,
		volatile uint8_t *pinIn, uint8_t pinBit, int8_t pinChangeInterrupt,
		MHV_PinEventListener *listener) {

	mhv_setInput(pinDir, pinOut, pinIn, pinBit, pinChangeInterrupt);

	_pins[pinChangeInterrupt].port = pinIn;
	_pins[pinChangeInterrupt].mask = _BV(pinBit);
	_pins[pinChangeInterrupt].listener = listener;
	_pins[pinChangeInterrupt].previous = mhv_pinRead(pinDir, pinOut, pinIn, pinBit, pinChangeInterrupt);
	_pins[pinChangeInterrupt].changed = false;

	// Enable the interrupt
	uint8_t bit = pinChangeInterrupt;
#if MHV_PC_INT_COUNT > 15
	if (pinChangeInterrupt > 15) {
		bit -= 16;
		PCMSK2 |= _BV(bit);
		PCICR |= _BV(PCIE2);
	} else
#endif
#if MHV_PC_INT_COUNT > 7
	if (pinChangeInterrupt > 7) {
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
 * @param	pin*		an MHV_PIN declaration
 */
void MHV_PinChangeManager::deregisterListener(volatile uint8_t *pinDir, volatile uint8_t *pinOut,
		volatile uint8_t *pinIn, uint8_t pinBit, int8_t pinChangeInterrupt) {

	_pins[pinChangeInterrupt].listener = NULL;
	_pins[pinChangeInterrupt].changed = false;

// Disable the interrupt
	uint8_t bit = pinChangeInterrupt;
#if MHV_PC_INT_COUNT > 15
	if (pinChangeInterrupt > 15) {
		bit -= 16;
		PCMSK2 &= ~_BV(bit);
	} else
#endif
#if MHV_PC_INT_COUNT > 7
	if (pinChangeInterrupt > 7) {
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
void MHV_PinChangeManager::handleEvents() {
	uint8_t i;

	for (i = 0; i < MHV_PC_INT_COUNT; i++) {
		if (_pins[i].changed) {
			_pins[i].changed = 0;
			_pins[i].listener->pinChanged(i, _pins[i].previous);
		}
	}
}
