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

struct mhv_debouncePin {
	volatile uint8_t	*port;
	uint8_t				mask;
	uint8_t				previous;
	MHV_TIMESTAMP		timestamp;
	void 				(*singlePress)(MHV_TIMESTAMP *heldFor, void *data);
	void 				(*heldDown)(MHV_TIMESTAMP *heldFor, void *data);
	void				*data;
	bool				held;
};
typedef struct mhv_debouncePin MHV_DEBOUNCE_PIN;

class MHV_Debounce {
private:
	MHV_RTC				*_rtc;
	MHV_DEBOUNCE_PIN	_pins[MHV_PC_INT_COUNT];
	MHV_TIMESTAMP		_debounceTime;
	MHV_TIMESTAMP		_heldTime;
	MHV_TIMESTAMP		_repeatTime;

	void pinChange(uint8_t offset);
	void initPin(uint8_t pinchangeInterrupt);

public:
	MHV_Debounce(MHV_RTC *rtc, uint16_t debounceTime, uint16_t heldTime, uint16_t repeatTime);
#if MHV_PC_INT_COUNT > 15
	void pinChange2();
#endif
#if MHV_PC_INT_COUNT > 7
	void pinChange1();
#endif
	void pinChange0();
	void assignKey(volatile uint8_t *dir, volatile uint8_t *out, volatile uint8_t *in,
			uint8_t pin, int8_t pinchangeInterrupt,
			void (*singlePress)(MHV_TIMESTAMP *heldFor, void *data),
			void (*heldDown)(MHV_TIMESTAMP *heldFor, void *data),
			void *data);
	void deassignKey(volatile uint8_t *dir, volatile uint8_t *out, volatile uint8_t *in,
		uint8_t pin, int8_t pinchangeInterrupt);
	void checkHeld();
};

#endif /* MHV_DEBOUNCE_H_ */
