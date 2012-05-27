/*
 * MHV_Debounce.h
 *
 *  Created on: 12/10/2010
 *      Author: Deece
 */

#ifndef MHV_DEBOUNCE_H_
#define MHV_DEBOUNCE_H_

#include <string.h>

#include <mhvlib/RTC.h>
#include <mhvlib/io.h>
#include <mhvlib/PinChangeManager.h>


#define MHV_DEBOUNCE_ASSIGN_PCINT(__mhvDebounce) \
ISR(PCINT_vect) { \
	__mhvDebounce.pinChange0(); \
}

#define MHV_DEBOUNCE_ASSIGN_PCINT0(__mhvDebounce) \
ISR(PCINT0_vect) { \
	__mhvDebounce.pinChange0(); \
}

#define MHV_DEBOUNCE_ASSIGN_PCINT1(__mhvDebounce) \
ISR(PCINT1_vect) { \
	__mhvDebounce.pinChange1(); \
}

#define MHV_DEBOUNCE_ASSIGN_PCINT2(__mhvDebounce) \
ISR(PCINT2_vect) { \
	__mhvDebounce.pinChange2(); \
}


#if MHV_PC_INT_COUNT > 15
#define MHV_DEBOUNCE_ASSIGN_INTERRUPTS(_mhvDebounce) \
		MHV_DEBOUNCE_ASSIGN_PCINT2(_mhvDebounce) \
		MHV_DEBOUNCE_ASSIGN_PCINT1(_mhvDebounce) \
		MHV_DEBOUNCE_ASSIGN_PCINT0(_mhvDebounce)
#elif MHV_PC_INT_COUNT > 7
#define MHV_DEBOUNCE_ASSIGN_INTERRUPTS(_mhvDebounce) \
		MHV_DEBOUNCE_ASSIGN_PCINT1(_mhvDebounce) \
		MHV_DEBOUNCE_ASSIGN_PCINT0(_mhvDebounce)
#elif defined PCINT0_vect
#define MHV_DEBOUNCE_ASSIGN_INTERRUPTS(mhvDebounce) \
		MHV_DEBOUNCE_ASSIGN_PCINT0(_mhvDebounce)
#else
#define MHV_DEBOUNCE_ASSIGN_INTERRUPTS(mhvDebounce) \
		MHV_DEBOUNCE_ASSIGN_PCINT(_mhvDebounce)
#endif

namespace mhvlib_bsd {

class DebounceListener {
public:
	virtual void singlePress(uint8_t pcInt, TIMESTAMP *heldFor) =0;
	virtual void heldDown(uint8_t pcInt, TIMESTAMP *heldFor) =0;
};

struct debouncePin {
	uint8_t				previous;
	TIMESTAMP			timestamp;
	DebounceListener	*listener;
	bool				held;
};
typedef struct debouncePin DEBOUNCE_PIN;

class Debounce : PinEventListener {
protected:
	RTC					&_rtc;
	DEBOUNCE_PIN		_pins[MHV_PC_INT_COUNT];
	TIMESTAMP			_debounceTime;
	TIMESTAMP			_heldTime;
	TIMESTAMP			_repeatTime;
	PinChangeManager	&_pinChangeManager;

	void pinChanged(uint8_t pcInt, bool newState);
	void initPin(uint8_t pinchangeInterrupt);

public:
	Debounce(PinChangeManager &pinChangeManager, RTC &rtc, uint16_t debounceTime, uint16_t heldTime, uint16_t repeatTime);
	void assignKey(MHV_DECLARE_PIN(pin), DebounceListener &listener);
	void deassignKey(int8_t pinPinChangeInterrupt);
	void checkHeld();
};

}
#endif /* MHV_DEBOUNCE_H_ */
