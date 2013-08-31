/*
 * Copyright (c) 2012, Make, Hack, Void Inc
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

#ifndef MHV_PINCHANGE_MANAGER_H_
#define MHV_PINCHANGE_MANAGER_H_

#include <mhvlib/io.h>
#include <mhvlib/Pin.h>

#define MHV_PINCHANGE_MANAGER_ASSIGN_PCINT(___mhvEventManager) \
ISR(PCINT_vect) { \
	___mhvEventManager.pinChange0(); \
}

#define MHV_PINCHANGE_MANAGER_ASSIGN_PCINT0(___mhvEventManager) \
ISR(PCINT0_vect) { \
	___mhvEventManager.pinChange0(); \
}

#define MHV_PINCHANGE_MANAGER_ASSIGN_PCINT1(___mhvEventManager) \
ISR(PCINT1_vect) { \
	___mhvEventManager.pinChange1(); \
}

#define MHV_PINCHANGE_MANAGER_ASSIGN_PCINT2(___mhvEventManager) \
ISR(PCINT2_vect) { \
	___mhvEventManager.pinChange2(); \
}

#if MHV_PC_INT_COUNT > 15
#define MHV_PINCHANGE_MANAGER_ASSIGN_INTERRUPTS(__mhvEventManager) \
		MHV_PINCHANGE_MANAGER_ASSIGN_PCINT2(__mhvEventManager) \
		MHV_PINCHANGE_MANAGER_ASSIGN_PCINT1(__mhvEventManager) \
		MHV_PINCHANGE_MANAGER_ASSIGN_PCINT0(__mhvEventManager)
#elif MHV_PC_INT_COUNT > 7
#define MHV_PINCHANGE_MANAGER_ASSIGN_INTERRUPTS(__mhvEventManager) \
		MHV_PINCHANGE_MANAGER_ASSIGN_PCINT1(__mhvEventManager) \
		MHV_PINCHANGE_MANAGER_ASSIGN_PCINT0(__mhvEventManager)
#elif defined PCINT0_vect
#define MHV_PINCHANGE_MANAGER_ASSIGN_INTERRUPTS(__mhvEventManager) \
		MHV_PINCHANGE_MANAGER_ASSIGN_PCINT0(__mhvEventManager)
#else
#define MHV_PINCHANGE_MANAGER_ASSIGN_INTERRUPTS(__mhvEventManager) \
		MHV_PINCHANGE_MANAGER_ASSIGN_PCINT(__mhvEventManager)
#endif



namespace mhvlib {

class PinEventListener {
public:
	virtual void pinChanged(uint8_t pcInt, bool newState) =0;
};

struct eventPin {
	mhv_register			port;
	uint8_t					mask;
	uint8_t					pcInt;
	PinEventListener	*listener;
	bool					previous;
	bool					changed;
};
typedef struct eventPin EVENT_PIN;

class PinChangeManager {
protected:
// Fixme: Allocate externally and pass in
	EVENT_PIN	_pins[MHV_PC_INT_COUNT];
//	uint8_t			_pinsUsed;
	void enablepinPinChangeInterrupt(uint8_t pinPinchangeInterrupt);
	void disablepinPinChangeInterrupt(const uint8_t pinPinchangeInterrupt);
	virtual void pinChangeCaught(EVENT_PIN pin, bool newval);

public:
	PinChangeManager();

	void pinChange0();
#if MHV_PC_INT_COUNT > 7
	void pinChange1();
#endif
#if MHV_PC_INT_COUNT > 7
	void pinChange2();
#endif
	void pinChange(uint8_t offset);

	void registerListener(MHV_DECLARE_PIN(pin), PinEventListener *listener);
	void registerListener(Pin &pin, PinEventListener *listener);
	void deregisterListener(int8_t pinPinChangeListener);

	void registerListener(MHV_PIN *x, PinEventListener * listener);
	void deregisterListener(MHV_PIN *_mhv_pin);

	void handleEvents();

};

}
#endif /* MHV_PINCHANGEMANAGER_H_ */
