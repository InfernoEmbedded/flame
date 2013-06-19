/*
 * MHV_Debounce2.h
 *
 *  Created on: 14/06/2013
 *      Author: pbarker
 */

#ifndef MHV_DEBOUNCE2_H_
#define MHV_DEBOUNCE2_H_

#include <string.h>

#include <mhvlib/RTC.h>
#include <mhvlib/io.h>
#include <mhvlib/PinChangeManager.h>

namespace mhvlib {

class Debounce2Listener : public PinEventListener {
public:
	virtual void pinChanged(uint8_t pcInt, bool newState) =0;
};

#define MAX_DEBOUNCES 20

/**
 * A debouncing class triggered by pin changes
 * @tparam	debounce2Time		the minimum amount of time to count as a button press (in milliseconds)
 * @tparam	heldTime			the minimum amount of time to consider a button held down
 * @tparam	repeatTime			the time after which the held call repeats
 */
template <uint16_t debounce2Time>
class Debounce2 : public PinChangeManager, public TimerListener {
		EVENT_PIN * debounced[MAX_DEBOUNCES];
		TIMESTAMP debounced_reenable_time[MAX_DEBOUNCES];
	RTC					&_rtc;

public:

	Debounce2(RTC &rtc) : _rtc(rtc) {
	};


	void pinChangeCaught(EVENT_PIN &pin) {
		PinChangeManager::disablepinPinChangeInterrupt(pin.pcInt);
		PinChangeManager::pinChangeCaught(pin);


		// find an empty slot:
		uint8_t my_slot;
		bool turn_alarm_on = true;
		for(uint8_t i = 0; i<MAX_DEBOUNCES;i++) {
			if (debounced[my_slot] == NULL) {
				my_slot = i;
			} else {
				// the slot was non-null; already an alarm set
				turn_alarm_on = false;
			}
		}
		debounced[my_slot] = &pin;
		_rtc.current(debounced_reenable_time[my_slot]);
		_rtc.timestampIncrement(debounced_reenable_time[my_slot],0,debounce2Time,0);

		if (turn_alarm_on == true) {
			// enable a-once-per-5-ms timer to reenable things
			_rtc.addAlarm(this,0,5,0,5);
		}
	}

		void alarm() {
			TIMESTAMP now;
			bool turn_alarm_off = true;
			_rtc.current(now);
			for (uint8_t i = 0; i< MAX_DEBOUNCES; i++) {
				if (debounced[i] != NULL) {
					if (timestampGreaterThanOrEqual(now,debounced_reenable_time[i])) {
						EVENT_PIN *pin = debounced[i];
						// may need to send another message due to
						// change in state while interrupt was down:
						bool now = _MMIO_BYTE(pin->port) & pin->mask;
			
						if (now == pin->previous) {
							// reenable interrupt:
							debounced[i] = NULL;
							PinChangeManager::enablepinPinChangeInterrupt(pin->pcInt);
						} else { // just send another message about the current value; debounce it again
							PinChangeManager::pinChangeCaught(*pin);
							_rtc.current(debounced_reenable_time[i]);
							_rtc.timestampIncrement(debounced_reenable_time[i],0,debounce2Time,0);
							turn_alarm_off = false;
						}
					} else {
						turn_alarm_off = false;
					}
				}
			}
			if (turn_alarm_off) {
				_rtc.removeAlarm(this);
			}
		}
	
};
} // end mhvlib namespace
#endif /* MHV_DEBOUNCE2_H_ */
