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
#ifndef MHV_RTC_H_
#define MHV_RTC_H_

#include <MHV_Timer8.h>

struct mhv_timestamp {
	uint32_t	timestamp;
	uint16_t	milliseconds;
	uint8_t		ticks;
};
typedef struct mhv_timestamp MHV_TIMESTAMP;


enum mhv_weekday {
	MHV_SUNDAY,
	MHV_MONDAY,
	MHV_TUESDAY,
	MHV_WEDNESDAY,
	MHV_THURSDAY,
	MHV_FRIDAY,
	MHV_SATURDAY
};
typedef enum mhv_weekday MHV_WEEKDAY;

enum mhv_month {
	MHV_JANUARY = 1,
	MHV_FEBRUARY = 2,
	MHV_MARCH = 3,
	MHV_APRIL = 4,
	MHV_MAY = 5,
	MHV_JUNE = 6,
	MHV_JULY = 7,
	MHV_AUGUST = 8,
	MHV_SEPTEMBER = 9,
	MHV_OCTOBER = 10,
	MHV_NOVEMBER = 11,
	MHV_DECEMBER = 12
};
typedef enum mhv_month MHV_MONTH;


struct mhv_time {
	uint16_t	milliseconds;	// 0 - 999
	uint8_t		seconds;		// 0 - 59 (we don't do leap seconds)
	uint8_t		minutes;		// 0 - 59
	uint8_t		hours;			// 0 - 23
	uint8_t		day;			// 1 - 31	day of the month
	MHV_MONTH	month;			// 1 - 12
	uint16_t	year;			// 1970 - 2106
//	MHV_WEEKDAY	weekday;		// Weekday
	uint16_t	yearday;		// 0 - 365 Day of the year
	uint16_t	timezone;		// Timezone offset in minutes
};
typedef struct mhv_time MHV_TIME;

struct mhv_alarm {
	MHV_TIMESTAMP		when;
	MHV_TIMESTAMP		repeat;
	MHV_TimerListener	*listener;
};
typedef struct mhv_alarm MHV_ALARM;

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
inline void mhv_alarmInit(MHV_ALARM &alarm, MHV_TimerListener &listener,
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
inline void mhv_alarmInit(MHV_ALARM &alarm, MHV_TimerListener &listener,
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
inline void mhv_alarmInit(MHV_ALARM &alarm, MHV_TimerListener &listener,
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
inline void mhv_alarmInit(MHV_ALARM &alarm, MHV_TimerListener &listener,
		uint32_t whenTimestamp, uint16_t whenMilliseconds) {
	alarm.listener = &listener;
	alarm.when.timestamp = whenTimestamp;
	alarm.when.milliseconds = whenMilliseconds;
	alarm.when.ticks = 0;
	alarm.repeat.timestamp = 0;
	alarm.repeat.milliseconds = 0;
	alarm.repeat.ticks = 0;
}

void mhv_timestampIncrement(MHV_TIMESTAMP &timestamp, uint32_t seconds, uint16_t milliseconds);
bool mhv_isLeapYear(uint16_t year);
bool mhv_timestampGreaterThanOrEqual(const MHV_TIMESTAMP &first, const MHV_TIMESTAMP &second);
bool mhv_timestampLessThan(const MHV_TIMESTAMP &first, const MHV_TIMESTAMP &second);
uint8_t mhv_daysInMonth(MHV_MONTH month, uint16_t year);

/**
 * Create a new RTC
 * @param	_mhvObjectName	the name of the RTC to create
 * @param	_mhvEventCount	the number of events
 * @param	_mhvTimezone	the timezone
 */
#define MHV_RTC_CREATE_TZ(_mhvObjectName,_mhvEventCount,_mhvTimezone) \
		MHV_ALARM _mhvObjectName ## Alarms[_mhvEventCount]; \
		MHV_RTC _mhvObjectName(_mhvObjectName ## Alarms, _mhvEventCount,_mhvTimezone);

/**
 * Create a new RTC
 * @param	_mhvObjectName	the name of the RTC to create
 * @param	_mhvEventCount	the number of events
 */
#define MHV_RTC_CREATE(_mhvObjectName,_mhvEventCount) \
		MHV_ALARM _mhvObjectName ## Alarms[_mhvEventCount]; \
		MHV_RTC _mhvObjectName(_mhvObjectName ## Alarms, _mhvEventCount);

class MHV_RTC : public MHV_TimerListener {
private:
	inline void incrementMilliseconds();

protected:
	MHV_ALARM				*_alarms;
	volatile uint8_t		_alarmCount;
	uint8_t					_alarmMax;
	uint32_t				_timestamp;
	uint16_t				_milliseconds;
	uint8_t					_ticks;
	uint8_t					_ticksPerMillisecond;
	int16_t					_tzOffset; // Timezone offset in minutes +/- GMT


public:
	MHV_RTC(MHV_ALARM eventBuffer[], uint8_t eventCount, int16_t timezone = 0);
	void timestampIncrement(MHV_TIMESTAMP &timestamp, const MHV_TIMESTAMP &timestamp2);
	void timestampIncrement(MHV_TIMESTAMP &timestamp, uint32_t seconds, uint16_t milliseconds, uint8_t ticks);
	void synchronise(MHV_Timer8 &_timer);
	void setTime(uint32_t timestamp, uint16_t milliseconds);
	void setTime(MHV_TIMESTAMP &timestamp);
	void current(MHV_TIMESTAMP &timestamp);
	void elapsed(const MHV_TIMESTAMP &since, MHV_TIMESTAMP &elapsed);
	void toTime(MHV_TIME &to, const MHV_TIMESTAMP &from);
	void toTimestamp(MHV_TIMESTAMP &to, const MHV_TIME &from);
	bool addAlarm(MHV_ALARM &alarm);
	bool addAlarm(MHV_TimerListener &listener,
			uint32_t whenSeconds, uint16_t whenMilliseconds, uint8_t whenTicks,
			uint32_t repeatSeconds, uint16_t repeatMilliseconds, uint8_t repeatTicks);
	bool addAlarm(MHV_TimerListener &listener,
			uint32_t whenSeconds, uint16_t whenMilliseconds,
			uint32_t repeatSeconds, uint16_t repeatMilliseconds);
	bool addAlarm(MHV_TimerListener &listener,
			uint32_t whenSeconds, uint16_t whenMilliseconds, uint8_t whenTicks);
	bool addAlarm(MHV_TimerListener &listener,
			uint32_t whenSeconds, uint16_t whenMilliseconds);
	bool addAlarm(MHV_TimerListener *listener,
			uint32_t whenSeconds, uint16_t whenMilliseconds, uint8_t whenTicks,
			uint32_t repeatSeconds, uint16_t repeatMilliseconds, uint8_t repeatTicks);
	bool addAlarm(MHV_TimerListener *listener,
			uint32_t whenSeconds, uint16_t whenMilliseconds,
			uint32_t repeatSeconds, uint16_t repeatMilliseconds);
	bool addAlarm(MHV_TimerListener *listener,
			uint32_t whenSeconds, uint16_t whenMilliseconds, uint8_t whenTicks);
	bool addAlarm(MHV_TimerListener *listener,
			uint32_t whenSeconds, uint16_t whenMilliseconds);
	void handleEvents();
	uint8_t alarmsPending();
	void removeAlarm(MHV_TimerListener &listener);
	void removeAlarm(MHV_TimerListener *listener);
// from MHV_TimerListener
	void alarm();


};

#endif /* MHV_RTC_H_ */
