/*
 * Copyright (c) 2010, Make, Hack, Void Inc
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

#include <MHV_RTC.h>
#include <avr/pgmspace.h>
#include <string.h> // Needed for memmove

#include <MHV_HardwareSerial.h>
extern MHV_HardwareSerial serial;

/**
 * A Realtime clock
 *
 * Takes a trigger from a timer, and keeps time.
 *
 * Recommended values:
 *	Clock	Prescaler	Top		Ticks/ms
 *  16MHz	64			249		1
 *
 */


// First some helper functions

/**
 *  Compare 2 timestamps
 * @param	first	the first timestamp
 * @param	second	the second timestamp
 * @return true if the first timestamp is less than the second
 */
bool mhv_timestampLessThan(MHV_TIMESTAMP *first, MHV_TIMESTAMP *second) {
	if (first->timestamp < second->timestamp) {
		return true;
	}

	if (first->timestamp == second->timestamp) {
		if (first->milliseconds < second->milliseconds) {
				return true;
			}
	}

	return false;
}

/**
 * Compare 2 timestamps
 * @param	first	the first timestamp
 * @param	second	the second timestamp
 * @return true if the first timestamp is greater than or equal to the second
 */
bool mhv_timestampGreaterThanOrEqual(MHV_TIMESTAMP *first, MHV_TIMESTAMP *second) {
	if (first->timestamp > second->timestamp) {
		return true;
	}

	if (first->timestamp == second->timestamp) {
		if (first->milliseconds >= second->milliseconds) {
				return true;
			}
	}

	return false;
}

/**
 *  Determine if a year is a leap year
 */
bool mhv_isLeapYear(uint16_t year) {
	if ((0 == year % 4 && year % 100) || 0 == year % 400) {
//	if ((year & 0x03   && year % 100) || 0 == year % 400) {
		return true;
	}
	return false;
}

/**
 * Increment a timestamp
 * @param timestamp the timestamp to increment
 * @param seconds the number of seconds to increment by
 * @param milliseconds the number of milliseconds to increment by
 */
void mhv_timestampIncrement(MHV_TIMESTAMP *timestamp, uint32_t seconds, uint16_t milliseconds) {
	timestamp->milliseconds += milliseconds;

	while (timestamp->milliseconds >= 1000) {
		timestamp->timestamp++;
		timestamp->milliseconds -= 1000;
	}

	timestamp->timestamp += seconds;
}

uint8_t mhv_daysInMonthArray[] PROGMEM = {
		31,	// Jan
		28,	// Feb
		31,	// Mar
		30, // Apr
		31, // May
		30, // Jun
		31, // Jul
		30, // Aug
		30, // Sep
		31, // Oct
		30, // Nov
		31 // Dec
};

/**
 * Get the number of days in a month
 * @param month the month
 * @param year the year
 * @return the number of days
 */
uint8_t mhv_daysInMonth(MHV_MONTH month, uint16_t year) {
	if (MHV_FEBRUARY == month && mhv_isLeapYear(year)) {
		return 29;
	}

	return pgm_read_byte(mhv_daysInMonthArray + month - 1);
}


/**
 * Create a new RTC
 * @param	timer		the timer this RTC is associated with
 * @param	eventBuffer	A buffer to store events until they are executed
 * @param	eventCount	The number of events that can be stored in the buffer
 * @param	timezone	minutes offset from UTC
 */
MHV_RTC::MHV_RTC(MHV_Timer8 *timer, MHV_EVENT *eventBuffer, uint8_t eventCount, int16_t timezone) {
	_timer = timer;

	_ticks = 0;
	_ticksPerMillisecond = 1;

	_tzOffset = timezone;

	int i;
	for (i = 0; i < eventCount; i++) {
		eventBuffer[i].when.timestamp = 0;
		eventBuffer[i].when.milliseconds = 0;
		eventBuffer[i].actionFunction = 0;
	}

	_events = eventBuffer;
	_eventMax = eventCount;
	_eventCount = 0;
}

/**
 *  Synchronise the ticksPerMillisecond with the timer (useful if you change the timer values)
 */
void MHV_RTC::synchronise(void) {
	uint32_t ticksPerMillisecond = F_CPU / _timer->getPrescalerMultiplier() / (_timer->getTop() + 1) / 1000;
	_ticksPerMillisecond = ticksPerMillisecond;
}

/**
 * Set the current time
 * @param timestamp		the current Unix timestamp
 * @param milliseconds	the current milliseconds offset
 */
void MHV_RTC::setTime(uint32_t timestamp, uint16_t milliseconds) {
	do {
		_milliseconds = milliseconds;
		_timestamp = timestamp;
	} while (milliseconds != _milliseconds); // Retry if the value changed while updating
}

/**
 * Set the current time
 * @param timestamp		the current Unix timestamp
 */
void MHV_RTC::setTime(MHV_TIMESTAMP *timestamp) {
	do {
		_milliseconds = timestamp->milliseconds;
		_timestamp = timestamp->timestamp;
	} while (timestamp->milliseconds != _milliseconds); // Retry if the value changed while updating
}


/**
 *  Tick from the timer module
 */
void MHV_RTC::tick(void) {
	if (++_ticks != _ticksPerMillisecond) {
		return;
	}

	_ticks = 0;

	_milliseconds++;
	if (_milliseconds > 999) {
		_milliseconds = 0;
		_timestamp++;
	}
}

/**
 *  Tick from the timer module that is exactly 1ms
 */
void MHV_RTC::tick1ms(void) {
	_milliseconds++;
	if (_milliseconds > 999) {
		_milliseconds = 0;
		_timestamp++;
	}
}

/**
 * Tick from the timer module, and run any pending events
 */
void MHV_RTC::tickAndRunEvents(void) {
	if (++_ticks != _ticksPerMillisecond) {
		return;
	}

	_ticks = 0;

	_milliseconds++;
	if (_milliseconds > 999) {
		_milliseconds = 0;
		_timestamp++;
	}

	runEvents();
}

/**
 * Tick from the timer module that is exactly 1ms, run any pending events
 */
void MHV_RTC::tick1msAndRunEvents(void) {
	_milliseconds++;
	if (_milliseconds > 999) {
		_milliseconds = 0;
		_timestamp++;
	}
}


/**
 * Get the current timestamp
 */
void MHV_RTC::current(MHV_TIMESTAMP *timestamp) {
	do {
		timestamp->milliseconds = _milliseconds;
		timestamp->timestamp = _timestamp;
	} while (timestamp->milliseconds != _milliseconds); // Retry if the value changed while updating
}

/**
 * Determine how long has elapsed since the supplied timestamp
 * @param	since		the timestamp to measure against
 * @param	elapsed		returns how much time has elapsed
 */
void MHV_RTC::elapsed(MHV_TIMESTAMP *since, MHV_TIMESTAMP *elapsed) {
	MHV_TIMESTAMP currentTimestamp;

	current(&currentTimestamp);

	elapsed->timestamp = currentTimestamp.timestamp - since->timestamp;
	if (currentTimestamp.milliseconds > since->milliseconds) {
		elapsed->milliseconds = currentTimestamp.milliseconds - since->milliseconds;
	} else {
		elapsed->timestamp--;
		elapsed->milliseconds = 1000 + currentTimestamp.milliseconds - since->milliseconds;
	}
}

// Used in toTime: Cumulative totals at the end of the month in a normal year
uint32_t mhv_secondsFromYearStart[] PROGMEM = {
		 2678400, 	// Jan
		 5097600,	// Feb
		 7776000,	// Mar
		10368000,	// Apr
		13046400,	// May
		15638400,	// Jun
		18316800,	// Jul
		20995200,	// Aug
		23587200,	// Sep
		26265600,	// Oct
		28857600,	// Nov
		31536000	// Dec
};


/**
 * Convert a timestamp into year,month,day,hours,minutes,seconds
 * @param to	the MHV_TIME to store the results
 * @param from	the MHV_TIMESTAMP struct to convert from
 */
void MHV_RTC::toTime(MHV_TIME *to, MHV_TIMESTAMP *from) {
	uint16_t year = 1970;
	bool leapYear = false;
	uint32_t seconds = from->timestamp + _tzOffset;

	to->timezone = _tzOffset;

	uint32_t secondsThisYear = 365 * 86400;
	uint32_t secondsSoFar = 0;
	while (seconds > secondsThisYear) {
		seconds -= secondsThisYear;
		secondsSoFar += secondsThisYear;

		if ((mhv_isLeapYear(++year))) {
			secondsThisYear = 366 * 86400;
		} else {
			secondsThisYear = 365 * 86400;
		}
	}
	to->year = year;

	uint8_t month;
	leapYear = mhv_isLeapYear(year);
	for (month = 0; month < sizeof(mhv_secondsFromYearStart); month++) {
		if (leapYear) {
			if (seconds < pgm_read_dword(&mhv_secondsFromYearStart[month] + (1 == month) ? 86400 : 0)) {
				break;
			}
		} else {
			if (seconds < pgm_read_dword(&mhv_secondsFromYearStart[month])) {
				break;
			}
		}
	}
	to->yearday = seconds / 86400;

	if (month) {
		seconds -= pgm_read_dword(&mhv_secondsFromYearStart[month - 1]);
	}

	if (leapYear && month > 1) {
		seconds -= 86400;
	}

	// Increment so we start numbering from 1
	to->month = (MHV_MONTH)(month + 1);

	uint32_t day = seconds / 86400;
	to->day = (uint8_t)day;
	seconds -= day * 86400;
	to->day += 1; // Increment so we start numbering from 1

	uint32_t hours = seconds / 3600;
	to->hours = (uint8_t)hours;
	seconds -= hours * 3600;

	uint32_t minutes = seconds / 60;
	to->minutes = (uint8_t)minutes;
	seconds -= minutes * 60;

	to->seconds = seconds;
	to->milliseconds = from->milliseconds;
}

/**
 * Convert year,month,day,hours,minutes,seconds into a timestamp
 * @param to	the MHV_TIMESTAMP to store the results
 * @param from	the MHV_TIME struct to convert from
 */
void MHV_RTC::toTimestamp(MHV_TIMESTAMP *to, MHV_TIME *from) {
	to->milliseconds = from->milliseconds;

	uint32_t seconds = from->seconds;

	seconds += (uint32_t)from->minutes * 60;

	seconds += (uint32_t)from->hours * 3600;

	if (from->yearday) {
		seconds += from->yearday * 86400;
	} else {
		seconds += (from->day - 1) * 86400;
		switch (from->month) {
		case MHV_JANUARY:
			break;
		case MHV_FEBRUARY:
			seconds += 5097600;
			break;
		default:
			seconds += pgm_read_dword(&mhv_secondsFromYearStart[(uint8_t)from->month - 2]);
			if (mhv_isLeapYear(from->year)) {
				seconds += 86400;
			}
			break;
		}
	}

	for (uint16_t year = from->year - 1; year >= 1970; year--) {
		seconds += 365 * 86400;
		if (mhv_isLeapYear(year)) {
			seconds += 86400;
		}
	}

	to->timestamp = seconds - _tzOffset;
}


/**
 * Insert an event, to be triggered at a later date
 * @param	event	the event, consisting of:
 * 						when it should be triggered
 * 						what should be called (it will be passed a pointer to the event)
 * 						a pointer to user-defined data
 * @return	true if the event could not be added
 */
bool MHV_RTC::addEvent(MHV_EVENT *event) {
	if (_eventCount == _eventMax) {
		return true;
	}

	// Figure out where it needs to be inserted
	int i;
	for (i = 0; i < _eventCount; ++i) {
		if (mhv_timestampLessThan(&(event->when), &(_events[i].when))) {
			break;
		}
	}
	// i now is the offset of where the timestamp should be inserted
	if (i < _eventCount) {
		memmove(&(_events[i+1]), &(_events[i]), (_eventCount - i) * sizeof(*_events));
		memcpy(&(_events[i]), event, sizeof(*_events));
	} else {
		memcpy(&(_events[_eventCount]), event, sizeof(*_events));
	}

	_eventCount++;

	return false;
}

/**
 * Check for events that are due, and run them
 * Run this from your main loop if you have no blocking calls, otherwise, call
 * tickAndRunEvents instead of tick from the timer (note that this will run your
 * events in the interrupt handler, so keep them short!)
 */
void MHV_RTC::runEvents(void) {
	int i;
	MHV_TIMESTAMP timestamp;
	current(&timestamp);

	// Run any events pending
	for (i = 0; i < _eventCount && mhv_timestampGreaterThanOrEqual(&timestamp, &(_events[i].when));
			i++, current(&timestamp)) {
		_events[i].actionFunction(&(_events[i]));
	}

	if (0 == i) {
		return;
	}

	// Shift remaining events down
	if (i < _eventCount) {
		memmove(_events, &(_events[i]), (_eventCount - i) * sizeof(*_events));
	}

	_eventCount -= i;
}

/**
 * How many events are left in the queue
 * @return the number of events
 */
uint8_t MHV_RTC::eventsPending() {
	return _eventCount;
}
