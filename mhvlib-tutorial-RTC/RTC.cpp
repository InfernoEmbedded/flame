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

/* A clock to demonstrate the RTC class
 */

#include <MHV_io.h>
#include <MHV_HardwareSerial.h>
#include <MHV_Timer8.h>
#include <MHV_RTC.h>
#include <stdlib.h> // required for atoi, itoa

// A buffer for the serial port to receive data
#define RX_BUFFER_SIZE	81
char rxBuf[RX_BUFFER_SIZE];
MHV_RingBuffer rxBuffer(rxBuf, RX_BUFFER_SIZE);

// The number of elements we want to be able to store to send asynchronously
#define TX_ELEMENTS_COUNT 10
#define TX_BUFFER_SIZE TX_ELEMENTS_COUNT * sizeof(MHV_SERIAL_BUFFER) + 1
// A buffer for the serial port to send data, it only contains pointers
char txBuf[TX_BUFFER_SIZE];
MHV_RingBuffer txBuffer(txBuf, TX_BUFFER_SIZE);

MHV_HardwareSerial serial(&rxBuffer, &txBuffer, MHV_USART0, 115200);
MHV_HARDWARESERIAL_ASSIGN_INTERRUPTS(serial, MHV_USART0_INTERRUPTS);

// A timer we will use to tick the RTC
MHV_Timer8 tickTimer(MHV_TIMER8_0);
MHV_TIMER_ASSIGN_1INTERRUPT(tickTimer, MHV_TIMER0_INTERRUPTS);

/* A buffer the RTC will use to store events - this determines how many events
 * can be registered simultaneously
 */
#define EVENT_COUNT	10
MHV_EVENT events[EVENT_COUNT];

#define TIMEZONE 600 // UTC+10

// The RTC object we will use
MHV_RTC rtc(&tickTimer, events, EVENT_COUNT, TIMEZONE);

// A timer trigger that will tick the RTC
void rtcTrigger(void *data) {
	/* We can call tick1ms here because we have been careful to configure the
	 * timer for exactly 1ms. If the timer was faster, we would call tick()
	 * instead, which is slightly more expensive.
	 */
	rtc.tick1ms();
}



/* Forward declaration of insertEvent, as there is a circular depedancy
 * between oncePerSecond and insertEvent
 */
void insertEvent(MHV_TIMESTAMP *timestamp);

/* An event that we will trigger every second
 * We will be passed the event that triggered us - we can have parameters
 * passed through the actionData member of the event
 */
void oncePerSecond(MHV_EVENT *event) {
	// Get the current timestamp
	MHV_TIMESTAMP timestamp;
	rtc.current(&timestamp);

	// Convert the timestamp to human readable time
	MHV_TIME time;
	rtc.toTime(&time, &timestamp);

	char buf[90];
	snprintf_P(buf, sizeof(buf), PSTR("The current time is %02u:%02u:%02u %u-%02u-%02u  (%lu.%03u)\r\n"),
			time.hours, time.minutes, time.seconds, time.year, time.month, time.day,
			timestamp.timestamp, timestamp.milliseconds);
	serial.asyncWrite(buf);

	insertEvent(&timestamp);
}

/* Insert an event to be triggered on the next second
 */
inline void insertEvent(MHV_TIMESTAMP *timestamp) {
	// Set up an initial event for the next second
	MHV_EVENT newEvent;
	newEvent.when.milliseconds = 0;
	newEvent.when.timestamp = timestamp->timestamp + 1;
	newEvent.actionFunction = oncePerSecond;
	newEvent.actionData = NULL;
	if (rtc.addEvent(&newEvent)) {
		serial.asyncWrite_P(PSTR("Adding event failed\r\n"));
	}
}


// Program main
int main(void) {
	// Enable interrupts
	sei();

	// Configure the tick timer to tick every 1ms (at 16MHz)
	tickTimer.setPeriods(MHV_TIMER_PRESCALER_5_64, 249, 0);
	tickTimer.setTriggers(rtcTrigger, 0, 0, 0);

	// Prompt the user for the current time
	MHV_TIME time;

	while (true) {
		bool error = false;

		serial.asyncWrite_P(PSTR("Please enter the current time in YYYY MM DD HH MM SS:\r\n"));
		serial.asyncWrite_P(PSTR("                                >"));
		serial.echo(true);
		char input[20];
		(void)serial.busyReadLine(input, sizeof(input));
		serial.echo(false);

		time.milliseconds = 0;
		time.yearday = 0;
		time.year = atoi(input);
		time.month = (MHV_MONTH)atoi(input + 5);
		time.day = atoi(input + 8);
		time.hours = atoi(input + 11);
		time.minutes = atoi(input + 14);
		time.seconds = atoi(input + 17);

		if (time.year < 1970 || time.month > 2106) {
			serial.asyncWrite_P(PSTR("Year must be later than 1970 and less than 2106\r\n"));
			error = true;
		}

		if (time.month > 13 || time.month < 1) {
			serial.asyncWrite_P(PSTR("Month must be between 1 and 12 inclusive\r\n"));
			error = true;
		}

		if (0 == time.day || time.day > mhv_daysInMonth(time.month, time.year)) {
			serial.asyncWrite_P(PSTR("Day must be between 1 and "));
			// Reuse input for string representation of the days in month
			itoa(mhv_daysInMonth(time.month, time.year), input, 10);
			serial.asyncWrite(input);
			serial.asyncWrite_P(PSTR(" inclusive\r\n"));
			error = true;
		}

		if (time.hours > 23) {
			serial.asyncWrite_P(PSTR("Hour must be less than or equal to 23\r\n"));
			error = true;
		}

		if (time.minutes > 59) {
			serial.asyncWrite_P(PSTR("Minutes must be less than or equal to 59\r\n"));
			error = true;
		}

		if (time.seconds > 59) {
			serial.asyncWrite_P(PSTR("Seconds must be less than or equal to 59\r\n"));
			error = true;
		}

		if (!error) {
			// Continue if everything is OK
			break;
		}
	}

	MHV_TIMESTAMP timestamp;
	rtc.toTimestamp(&timestamp, &time);

	// Set the RTC
	rtc.setTime(&timestamp);

	rtc.current(&timestamp);
	char buf[80];
	snprintf_P(buf, sizeof(buf), PSTR("\r\nThe current unix timestamp is %lu\r\n"), timestamp.timestamp);
	serial.asyncWrite(buf);

	serial.asyncWrite_P(PSTR("\r\nStarting clock\r\n"));

	// Start ticking the RTC through its associated timer
	tickTimer.enable();

	// Insert the initial event
	insertEvent(&timestamp);

	// main loop
	for (;;) {
		rtc.runEvents();
	}

	return 0;
}
