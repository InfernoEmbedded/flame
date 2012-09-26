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

namespace mhvlib {

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

/**
 * A debouncing class triggered by pin changes
 * @tparam	debounceTime		the minimum amount of time to count as a button press (in milliseconds)
 * @tparam	heldTime			the minimum amount of time to consider a button held down
 * @tparam	repeatTime			the time after which the held call repeats
 */
template <uint16_t debounceTime, uint16_t heldTime, uint16_t repeatTime>
class Debounce : PinEventListener {
protected:
	RTC					&_rtc;
	DEBOUNCE_PIN		_pins[MHV_PC_INT_COUNT];
	PinChangeManager	&_pinChangeManager;

	/**
	 * Trigger for pin change interrupts - scans through 8 pins starting at the offset
	 * @param	pcInt		the pin change interrupt that was triggered
	 * @param	newState	the new state of the pin
	 */
	void pinChanged(uint8_t pcInt, bool newState) {
		if (newState) {
	// Pin has been released
			TIMESTAMP heldFor;

	/* Call the singlePress function if the time is greater than the debounce time,
	 * and the button has not been held down
	 */
			_rtc.elapsed(_pins[pcInt].timestamp, heldFor);
			uint16_t heldMilliseconds = getMilliseconds(heldFor);
			if (!_pins[pcInt].held && heldMilliseconds >= debounceTime) {
				_pins[pcInt].listener->singlePress(pcInt, &heldFor);
			}

			_pins[pcInt].held = false;
			_pins[pcInt].timestamp.timestamp = 0;
		} else {
	// Pin has been pressed
			_rtc.current(_pins[pcInt].timestamp);
		}
	}

	void initPin(uint8_t pinchangeInterrupt) {
		memClear(_pins + pinchangeInterrupt, MHV_BYTESIZEOF(*_pins), (uint8_t)1);
	}

public:
	/**
	 * Debouncing helper for buttons connected directly to PCINT capable pins
	 * The user must pull the pin up, either externally (and initing the pin by calling mhv_setInput),
	 * or internally by calling mhv_setInputPullup
	 *
	 * @param	pinChangeManager	the pin change manager
	 * @param	rtc					the realtime clock we will use for timing
	 */
	Debounce(PinChangeManager &pinChangeManager, RTC &rtc) :
				_rtc(rtc),
				_pinChangeManager(pinChangeManager) {

		for (uint8_t i = 0; i < MHV_PC_INT_COUNT; ++i) {
			initPin(i);
		}
	}

	/**
	 * Assign a pin to debounce
	 * @param	pin						An MHV_PIN_* macro
	 * @param	listener				a class to call when the button is pressed or held down
	 */
	void assignKey(MHV_DECLARE_PIN(pin), DebounceListener &listener) {
		_pins[pinPinchangeInterrupt].previous = _MMIO_BYTE(pinIn) & _BV(pinPin);
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
	void deassignKey(int8_t pinPinchangeInterrupt) {
		if (pinPinchangeInterrupt) {
			initPin(pinPinchangeInterrupt);
		}
	}
	/**
	 * Called periodically to check if pins have been held
	 * Ideally, this should be called from the main loop, rather than the interrupt context
	 */
	void checkHeld() {
		DEBOUNCE_PIN *pin = _pins;
		DEBOUNCE_PIN *maxPin = pin + MHV_PC_INT_COUNT;

		for (; pin < maxPin; ++pin) {
			if (pin->timestamp.timestamp && !pin->previous) {
	// Pin is currently held down
				TIMESTAMP heldFor;
				_rtc.elapsed(pin->timestamp, heldFor);
				uint16_t heldMilliseconds = getMilliseconds(heldFor);

				if (!pin->held && heldMilliseconds >= heldTime) {
					pin->held = true;
					pin->listener->heldDown(pin - _pins, &heldFor);
					_rtc.current(pin->timestamp);
				} else if (pin->held && heldMilliseconds >= repeatTime) {
					pin->listener->heldDown(pin - _pins, &heldFor);
					_rtc.current(pin->timestamp);
				}
			}
		}
	}
};

}
#endif /* MHV_DEBOUNCE_H_ */