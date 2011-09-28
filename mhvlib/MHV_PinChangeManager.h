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

#ifndef MHV_PINCHANGE_MANAGER_H_
#define MHV_PINCHANGE_MANAGER_H_

#include <MHV_io.h>
#include <MHV_Device_RX.h>

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




class MHV_PinEventListener {
public:
	virtual void pinChanged(uint8_t pcInt, bool newState) =0;
};

struct mhv_eventPin {
	volatile uint8_t		*port;
	uint8_t					mask;
	uint8_t					pcInt;
	MHV_PinEventListener	*listener;
	bool					previous;
	bool					changed;
};
typedef struct mhv_eventPin MHV_EVENT_PIN;

class MHV_PinChangeManager {
protected:
// Fixme: Allocate externally and pass in
	MHV_EVENT_PIN	_pins[MHV_PC_INT_COUNT];
//	uint8_t			_pinsUsed;

public:
	MHV_PinChangeManager();

	void pinChange0();
#if MHV_PC_INT_COUNT > 7
	void pinChange1();
#endif
#if MHV_PC_INT_COUNT > 7
	void pinChange2();
#endif
	void pinChange(uint8_t offset);
	void registerListener(volatile uint8_t *pinDir, volatile uint8_t *pinOut,
			volatile uint8_t *pinIn, uint8_t pinBit, int8_t pinChangeInterrupt,
			MHV_PinEventListener *listener);
	void deregisterListener(volatile uint8_t *pinDir, volatile uint8_t *pinOut,
			volatile uint8_t *pinIn, uint8_t pinBit, int8_t pinChangeInterrupt);

	void handleEvents();

};

#endif /* MHV_PINCHANGEMANAGER_H_ */
