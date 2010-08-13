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
#ifndef MHV_RTC_H_
#define MHV_RTC_H_

#include <MHV_Timer8.h>

struct mhv_timestamp {
	uint32_t	timestamp;
	uint16_t	milliseconds;
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

typedef struct mhv_event MHV_EVENT;
struct mhv_event {
	MHV_TIMESTAMP	when;
	void (*actionFunction)(MHV_EVENT *event);
	void *actionData;
};

void mhv_timestampIncrement(MHV_TIMESTAMP *timestamp, uint32_t seconds, uint16_t milliseconds);
bool mhv_isLeapYear(uint16_t year);
bool mhv_timestampGreaterThanOrEqual(MHV_TIMESTAMP *first, MHV_TIMESTAMP *second);
bool mhv_timestampLessThan(MHV_TIMESTAMP *first, MHV_TIMESTAMP *second);
uint8_t mhv_daysInMonth(MHV_MONTH month, uint16_t year);

class MHV_RTC {
protected:
	MHV_Timer8				*_timer;
	MHV_EVENT				*_events;
	volatile uint8_t		_eventCount;
	uint8_t					_eventMax;
	uint32_t				_timestamp;
	uint16_t				_milliseconds;
	uint8_t					_ticks;
	uint8_t					_ticksPerMillisecond;
	int16_t					_tzOffset; // Timezone offset in minutes +/- GMT


public:
	MHV_RTC(MHV_Timer8 *timer, MHV_EVENT *eventBuffer, uint8_t eventCount, int16_t timezone);
	void synchronise();
	void setTime(uint32_t timestamp, uint16_t milliseconds);
	void setTime(MHV_TIMESTAMP *timestamp);
	void tick();
	void tick1ms();
	void tickAndRunEvents();
	void tick1msAndRunEvents();
	void current(MHV_TIMESTAMP *timestamp);
	void elapsed(MHV_TIMESTAMP *since, MHV_TIMESTAMP *elapsed);
	void toTime(MHV_TIME *to, MHV_TIMESTAMP *from);
	void toTimestamp(MHV_TIMESTAMP *to, MHV_TIME *from);
	bool addEvent(MHV_EVENT *event);
	void runEvents();
	uint8_t eventsPending();

};

#endif /* MHV_RTC_H_ */
