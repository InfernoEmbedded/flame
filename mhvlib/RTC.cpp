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

#include <mhvlib/RTC.h>

namespace mhvlib {

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
bool timestampLessThan(const TIMESTAMP &first, const TIMESTAMP &second) PURE;
bool timestampLessThan(const TIMESTAMP &first, const TIMESTAMP &second) {
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
 * Get a timestamp in milliseconds
 * @param	timestamp	the timestamp
 * @return the timestamp in milliseconds, clamped at 65535
 */
uint16_t getMilliseconds(const TIMESTAMP &timestamp) PURE;
uint16_t getMilliseconds(const TIMESTAMP &timestamp) {
	if (timestamp.timestamp > 65) {
		return 65535;
	}
	return timestamp.milliseconds + (uint16_t)timestamp.timestamp * 1000;
}

/**
 * Compare 2 timestamps
 * @param	first	the first timestamp
 * @param	second	the second timestamp
 * @return true if the first timestamp is greater than or equal to the second
 */
bool timestampGreaterThanOrEqual(const TIMESTAMP &first, const TIMESTAMP &second) PURE;
bool timestampGreaterThanOrEqual(const TIMESTAMP &first, const TIMESTAMP &second) {
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
bool isLeapYear(uint16_t year) CONST;
bool isLeapYear(uint16_t year) {
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
void timestampIncrement(TIMESTAMP &timestamp, uint32_t seconds, uint16_t milliseconds) {
	timestamp.milliseconds += milliseconds;

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
uint8_t CONST daysInMonth(MONTH month, uint16_t year) {
	if (MONTH::FEBRUARY == month && isLeapYear(year)) {
		return 29;
	}

	return pgm_read_byte(mhv_daysInMonthArray + (month - 1));
}

}
