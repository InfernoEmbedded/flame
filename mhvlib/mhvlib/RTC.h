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
#ifndef MHV_RTC_H_
#define MHV_RTC_H_

#include <mhvlib/Timer.h>
#include <avr/pgmspace.h>
#include <string.h> // Needed for memmove
#include <util/atomic.h>
#include <mhvlib/Device_TX.h>

namespace mhvlib {

struct timestamp {
	uint32_t	timestamp;
	uint16_t	milliseconds;
	uint8_t		ticks;
};
typedef struct timestamp TIMESTAMP;

enum class Weekday : uint8_t {
	SUNDAY = 0,
	MONDAY = 1,
	TUESDAY = 2,
	WEDNESDAY = 3,
	THURSDAY = 4,
	FRIDAY = 5,
	SATURDAY = 6
};

enum class Month : uint8_t {
	JANUARY = 1,
	FEBRUARY = 2,
	MARCH = 3,
	APRIL = 4,
	MAY = 5,
	JUNE = 6,
	MHV_JULY = 7,
	MHV_AUGUST = 8,
	MHV_SEPTEMBER = 9,
	MHV_OCTOBER = 10,
	MHV_NOVEMBER = 11,
	MHV_DECEMBER = 12
};

INLINE uint8_t operator- (Month month, uint8_t subtractValue) {
	const uint8_t myMonth = static_cast<uint8_t>(month);
	return(myMonth - subtractValue);
}


struct time {
	uint16_t	milliseconds;	// 0 - 999
	uint8_t		seconds;		// 0 - 59 (we don't do leap seconds)
	uint8_t		minutes;		// 0 - 59
	uint8_t		hours;			// 0 - 23
	uint8_t		day;			// 1 - 31	day of the month
	Month	month;			// 1 - 12
	uint16_t	year;			// 1970 - 2106
//	MHV_WEEKDAY	weekday;		// Weekday
	uint16_t	yearday;		// 0 - 365 Day of the year
	uint16_t	timezone;		// Timezone offset in minutes
};
typedef struct time TIME;

struct alarm {
	TIMESTAMP		when;
	TIMESTAMP		repeat;
	TimerListener	*listener;
};
typedef struct alarm ALARM;

/**
 * Initialise an alarm struct with tick resolution and repeats
 *
 * @param alarm					the alarm to initialise
 * @param listener				the listener to execute when the alarm fires
 * @param whenTimestamp			the timestamp the alarm should fire at
 * @param whenMilliseconds		the milliseconds past the timestamp the alarm should fire at
 * @param whenTicks				the ticks past the millisecond the alarm should fire at
 * @param repeatTimestamp		the number of seconds to repeat the alarm
 * @param repeatMilliseconds	the number of milliseconds to repeat the alarm
 * @param repeatTicks			the number of ticks to repeat the alarm
 */
inline void alarmInit(ALARM &alarm, TimerListener &listener,
		uint32_t whenTimestamp, uint16_t whenMilliseconds, uint8_t whenTicks,
		uint32_t repeatTimestamp, uint16_t repeatMilliseconds, uint8_t repeatTicks) {
	alarm.listener = &listener;
	alarm.when.timestamp = whenTimestamp;
	alarm.when.milliseconds = whenMilliseconds;
	alarm.when.ticks = whenTicks;
	alarm.repeat.timestamp = repeatTimestamp;
	alarm.repeat.milliseconds = repeatMilliseconds;
	alarm.repeat.ticks = repeatTicks;
}

/**
 * Initialise an alarm struct with millisecond resolution and repeats
 *
 * @param alarm					the alarm to initialise
 * @param listener				the listener to execute when the alarm fires
 * @param whenTimestamp			the timestamp the alarm should fire at
 * @param whenMilliseconds		the milliseconds past the timestamp the alarm should fire at
 * @param repeatTimestamp		the number of seconds to repeat the alarm
 * @param repeatMilliseconds	the number of milliseconds to repeat the alarm
 */
inline void alarmInit(ALARM &alarm, TimerListener &listener,
		uint32_t whenTimestamp, uint16_t whenMilliseconds,
		uint32_t repeatTimestamp, uint16_t repeatMilliseconds) {
	alarm.listener = &listener;
	alarm.when.timestamp = whenTimestamp;
	alarm.when.milliseconds = whenMilliseconds;
	alarm.when.ticks = 0;
	alarm.repeat.timestamp = repeatTimestamp;
	alarm.repeat.milliseconds = repeatMilliseconds;
	alarm.repeat.ticks = 0;
}

/**
 * Initialise an alarm struct with tick resolution
 *
 * @param alarm					the alarm to initialise
 * @param listener				the listener to execute when the alarm fires
 * @param whenTimestamp			the timestamp the alarm should fire at
 * @param whenMilliseconds		the milliseconds past the timestamp the alarm should fire at
 * @param whenTicks				the ticks past the millisecond the alarm should fire at
 */
inline void alarmInit(ALARM &alarm, TimerListener &listener,
		uint32_t whenTimestamp, uint16_t whenMilliseconds, uint8_t whenTicks) {
	alarm.listener = &listener;
	alarm.when.timestamp = whenTimestamp;
	alarm.when.milliseconds = whenMilliseconds;
	alarm.when.ticks = whenTicks;
	alarm.repeat.timestamp = 0;
	alarm.repeat.milliseconds = 0;
	alarm.repeat.ticks = 0;
}

/**
 * Initialise an alarm struct with millisecond resolution
 *
 * @param alarm					the alarm to initialise
 * @param listener				the listener to execute when the alarm fires
 * @param whenTimestamp			the timestamp the alarm should fire at
 * @param whenMilliseconds		the milliseconds past the timestamp the alarm should fire at
 */
inline void alarmInit(ALARM &alarm, TimerListener &listener,
		uint32_t whenTimestamp, uint16_t whenMilliseconds) {
	alarm.listener = &listener;
	alarm.when.timestamp = whenTimestamp;
	alarm.when.milliseconds = whenMilliseconds;
	alarm.when.ticks = 0;
	alarm.repeat.timestamp = 0;
	alarm.repeat.milliseconds = 0;
	alarm.repeat.ticks = 0;
}

void timestampIncrement(TIMESTAMP &timestamp, uint32_t seconds, uint16_t milliseconds);
bool isLeapYear(uint16_t year);
uint16_t getMilliseconds(const TIMESTAMP &timestamp);
bool timestampGreaterThanOrEqual(const TIMESTAMP &first, const TIMESTAMP &second);
bool timestampLessThan(const TIMESTAMP &first, const TIMESTAMP &second);
uint8_t daysInMonth(Month month, uint16_t year);


// Used in toTime: Cumulative totals at the end of the month in a normal year
#if 0 && defined __FLASH
#define SECONDS_FROM_YEAR_START(month) secondsFromYearStart[month]
const uint32_t __flash secondsFromYearStart[] = {
#else
#define SECONDS_FROM_YEAR_START(month) pgm_read_dword(&secondsFromYearStart[month])
const uint32_t secondsFromYearStart[] PROGMEM = {
#endif
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
 * Create a new RTC
 * @param	_mhvObjectName	the name of the RTC to create
 * @param	_mhvEventCount	the number of events
 * @param	_mhvTimezone	the timezone
 */
#define MHV_RTC_CREATE_TZ(_mhvObjectName,_mhvEventCount,_mhvTimezone) \
		MHV_RTCTemplate<_mhvEventCount> _mhvObjectName(_mhvTimezone);

/**
 * Create a new RTC
 * @param	_mhvObjectName	the name of the RTC to create
 * @param	_mhvEventCount	the number of events
 */
#define MHV_RTC_CREATE(_mhvObjectName,_mhvEventCount) \
		RTCImplementation<_mhvEventCount> _mhvObjectName;


class RTC : public TimerListener {
protected:
	volatile uint8_t		_alarmCount;
	uint32_t				_timestamp;
	uint16_t				_milliseconds;
	uint8_t					_ticks;
	uint8_t					_ticksPerMillisecond;
	int16_t					_tzOffset; // Timezone offset in minutes +/- GMT

	/**
	 * Increment milliseconds by 1
	 */
	inline void incrementMilliseconds() {
		_milliseconds++;

		if (_milliseconds > 999) {
			_milliseconds = 0;
			_timestamp++;
		}
	}


public:
	/**
	 * Create a new RTC
	 * @param	timezone	minutes offset from UTC
	 */
	RTC(int16_t timezone = 0) :
			_alarmCount(0),
			_milliseconds(0),
			_ticks(0),
			_ticksPerMillisecond(1),
			_tzOffset(timezone) {}

	/**
	 *  Synchronise the ticksPerMillisecond with the timer
	 *  This is useful if you change the timer values, or if you are are running the timer faster than 1ms
	 *  @param	timer	the timer to sync with
	 */
	void synchronise(Timer &timer) {
		uint32_t ticksPerMillisecond = F_CPU / timer.getPrescalerMultiplier() / (timer.getTop() + 1) / 1000;
		_ticksPerMillisecond = ticksPerMillisecond;
	}

	/**
	 * Set the current time
	 * @param timestamp		the current Unix timestamp
	 * @param milliseconds	the current milliseconds offset
	 */
	void setTime(uint32_t timestamp, uint16_t milliseconds) {
		do {
			_milliseconds = milliseconds;
			_timestamp = timestamp;
		} while (milliseconds != _milliseconds); // Retry if the value changed while updating
	}

	/**
	 * Set the current time
	 * @param timestamp		the current Unix timestamp
	 */
	void setTime(TIMESTAMP &timestamp) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			_ticks = timestamp.ticks;
			_milliseconds = timestamp.milliseconds;
			_timestamp = timestamp.timestamp;
		}
	}


	/**
	 * Alarm handler from the timer
	 */
	void alarm(UNUSED AlarmSource source) {
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
	void current(TIMESTAMP &timestamp) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
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
	void elapsed(const TIMESTAMP &since, TIMESTAMP &elapsed) {
		TIMESTAMP currentTimestamp;

		current(currentTimestamp);

		minusTimeStamp(currentTimestamp,since,elapsed);
	}

	/**
	 * Substracts b from a
	 * Fixme: Does not deal with ticks
	 * Fixme: should be an operator
	 *
	 * @param	a		timestamp to subtract from
	 * @param	b		timestamp to substract
	 * @param	delta		the result
	 */
	void minusTimeStamp(const TIMESTAMP &a, const TIMESTAMP &b, TIMESTAMP &delta) {
		delta.timestamp = a.timestamp - b.timestamp;

		if (a.milliseconds > b.milliseconds) {
			delta.milliseconds = a.milliseconds - b.milliseconds;
		} else if (delta.timestamp) {
			delta.timestamp--;
			delta.milliseconds = 1000 + a.milliseconds - b.milliseconds;
		} else {
			delta.milliseconds = 0;
		}
	}

	/**
	 * Convert a timestamp into year,month,day,hours,minutes,seconds
	 * @param to	the MHV_TIME to store the results
	 * @param from	the MHV_TIMESTAMP struct to convert from
	 */
	void toTime(TIME &to, const TIMESTAMP &from) {
		uint16_t year = 1970;
		bool leapYear = false;
		uint32_t seconds = from.timestamp + _tzOffset;

		to.timezone = _tzOffset;

		uint32_t secondsThisYear = 365 * 86400;
		uint32_t secondsSoFar = 0;
		while (seconds > secondsThisYear) {
			seconds -= secondsThisYear;
			secondsSoFar += secondsThisYear;

			if ((isLeapYear(++year))) {
				secondsThisYear = 366 * 86400;
			} else {
				secondsThisYear = 365 * 86400;
			}
		}
		to.year = year;

		uint8_t month;
		leapYear = isLeapYear(year);
		for (month = 0; month < sizeof(secondsFromYearStart); month++) {
			if (leapYear) {
				if (seconds < SECONDS_FROM_YEAR_START(month) + (1 == month) ? 86400 : 0) {
					break;
				}
			} else {
				if (seconds < SECONDS_FROM_YEAR_START(month)) {
					break;
				}
			}
		}
		to.yearday = seconds / 86400;

		if (month) {
			seconds -= SECONDS_FROM_YEAR_START(month - 1);
		}

		if (leapYear && month > 1) {
			seconds -= 86400;
		}

		// Increment so we start numbering from 1
		to.month = (Month)(month + 1);

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
	void toTimestamp(TIMESTAMP &to, const TIME &from) {
		to.milliseconds = from.milliseconds;

		uint32_t seconds = from.seconds;

		seconds += (uint32_t)from.minutes * 60;

		seconds += (uint32_t)from.hours * 3600;

		if (from.yearday) {
			seconds += from.yearday * 86400;
		} else {
			seconds += (from.day - 1) * 86400;
			switch (from.month) {
			case Month::JANUARY:
				break;
			case Month::FEBRUARY:
				seconds += 5097600;
				break;
			default:
				seconds += SECONDS_FROM_YEAR_START(from.month - 2);
				if (isLeapYear(from.year)) {
					seconds += 86400;
				}
				break;
			}
		}

		for (uint16_t year = from.year - 1; year >= 1970; year--) {
			seconds += 365 * 86400;
			if (isLeapYear(year)) {
				seconds += 86400;
			}
		}

		to.timestamp = seconds - _tzOffset;
		to.ticks = 0;
	}

	/**
	 * Increment a timestamp
	 * @param timestamp		the timestamp to increment
	 * @param timestamp2	the timestamp to increment by
	 */
	void timestampIncrement(TIMESTAMP &timestamp, const TIMESTAMP &timestamp2) {
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
	void timestampIncrement(TIMESTAMP &timestamp, uint32_t seconds, uint16_t milliseconds, uint8_t ticks) {
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

	/**
	 * Insert an alarm to be triggered at a later time
	 * @param	listener			the alarm listener to be called when the alarm is triggered
	 * @param	whenSeconds			the number of seconds past current that the alarm will be executed
	 * @param	whenMilliseconds	the number of milliseconds past current that the alarm will be executed
	 * @param	whenTicks			the number of ticks past current that the alarm will be executed
	 * @param	repeatSeconds		the number of seconds past current that the alarm will be executed
	 * @param	repeatMilliseconds	the number of milliseconds past current that the alarm will be executed
	 * @param	repeatTicks			the number of ticks past current that the alarm will be executed
	 * @return	true if the event could not be added
	 */
	bool addAlarm(TimerListener &listener,
			uint32_t whenSeconds, uint16_t whenMilliseconds, uint8_t whenTicks,
			uint32_t repeatSeconds, uint16_t repeatMilliseconds, uint8_t repeatTicks) {
		ALARM alarm;

		alarm.listener = &listener;
		current(alarm.when);
		timestampIncrement(alarm.when, whenSeconds, whenMilliseconds, whenTicks);
		alarm.repeat.timestamp = repeatSeconds;
		alarm.repeat.milliseconds = repeatMilliseconds;
		alarm.repeat.ticks = repeatTicks;

		return registerAlarm(alarm);
	}

	/**
	 * Insert an alarm to be triggered at a later time
	 * @param	listener			the alarm listener to be called when the alarm is triggered
	 * @param	whenSeconds			the number of seconds past current that the alarm will be executed
	 * @param	whenMilliseconds	the number of milliseconds past current that the alarm will be executed
	 * @param	repeatSeconds		the number of seconds past current that the alarm will be executed
	 * @param	repeatMilliseconds	the number of milliseconds past current that the alarm will be executed
	 * @return	true if the event could not be added
	 */
	bool addAlarm(TimerListener &listener,
			uint32_t whenSeconds, uint16_t whenMilliseconds,
			uint32_t repeatSeconds, uint16_t repeatMilliseconds) {
		ALARM alarm;

		alarm.listener = &listener;
		current(alarm.when);
		timestampIncrement(alarm.when, whenSeconds, whenMilliseconds, 0);
		alarm.repeat.timestamp = repeatSeconds;
		alarm.repeat.milliseconds = repeatMilliseconds;
		alarm.repeat.ticks = 0;

		return registerAlarm(alarm);
	}

	/**
	 * Insert an alarm to be triggered at a later time
	 * @param	listener			the alarm listener to be called when the alarm is triggered
	 * @param	whenSeconds			the number of seconds past current that the alarm will be executed
	 * @param	whenMilliseconds	the number of milliseconds past current that the alarm will be executed
	 * @param	whenTicks			the number of ticks past current that the alarm will be executed
	 * @return	true if the event could not be added
	 */
	bool addAlarm(TimerListener &listener,
			uint32_t whenSeconds, uint16_t whenMilliseconds, uint8_t whenTicks) {
		ALARM alarm;

		alarm.listener = &listener;
		current(alarm.when);
		timestampIncrement(alarm.when, whenSeconds, whenMilliseconds, whenTicks);
		alarm.repeat.timestamp = 0;
		alarm.repeat.milliseconds = 0;
		alarm.repeat.ticks = 0;

		return registerAlarm(alarm);
	}

	/**
	 * Insert an alarm to be triggered at a later time
	 * @param	listener			the alarm listener to be called when the alarm is triggered
	 * @param	whenSeconds			the number of seconds past current that the alarm will be executed
	 * @param	whenMilliseconds	the number of milliseconds past current that the alarm will be executed
	 * @return	true if the event could not be added
	 */
	bool addAlarm(TimerListener &listener,
			uint32_t whenSeconds, uint16_t whenMilliseconds) {
		ALARM alarm;

		alarm.listener = &listener;
		current(alarm.when);
		timestampIncrement(alarm.when, whenSeconds, whenMilliseconds, 0);
		alarm.repeat.timestamp = 0;
		alarm.repeat.milliseconds = 0;
		alarm.repeat.ticks = 0;

		return registerAlarm(alarm);
	}

	/**
	 * Insert an alarm to be triggered at a later time
	 * @param	listener			the alarm listener to be called when the alarm is triggered
	 * @param	when				the time the alarm will be executed
	 * @return	true if the event could not be added
	 */
	bool addAlarm(TimerListener &listener, TIMESTAMP &when) {
		ALARM alarm;

		alarm.listener = &listener;

		memcpy(&alarm.when, &when, MHV_BYTESIZEOF(when));
		alarm.repeat.timestamp = 0;
		alarm.repeat.milliseconds = 0;
		alarm.repeat.ticks = 0;

		return registerAlarm(alarm);
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
	 * @return	true if the event could not be added
	 */
	bool addAlarm(TimerListener *listener,
			uint32_t whenSeconds, uint16_t whenMilliseconds, uint8_t whenTicks,
			uint32_t repeatSeconds, uint16_t repeatMilliseconds, uint8_t repeatTicks) {
		ALARM alarm;

		alarm.listener = listener;
		current(alarm.when);
		timestampIncrement(alarm.when, whenSeconds, whenMilliseconds, whenTicks);
		alarm.repeat.timestamp = repeatSeconds;
		alarm.repeat.milliseconds = repeatMilliseconds;
		alarm.repeat.ticks = repeatTicks;

		return registerAlarm(alarm);
	}

	/**
	 * Insert an alarm to be triggered at a later time
	 * @param	listener			the alarm listener to be called when the alarm is triggered
	 * @param	whenSeconds			the number of seconds past current that the alarm will be executed
	 * @param	whenMilliseconds	the number of milliseconds past current that the alarm will be executed
	 * @param	repeatSeconds		the number of seconds past current that the alarm will be executed
	 * @param	repeatMilliseconds	the number of milliseconds past current that the alarm will be executed
	 * @return	true if the event could not be added
	 */
	bool addAlarm(TimerListener *listener,
			uint32_t whenSeconds, uint16_t whenMilliseconds,
			uint32_t repeatSeconds, uint16_t repeatMilliseconds) {
		ALARM alarm;

		alarm.listener = listener;
		current(alarm.when);
		timestampIncrement(alarm.when, whenSeconds, whenMilliseconds, 0);
		alarm.repeat.timestamp = repeatSeconds;
		alarm.repeat.milliseconds = repeatMilliseconds;
		alarm.repeat.ticks = 0;

		return registerAlarm(alarm);
	}

	/**
	 * Insert an alarm to be triggered at a later time
	 * @param	listener			the alarm listener to be called when the alarm is triggered
	 * @param	whenSeconds			the number of seconds past current that the alarm will be executed
	 * @param	whenMilliseconds	the number of milliseconds past current that the alarm will be executed
	 * @param	whenTicks			the number of ticks past current that the alarm will be executed
	 * @return	true if the event could not be added
	 */
	bool addAlarm(TimerListener *listener,
			uint32_t whenSeconds, uint16_t whenMilliseconds, uint8_t whenTicks) {
		ALARM alarm;

		alarm.listener = listener;
		current(alarm.when);
		timestampIncrement(alarm.when, whenSeconds, whenMilliseconds, whenTicks);
		alarm.repeat.timestamp = 0;
		alarm.repeat.milliseconds = 0;
		alarm.repeat.ticks = 0;

		return registerAlarm(alarm);
	}

	/**
	 * Insert an alarm to be triggered at a later time
	 * @param	listener			the alarm listener to be called when the alarm is triggered
	 * @param	whenSeconds			the number of seconds past current that the alarm will be executed
	 * @param	whenMilliseconds	the number of milliseconds past current that the alarm will be executed
	 * @return	true if the event could not be added
	 */
	bool addAlarm(TimerListener *listener,
			uint32_t whenSeconds, uint16_t whenMilliseconds) {
		ALARM alarm;

		alarm.listener = listener;
		current(alarm.when);
		timestampIncrement(alarm.when, whenSeconds, whenMilliseconds, 0);
		alarm.repeat.timestamp = 0;
		alarm.repeat.milliseconds = 0;
		alarm.repeat.ticks = 0;

		return registerAlarm(alarm);
	}

	/**
	 * Insert an alarm, to be triggered at a later time
	 * @param	alarm	the alarm, consisting of:
	 * 						when it should be triggered
	 * 						how often it should be repeated
	 * 						what should be called (it will be passed a reference to the alarm)
	 * @return	true if the event could not be added
	 */
	bool addAlarm(ALARM &alarm) {
		return registerAlarm(alarm);
	}

	/**
	 * Insert an alarm, to be triggered at a later time
	 * @param	alarm	the alarm, consisting of:
	 * 						when it should be triggered
	 * 						how often it should be repeated
	 * 						what should be called (it will be passed a reference to the alarm)
	 * @return	true if the event could not be added
	 */
	virtual bool registerAlarm(ALARM &alarm) =0;

	/**
	 * Check for events that are due, and run them
	 * Run this from your main loop if you have no blocking calls, otherwise, call
	 * tickAndRunEvents instead of tick from the timer (note that this will run your
	 * events in the interrupt handler, so keep them short!)
	 */
	virtual void handleEvents() =0;

	/**
	 * How many events are left in the queue
	 * @return the number of events
	 */
	uint8_t alarmsPending() {
		return _alarmCount;
	}

	/**
	 * Remove all matching events from the list of pending events
	 * @param	listener		the listener for the event to remove
	 */
	virtual void removeAlarm(TimerListener &listener) =0;

	/**
	 * Remove all matching events from the list of pending events
	 * @param	listener		the listener for the event to remove
	 */
	virtual void removeAlarm(TimerListener *listener) =0;
};


template<uint8_t alarmMax = 8>
class RTCImplementation : public RTC {
protected:
	ALARM				_alarms[alarmMax];

public:
	/**
	 * Create a new RTC
	 * @param	timezone	minutes offset from UTC
	 */
	RTCImplementation(int16_t timezone = 0) : RTC(timezone) {
		memset (_alarms, sizeof(_alarms[0]), alarmMax);
	}

	/**
	 * Remove all matching events from the list of pending events
	 * @param	listener		the listener for the event to remove
	 */
	void removeAlarm(TimerListener &listener) {
		removeAlarm(&listener);
	}

	/**
	 * Remove all matching events from the list of pending events
	 * @param	listener		the listener for the event to remove
	 */
	void removeAlarm(TimerListener *listener) {
		for (uint8_t i = 0; i < _alarmCount; i++) {
			if (_alarms[i].listener == listener) {
				// Shift remaining events down
				if (i < _alarmCount) {
					memcpy(_alarms + i,  _alarms + i + 1, MHV_BYTESIZEOF(*_alarms) * (uint8_t)(_alarmCount - i));
				}

				_alarmCount--;
			}
		}
	}

	/**
	 * Check for events that are due, and run them
	 * Run this from your main loop if you have no blocking calls, otherwise, call
	 * tickAndRunEvents instead of tick from the timer (note that this will run your
	 * events in the interrupt handler, so keep them short!)
	 */
	void handleEvents_old() {
		uint8_t i;
		TIMESTAMP timestamp;
		current(timestamp);

		// Run any events pending
		for (i = 0; i < _alarmCount && timestampGreaterThanOrEqual(timestamp, _alarms[i].when);
				i++, current(timestamp)) {
//			tx.printf(PSTR("Handing alarm at %d, current = %ld.%d, scheduled = %ld.%d\r\n"),
//					i, timestamp.timestamp, timestamp.milliseconds,
//					_alarms[i].when.timestamp, _alarms[i].when.milliseconds);

			_alarms[i].listener->alarm(AlarmSource::RTC);

			// Repeat the event if necessary
			if (0 != _alarms[i].repeat.ticks || 0 != _alarms[i].repeat.milliseconds || 0 != _alarms[i].repeat.timestamp) {
				timestampIncrement(_alarms[i].when, _alarms[i].repeat);
				registerAlarm(_alarms[i]);
			}
		}

		// i is now the number of handled events
		if (0 == i) {
			return;
		}

		// Shift remaining events down
		if (i < _alarmCount) {
			memcpy(_alarms,  &(_alarms[i]), sizeof(*_alarms) * (uint8_t)(_alarmCount - i));
		}

		_alarmCount -= i;
	}


	void handleEvents() {
		uint8_t i = 0; // this is the number of alarms we know are in the future
		TIMESTAMP timestamp;
		ALARM tmp;

		current(timestamp);

		while (i<_alarmCount) {
			if (timestampGreaterThanOrEqual(timestamp, _alarms[i].when)) {
				// take a copy:
				memcpy(&tmp,&_alarms[i],MHV_BYTESIZEOF(*_alarms));
				// splice out original:
				memmove(&_alarms[i],&_alarms[i+1],(_alarmCount-i-1)*sizeof(ALARM));
				_alarmCount--; // take one down, pass it around....
				tmp.listener->alarm(); // oh yeah, better do this...
				if (tmp.repeat.ticks || tmp.repeat.milliseconds || tmp.repeat.timestamp) {
					// repeat - reinsert it (somewhere!)
					timestampIncrement(tmp.when, tmp.repeat);
					registerAlarm(tmp);
				}
			} else {
				i++;
			}
		}
	}

	/**
	 * Insert an alarm, to be triggered at a later time
	 * @param	alarm	the alarm, consisting of:
	 * 						when it should be triggered
	 * 						how often it should be repeated
	 * 						what should be called (it will be passed a reference to the alarm)
	 * @return	true if the event could not be added
	 */
	bool registerAlarm(ALARM &alarm) {
		if (_alarmCount == alarmMax) {
			return true;
		}

		// Figure out where it needs to be inserted
		uint8_t i;
		for (i = 0; i < _alarmCount; ++i) {
			if (timestampLessThan(alarm.when, _alarms[i].when)) {
				break;
			}
		}
		// i now is the offset of where the timestamp should be inserted
		if (i < _alarmCount) {
			memmove(&_alarms[i + 1], &_alarms[i], MHV_BYTESIZEOF(*_alarms) * (uint8_t)(_alarmCount - i));
			memcpy(&_alarms[i], &alarm, MHV_BYTESIZEOF(*_alarms));
		} else {
			memcpy(&(_alarms[_alarmCount]), &alarm, MHV_BYTESIZEOF(*_alarms));
		}

		_alarmCount++;

		return false;
	}

	/**
	 * Dump the current alarms
	 * @param	tx	an output device to write to
	 */
	void dumpAlarms(Device_TX &tx) {
		tx.printf(PSTR("Dumping %d alarms\r\n"), _alarmCount);
		for (uint8_t i = 0; i < _alarmCount; i++) {
			tx.printf(PSTR("Alarm %d (%p)\t"), i, _alarms[i].listener);
			tx.printf(PSTR("Execute at:  %ld.%d.%d\tRepeat  at:  %ld.%d.%d\r\n"),
					_alarms[i].when.timestamp, _alarms[i].when.milliseconds, _alarms[i].when.ticks,
					_alarms[i].repeat.timestamp, _alarms[i].repeat.milliseconds, _alarms[i].repeat.ticks);
		}
		tx.write_P(PSTR("\r\n"));
	}
};

}
#endif /* MHV_RTC_H_ */
