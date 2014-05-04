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

#ifndef FLAME_PINCHANGE_MANAGER_H_
#define FLAME_PINCHANGE_MANAGER_H_

#include <flame/io.h>
#include <flame/Pin.h>

#define FLAME_PINCHANGE_MANAGER_ASSIGN_PCINT(___flameEventManager) \
ISR(PCINT_vect) { \
	___flameEventManager.pinChange0(); \
}

#define FLAME_PINCHANGE_MANAGER_ASSIGN_PCINT0(___flameEventManager) \
ISR(PCINT0_vect) { \
	___flameEventManager.pinChange0(); \
}

#define FLAME_PINCHANGE_MANAGER_ASSIGN_PCINT1(___flameEventManager) \
ISR(PCINT1_vect) { \
	___flameEventManager.pinChange1(); \
}

#define FLAME_PINCHANGE_MANAGER_ASSIGN_PCINT2(___flameEventManager) \
ISR(PCINT2_vect) { \
	___flameEventManager.pinChange2(); \
}

#if FLAME_PC_INT_COUNT > 15
#define FLAME_PINCHANGE_MANAGER_ASSIGN_INTERRUPTS(__flameEventManager) \
		FLAME_PINCHANGE_MANAGER_ASSIGN_PCINT2(__flameEventManager) \
		FLAME_PINCHANGE_MANAGER_ASSIGN_PCINT1(__flameEventManager) \
		FLAME_PINCHANGE_MANAGER_ASSIGN_PCINT0(__flameEventManager)
#elif FLAME_PC_INT_COUNT > 7
#define FLAME_PINCHANGE_MANAGER_ASSIGN_INTERRUPTS(__flameEventManager) \
		FLAME_PINCHANGE_MANAGER_ASSIGN_PCINT1(__flameEventManager) \
		FLAME_PINCHANGE_MANAGER_ASSIGN_PCINT0(__flameEventManager)
#elif defined PCINT0_vect
#define FLAME_PINCHANGE_MANAGER_ASSIGN_INTERRUPTS(__flameEventManager) \
		FLAME_PINCHANGE_MANAGER_ASSIGN_PCINT0(__flameEventManager)
#else
#define FLAME_PINCHANGE_MANAGER_ASSIGN_INTERRUPTS(__flameEventManager) \
		FLAME_PINCHANGE_MANAGER_ASSIGN_PCINT(__flameEventManager)
#endif



namespace flame {

class PinEventListener {
public:
	virtual void pinChanged(uint8_t pcInt, bool newState) =0;
};

struct eventPin {
	FLAME_register			port;
	uint8_t					mask;
	uint8_t					pcInt;
	PinEventListener	*listener;
	bool					previous:1;
	bool					changed:1;
};
typedef struct eventPin EVENT_PIN;

class PinChangeManager {
protected:
// Fixme: Allocate externally and pass in
	EVENT_PIN	_pins[FLAME_PC_INT_COUNT];
//	uint8_t			_pinsUsed;
	void enablepinPinChangeInterrupt(uint8_t pinPinchangeInterrupt);
	void disablepinPinChangeInterrupt(const uint8_t pinPinchangeInterrupt);
	virtual void pinChangeCaught(EVENT_PIN pin, bool newval);

public:
	PinChangeManager();

	void pinChange0();
#if FLAME_PC_INT_COUNT > 7
	void pinChange1();
#endif
#if FLAME_PC_INT_COUNT > 7
	void pinChange2();
#endif
	void pinChange(uint8_t offset);
	void registerListener(FLAME_DECLARE_PIN(pin), PinEventListener *listener);
	void registerListener(Pin &pin, PinEventListener *listener);
	void registerListener(Pin *pin, PinEventListener *listener);
	void deregisterListener(int8_t pinPinChangeListener);

	void handleEvents();

};

}
#endif /* FLAME_PINCHANGEMANAGER_H_ */
