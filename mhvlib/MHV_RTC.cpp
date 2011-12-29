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

#include <MHV_RTC.h>
#include <avr/pgmspace.h>
#include <string.h> // Needed for memmove
#include <util/atomic.h>

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
bool mhv_timestampLessThan(const MHV_TIMESTAMP &first, const MHV_TIMESTAMP &second) {
	if (first.timestamp < second.timestamp) {
		return true;
	}

	if (first.timestamp == second.timestamp) {
		if (first.milliseconds < second.milliseconds) {
			return true;
		}

		if (first.milliseconds == second.milliseconds) {
			if (first.ticks < second.ticks) {
				return true;
			}
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
bool mhv_timestampGreaterThanOrEqual(const MHV_TIMESTAMP &first, const MHV_TIMESTAMP &second) {
	if (first.timestamp > second.timestamp) {
		return true;
	}

	if (first.timestamp == second.timestamp) {
		if (first.milliseconds > second.milliseconds) {
			return true;
		}

		if (first.ticks >= second.ticks) {
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
		return true;
	}
	return false;
}

/**
 * Increment a timestamp
 * @param timestamp		the timestamp to increment
 * @param seconds		the number of seconds to increment by
 * @param milliseconds	the number of milliseconds to increment by
 */
void mhv_timestampIncrement(MHV_TIMESTAMP &timestamp, uint32_t seconds, uint16_t milliseconds) {
	timestamp.milliseconds += milliseconds;

	while (timestamp.milliseconds >= 1000) {
		timestamp.timestamp++;
		timestamp.milliseconds -= 1000;
	}

	timestamp.timestamp += seconds;
}

/**
 * Increment a timestamp
 * @param timestamp		the timestamp to increment
 * @param timestamp2	the timestamp to increment by
 */
void MHV_RTC::timestampIncrement(MHV_TIMESTAMP &timestamp, const MHV_TIMESTAMP &timestamp2) {
	timestamp.ticks += timestamp2.ticks;
	timestamp.milliseconds += timestamp2.milliseconds;

	while (timestamp.ticks > _ticksPerMillisecond) {
		timestamp.milliseconds++;
		timestamp.ticks -= _ticksPerMillisecond;
	}

	while (timestamp.milliseconds >= 1000) {
		timestamp.timestamp++;
		timestamp.milliseconds -= 1000;
	}

	timestamp.timestamp += timestamp2.timestamp;
}

/**
 * Increment a timestamp
 * @param timestamp		the timestamp to increment
 * @param seconds		the number of seconds to increment by
 * @param milliseconds	the number of milliseconds to increment by
 * @param ticks			the number of ticks to increment by
 */
void MHV_RTC::timestampIncrement(MHV_TIMESTAMP &timestamp, uint32_t seconds, uint16_t milliseconds, uint8_t ticks) {
	timestamp.ticks += ticks;
	timestamp.milliseconds += milliseconds;

	while (timestamp.ticks > _ticksPerMillisecond) {
		timestamp.milliseconds++;
		timestamp.ticks -= _ticksPerMillisecond;
	}

	while (timestamp.milliseconds >= 1000) {
		timestamp.timestamp++;
		timestamp.milliseconds -= 1000;
	}

	timestamp.timestamp += seconds;
}


const uint8_t mhv_daysInMonthArray[] PROGMEM = {
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
 * @param	eventBuffer	A buffer to store events until they are executed
 * @param	eventCount	The number of events that can be stored in the buffer
 * @param	timezone	minutes offset from UTC
 */
MHV_RTC::MHV_RTC(MHV_ALARM eventBuffer[], uint8_t eventCount, int16_t timezone) :
		_alarms(eventBuffer),
		_alarmCount(0),
		_alarmMax(eventCount),
		_milliseconds(0),
		_ticks(0),
		_ticksPerMillisecond(1),
		_tzOffset(timezone) {
	mhv_memClear (eventBuffer, sizeof(*eventBuffer), eventCount);
}

/**
 *  Synchronise the ticksPerMillisecond with the timer
 *  This is useful if you change the timer values, or if you are are running the timer faster than 1ms
 *  @param	timer	the timer to sync with
 */
void MHV_RTC::synchronise(MHV_Timer8 &timer) {
	uint32_t ticksPerMillisecond = F_CPU / timer.getPrescalerMultiplier() / (timer.getTop() + 1) / 1000;
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
void MHV_RTC::setTime(MHV_TIMESTAMP &timestamp) {
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		_ticks = timestamp.ticks;
		_milliseconds = timestamp.milliseconds;
		_timestamp = timestamp.timestamp;
	}
}

/**
 * Increment milliseconds by 1
 */
inline void MHV_RTC::incrementMilliseconds() {
	_milliseconds++;

	if (_milliseconds > 999) {
		_milliseconds = 0;
		_timestamp++;
	}
}

/**
 * Alarm handler from the timer
 */
void MHV_RTC::alarm() {
	switch (_ticksPerMillisecond) {
	case 1:
		incrementMilliseconds();
		break;
	default:
		if (++_ticks != _ticksPerMillisecond) {
			return;
		}

		_ticks = 0;

		incrementMilliseconds();
		break;
	}
}

/**
 * Get the current timestamp
 * @param	timestamp	the timestamp to write into
 */
void MHV_RTC::current(MHV_TIMESTAMP &timestamp) {
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		timestamp.ticks = _ticks;
		timestamp.milliseconds = _milliseconds;
		timestamp.timestamp = _timestamp;
	}
}


/**
 * Determine how long has elapsed since the supplied timestamp
 * Fixme: Does not deal with ticks
 *
 * @param	since		the timestamp to measure against
 * @param	elapsed		returns how much time has elapsed
 */
void MHV_RTC::elapsed(const MHV_TIMESTAMP &since, MHV_TIMESTAMP &elapsed) {
	MHV_TIMESTAMP currentTimestamp;

	current(currentTimestamp);

	elapsed.timestamp = currentTimestamp.timestamp - since.timestamp;

	if (currentTimestamp.milliseconds > since.milliseconds) {
		elapsed.milliseconds = currentTimestamp.milliseconds - since.milliseconds;
	} else if (elapsed.timestamp) {
		elapsed.timestamp--;
		elapsed.milliseconds = 1000 + currentTimestamp.milliseconds - since.milliseconds;
	} else {
		elapsed.milliseconds = 0;
	}
}

// Used in toTime: Cumulative totals at the end of the month in a normal year
const uint32_t mhv_secondsFromYearStart[] PROGMEM = {
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
void MHV_RTC::toTime(MHV_TIME &to, const MHV_TIMESTAMP &from) {
	uint16_t year = 1970;
	bool leapYear = false;
	uint32_t seconds = from.timestamp + _tzOffset;

	to.timezone = _tzOffset;

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
	to.year = year;

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
	to.yearday = seconds / 86400;

	if (month) {
		seconds -= pgm_read_dword(&mhv_secondsFromYearStart[month - 1]);
	}

	if (leapYear && month > 1) {
		seconds -= 86400;
	}

	// Increment so we start numbering from 1
	to.month = (MHV_MONTH)(month + 1);

	uint32_t day = seconds / 86400;
	to.day = (uint8_t)day;
	seconds -= day * 86400;
	to.day += 1; // Increment so we start numbering from 1

	uint32_t hours = seconds / 3600;
	to.hours = (uint8_t)hours;
	seconds -= hours * 3600;

	uint32_t minutes = seconds / 60;
	to.minutes = (uint8_t)minutes;
	seconds -= minutes * 60;

	to.seconds = seconds;
	to.milliseconds = from.milliseconds;
}

/**
 * Convert year,month,day,hours,minutes,seconds into a timestamp
 * @param to	the MHV_TIMESTAMP to store the results
 * @param from	the MHV_TIME struct to convert from
 */
void MHV_RTC::toTimestamp(MHV_TIMESTAMP &to, const MHV_TIME &from) {
	to.milliseconds = from.milliseconds;

	uint32_t seconds = from.seconds;

	seconds += (uint32_t)from.minutes * 60;

	seconds += (uint32_t)from.hours * 3600;

	if (from.yearday) {
		seconds += from.yearday * 86400;
	} else {
		seconds += (from.day - 1) * 86400;
		switch (from.month) {
		case MHV_JANUARY:
			break;
		case MHV_FEBRUARY:
			seconds += 5097600;
			break;
		default:
			seconds += pgm_read_dword(&mhv_secondsFromYearStart[(uint8_t)from.month - 2]);
			if (mhv_isLeapYear(from.year)) {
				seconds += 86400;
			}
			break;
		}
	}

	for (uint16_t year = from.year - 1; year >= 1970; year--) {
		seconds += 365 * 86400;
		if (mhv_isLeapYear(year)) {
			seconds += 86400;
		}
	}

	to.timestamp = seconds - _tzOffset;
	to.ticks = 0;
}


/**
 * Insert an alarm, to be triggered at a later time
 * @param	alarm	the alarm, consisting of:
 * 						when it should be triggered
 * 						how often it should be repeated
 * 						what should be called (it will be passed a reference to the alarm)
 * @return	true if the event could not be added
 */
bool MHV_RTC::addAlarm(MHV_ALARM &alarm) {
	if (_alarmCount == _alarmMax) {
		return true;
	}

	// Figure out where it needs to be inserted
	uint8_t i;
	for (i = 0; i < _alarmCount; ++i) {
		if (mhv_timestampLessThan(alarm.when, _alarms[i].when)) {
			break;
		}
	}
	// i now is the offset of where the timestamp should be inserted
	if (i < _alarmCount) {
		memmove(_alarms + i + 1, _alarms + i, (_alarmCount - i) * sizeof(*_alarms));
		memmove(_alarms + i, &alarm, sizeof(*_alarms));
	} else {
		memcpy(&(_alarms[_alarmCount]), &alarm, sizeof(*_alarms));
	}

	_alarmCount++;

	return false;
}

/**
 * Insert an alarm to be triggered at a later time
 * @param	listener			the alarm listener to be called when the alarm is triggered
 * @param	whenSeconds			the number of seconds past current that the alarm will be executed
 * @param	whenMilliseconds	the number of milliseconds past current that the alarm will be executed
 * @param	whenTicks			the number of ticks past current that the alarm will be executed
 * @param	repeatSeconds		the number of seconds past current that the alarm will be executed
 * @param	repeatMilliseconds	the number of milliseconds past current that the alarm will be executed
 * @param	repeatTicks			the number of ticks past current that the alarm will be executed
 */
bool MHV_RTC::addAlarm(MHV_AlarmListener &listener,
		uint32_t whenSeconds, uint16_t whenMilliseconds, uint8_t whenTicks,
		uint32_t repeatSeconds, uint16_t repeatMilliseconds, uint8_t repeatTicks) {
	MHV_ALARM alarm;

	alarm.listener = &listener;
	current(alarm.when);
	timestampIncrement(alarm.when, whenSeconds, whenMilliseconds, whenTicks);
	alarm.repeat.timestamp = repeatSeconds;
	alarm.repeat.milliseconds = repeatMilliseconds;
	alarm.repeat.ticks = repeatTicks;

	addAlarm(alarm);
}

/**
 * Insert an alarm to be triggered at a later time
 * @param	listener			the alarm listener to be called when the alarm is triggered
 * @param	whenSeconds			the number of seconds past current that the alarm will be executed
 * @param	whenMilliseconds	the number of milliseconds past current that the alarm will be executed
 * @param	repeatSeconds		the number of seconds past current that the alarm will be executed
 * @param	repeatMilliseconds	the number of milliseconds past current that the alarm will be executed
 */
bool MHV_RTC::addAlarm(MHV_AlarmListener &listener,
		uint32_t whenSeconds, uint16_t whenMilliseconds,
		uint32_t repeatSeconds, uint16_t repeatMilliseconds) {
	MHV_ALARM alarm;

	alarm.listener = &listener;
	current(alarm.when);
	timestampIncrement(alarm.when, whenSeconds, whenMilliseconds, 0);
	alarm.repeat.timestamp = repeatSeconds;
	alarm.repeat.milliseconds = repeatMilliseconds;
	alarm.repeat.ticks = 0;

	addAlarm(alarm);
}

/**
 * Insert an alarm to be triggered at a later time
 * @param	listener			the alarm listener to be called when the alarm is triggered
 * @param	whenSeconds			the number of seconds past current that the alarm will be executed
 * @param	whenMilliseconds	the number of milliseconds past current that the alarm will be executed
 * @param	whenTicks			the number of ticks past current that the alarm will be executed
 */
bool MHV_RTC::addAlarm(MHV_AlarmListener &listener,
		uint32_t whenSeconds, uint16_t whenMilliseconds, uint8_t whenTicks) {
	MHV_ALARM alarm;

	alarm.listener = &listener;
	current(alarm.when);
	timestampIncrement(alarm.when, whenSeconds, whenMilliseconds, whenTicks);
	alarm.repeat.timestamp = 0;
	alarm.repeat.milliseconds = 0;
	alarm.repeat.ticks = 0;

	addAlarm(alarm);
}

/**
 * Insert an alarm to be triggered at a later time
 * @param	listener			the alarm listener to be called when the alarm is triggered
 * @param	whenSeconds			the number of seconds past current that the alarm will be executed
 * @param	whenMilliseconds	the number of milliseconds past current that the alarm will be executed
 */
bool MHV_RTC::addAlarm(MHV_AlarmListener &listener,
		uint32_t whenSeconds, uint16_t whenMilliseconds) {
	MHV_ALARM alarm;

	alarm.listener = &listener;
	current(alarm.when);
	timestampIncrement(alarm.when, whenSeconds, whenMilliseconds, 0);
	alarm.repeat.timestamp = 0;
	alarm.repeat.milliseconds = 0;
	alarm.repeat.ticks = 0;

	addAlarm(alarm);
}

/**
 * Insert an alarm to be triggered at a later time
 * @param	listener			the alarm listener to be called when the alarm is triggered
 * @param	whenSeconds			the number of seconds past current that the alarm will be executed
 * @param	whenMilliseconds	the number of milliseconds past current that the alarm will be executed
 * @param	whenTicks			the number of ticks past current that the alarm will be executed
 * @param	repeatSeconds		the number of seconds past current that the alarm will be executed
 * @param	repeatMilliseconds	the number of milliseconds past current that the alarm will be executed
 * @param	repeatTicks			the number of ticks past current that the alarm will be executed
 */
bool MHV_RTC::addAlarm(MHV_AlarmListener *listener,
		uint32_t whenSeconds, uint16_t whenMilliseconds, uint8_t whenTicks,
		uint32_t repeatSeconds, uint16_t repeatMilliseconds, uint8_t repeatTicks) {
	MHV_ALARM alarm;

	alarm.listener = listener;
	current(alarm.when);
	timestampIncrement(alarm.when, whenSeconds, whenMilliseconds, whenTicks);
	alarm.repeat.timestamp = repeatSeconds;
	alarm.repeat.milliseconds = repeatMilliseconds;
	alarm.repeat.ticks = repeatTicks;

	addAlarm(alarm);
}

/**
 * Insert an alarm to be triggered at a later time
 * @param	listener			the alarm listener to be called when the alarm is triggered
 * @param	whenSeconds			the number of seconds past current that the alarm will be executed
 * @param	whenMilliseconds	the number of milliseconds past current that the alarm will be executed
 * @param	repeatSeconds		the number of seconds past current that the alarm will be executed
 * @param	repeatMilliseconds	the number of milliseconds past current that the alarm will be executed
 */
bool MHV_RTC::addAlarm(MHV_AlarmListener *listener,
		uint32_t whenSeconds, uint16_t whenMilliseconds,
		uint32_t repeatSeconds, uint16_t repeatMilliseconds) {
	MHV_ALARM alarm;

	alarm.listener = listener;
	current(alarm.when);
	timestampIncrement(alarm.when, whenSeconds, whenMilliseconds, 0);
	alarm.repeat.timestamp = repeatSeconds;
	alarm.repeat.milliseconds = repeatMilliseconds;
	alarm.repeat.ticks = 0;

	addAlarm(alarm);
}

/**
 * Insert an alarm to be triggered at a later time
 * @param	listener			the alarm listener to be called when the alarm is triggered
 * @param	whenSeconds			the number of seconds past current that the alarm will be executed
 * @param	whenMilliseconds	the number of milliseconds past current that the alarm will be executed
 * @param	whenTicks			the number of ticks past current that the alarm will be executed
 */
bool MHV_RTC::addAlarm(MHV_AlarmListener *listener,
		uint32_t whenSeconds, uint16_t whenMilliseconds, uint8_t whenTicks) {
	MHV_ALARM alarm;

	alarm.listener = listener;
	current(alarm.when);
	timestampIncrement(alarm.when, whenSeconds, whenMilliseconds, whenTicks);
	alarm.repeat.timestamp = 0;
	alarm.repeat.milliseconds = 0;
	alarm.repeat.ticks = 0;

	addAlarm(alarm);
}

/**
 * Insert an alarm to be triggered at a later time
 * @param	listener			the alarm listener to be called when the alarm is triggered
 * @param	whenSeconds			the number of seconds past current that the alarm will be executed
 * @param	whenMilliseconds	the number of milliseconds past current that the alarm will be executed
 */
bool MHV_RTC::addAlarm(MHV_AlarmListener *listener,
		uint32_t whenSeconds, uint16_t whenMilliseconds) {
	MHV_ALARM alarm;

	alarm.listener = listener;
	current(alarm.when);
	timestampIncrement(alarm.when, whenSeconds, whenMilliseconds, 0);
	alarm.repeat.timestamp = 0;
	alarm.repeat.milliseconds = 0;
	alarm.repeat.ticks = 0;

	addAlarm(alarm);
}


/**
 * Check for events that are due, and run them
 * Run this from your main loop if you have no blocking calls, otherwise, call
 * tickAndRunEvents instead of tick from the timer (note that this will run your
 * events in the interrupt handler, so keep them short!)
 */
void MHV_RTC::handleEvents() {
	uint8_t i;
	MHV_TIMESTAMP timestamp;
	current(timestamp);

	// Run any events pending
	for (i = 0; i < _alarmCount && mhv_timestampGreaterThanOrEqual(timestamp, _alarms[i].when);
			i++, current(timestamp)) {
		_alarms[i].listener->alarm(_alarms[i]);

		// Repeat the event if necessary
		if (0 != _alarms[i].repeat.ticks || 0 != _alarms[i].repeat.milliseconds || 0 != _alarms[i].repeat.timestamp) {
			current(_alarms[i].when);
			timestampIncrement(_alarms[i].when, _alarms[i].repeat);
			addAlarm(_alarms[i]);
		}
	}

	if (0 == i) {
		return;
	}

	// Shift remaining events down
	if (i < _alarmCount) {
		mhv_memcpyTailFirst(_alarms,  &(_alarms[i]), MHV_BYTESIZEOF(*_alarms), (uint8_t)(_alarmCount - i));
	}

	_alarmCount -= i;
}

/**
 * How many events are left in the queue
 * @return the number of events
 */
uint8_t MHV_RTC::alarmsPending() {
	return _alarmCount;
}

/**
 * Remove all matching events from the list of pending events
 * @param	listener		the listener for the event to remove
 */
void MHV_RTC::removeAlarm(MHV_AlarmListener &listener) {
	for (uint8_t i = 0; i < _alarmCount; i++) {
		if (_alarms[i].listener == &listener) {
			// Shift remaining events down
			if (i < _alarmCount) {
				mhv_memcpyTailFirst(_alarms,  &(_alarms[i]), MHV_BYTESIZEOF(*_alarms), (uint8_t)(_alarmCount - i));
			}

			_alarmCount -= i;
		}
	}
}

/**
 * Remove all matching events from the list of pending events
 * @param	listener		the listener for the event to remove
 */
void MHV_RTC::removeAlarm(MHV_AlarmListener *listener) {
	for (uint8_t i = 0; i < _alarmCount; i++) {
		if (_alarms[i].listener == listener) {
			// Shift remaining events down
			if (i < _alarmCount) {
				memmove(_alarms, &(_alarms[i]), (_alarmCount - i) * sizeof(*_alarms));
			}

			_alarmCount -= i;
		}
	}
}
