/*
 * MHV_Debounce.h
 *
 *  Created on: 12/10/2010
 *      Author: Deece
 */

#ifndef MHV_DEBOUNCE_H_
#define MHV_DEBOUNCE_H_

#include <string.h>

#include <MHV_RTC.h>
#include <MHV_io.h>
#include <MHV_PinChangeManager.h>


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

class MHV_DebounceListener {
public:
	virtual void singlePress(uint8_t pcInt, MHV_TIMESTAMP *heldFor) =0;
	virtual void heldDown(uint8_t pcInt, MHV_TIMESTAMP *heldFor) =0;
};

struct mhv_debouncePin {
	uint8_t					previous;
	MHV_TIMESTAMP			timestamp;
	MHV_DebounceListener	*listener;
	bool					held;
};
typedef struct mhv_debouncePin MHV_DEBOUNCE_PIN;

class MHV_Debounce : MHV_PinEventListener {
protected:
	MHV_RTC					&_rtc;
	MHV_DEBOUNCE_PIN		_pins[MHV_PC_INT_COUNT];
	MHV_TIMESTAMP			_debounceTime;
	MHV_TIMESTAMP			_heldTime;
	MHV_TIMESTAMP			_repeatTime;
	MHV_PinChangeManager	&_pinChangeManager;

	void pinChanged(uint8_t pcInt, bool newState);
	void initPin(uint8_t pinchangeInterrupt);

public:
	MHV_Debounce(MHV_PinChangeManager &pinChangeManager, MHV_RTC &rtc, uint16_t debounceTime, uint16_t heldTime, uint16_t repeatTime);
	void assignKey(MHV_DECLARE_PIN(pin), MHV_DebounceListener &listener);
	void deassignKey(MHV_DECLARE_PIN(pin));
	void checkHeld();
};

#endif /* MHV_DEBOUNCE_H_ */
