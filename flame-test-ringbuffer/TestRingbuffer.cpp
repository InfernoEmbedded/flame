/*
 * Copyright (c) 2014, Inferno Embedded
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

/* A simple blinking LED on Arduino pin 13 - a loop with sleeps
 */



// Bring in the FLAME IO header
#include <flame/io.h>

// Bring in the FLAME Serial header
#include <flame/HardwareSerial.h>

// Bring in the AVR delay header (needed for _delay_ms)
#include <util/delay.h>

// Bring in the AVR interrupt header (needed for cli)
#include <avr/interrupt.h>

// Bring in the AVR PROGMEM header, needed to store data in PROGMEM
#include <avr/pgmspace.h>

// Bring in stdio, required for snprintf
#include <stdio.h>

#include <string.h>

using namespace flame;


/* Declare the serial object on UART0
 * Set the baud rate to 115,200
 */
FLAME_HARDWARESERIAL_CREATE(serial, 1, 1, FLAME_USART0, 115200);

// Space for 3 instances of TX_BUFFER
#define TX_BUFFER_SIZE 3 * sizeof(TXBuffer) + 1
RingBufferImplementation<TX_BUFFER_SIZE> testRingBuffer;

void dumpRingBufferState() {
	char buffer[80];
	snprintf(buffer, sizeof(buffer), "Capacity: %i  Length: %i  Head: %i Tail: %i\r\n",
			testRingBuffer.size(), testRingBuffer.length(),
			testRingBuffer.head(), testRingBuffer.tail());
	serial.busyWrite(buffer);
}

MAIN {
// Enable interrupts
	sei();

	char buffer[80];
	serial.busyWrite_P(PSTR("Starting up\r\n"));

	snprintf(buffer, sizeof(buffer), "Allocated %d bytes for buffer, it thinks it is %d bytes\r\n",
			TX_BUFFER_SIZE, testRingBuffer.size());
	serial.busyWrite(buffer);

	dumpRingBufferState();
	snprintf(buffer, sizeof(buffer), "Buffer contains %d out of %d bytes\r\n",
			testRingBuffer.length(), testRingBuffer.size());
	serial.busyWrite(buffer);

	char object[10];
	memset(&object, '_', sizeof(object));

	int i;
	for (i = 0; i < 4; i++) {
		bool fail = testRingBuffer.append(&object, sizeof(object));
		snprintf(buffer, sizeof(buffer),
				"Inserting object of size %d from %p (%s), buffer now contains %d out of %d bytes\r\n",
				sizeof(object), &object, (fail) ? "failed" : "success",
				testRingBuffer.length(), testRingBuffer.size());
		serial.busyWrite(buffer);
		snprintf(buffer, sizeof(buffer), "Buffer is %sfull\r\n", testRingBuffer.full() ? "" : "not ");
		serial.busyWrite(buffer);
		dumpRingBufferState();
	}

	testRingBuffer.flush();
	serial.busyWrite_P(PSTR("Flushed buffer\r\n"));
	dumpRingBufferState();


	const char *data[4];
	data[0] = "1 abcde";
	data[1] = "2 abcde";
	data[2] = "3 abcde";
	data[3] = "4 abcde";

	for (i = 0; i < 4; i++) {
		bool fail = testRingBuffer.append(data[i], 8);
		snprintf(buffer, sizeof(buffer),
				"A Inserting '%s' of size %d (%s), buffer now contains %d out of %d bytes\r\n",
				data[i], 8, (fail) ? "failed" : "success",
				testRingBuffer.length(), testRingBuffer.size());
		serial.busyWrite(buffer);
		snprintf(buffer, sizeof(buffer),
				"Buffer is %sfull\r\n",
				testRingBuffer.full() ? "" : "not ");
		serial.busyWrite(buffer);
		dumpRingBufferState();
	}

	char buf[8];
	for (i = 0; i < 1; ++i) {
		testRingBuffer.consume((void *)buf, 8);
		snprintf(buffer, sizeof(buffer),
				"Consumed %d entries '%s'\r\n",
				i+1, buf);
		serial.busyWrite(buffer);
		dumpRingBufferState();
	}

	snprintf(buffer, sizeof(buffer),
			"A Buffer now contains %d out of %d bytes\r\n",
			testRingBuffer.length(), testRingBuffer.size());
	serial.busyWrite(buffer);


	for (i = 0; i < 4; i++) {
		(void)testRingBuffer.append(data[i], 8);
		snprintf(buffer, sizeof(buffer),
				"A Inserting '%s' of size %d, buffer now contains %d out of %d bytes\r\n",
				data[i], 8, testRingBuffer.length(), testRingBuffer.size());
		serial.busyWrite(buffer);
		snprintf(buffer, sizeof(buffer),
				"Buffer is %sfull\r\n", testRingBuffer.full() ? "" : "not ");
		serial.busyWrite(buffer);
		dumpRingBufferState();
	}

	for (i = 0; i < 4; i++) {
		bool success = !testRingBuffer.consume(buf, 8);

		if (success) {
			snprintf(buffer, sizeof(buffer),
				"read at %d '%s' buffer now contains %d out of %d bytes\r\n",
				i, buf, testRingBuffer.length(), testRingBuffer.size());
		}
		serial.busyWrite(buffer);
		snprintf(buffer, sizeof(buffer),
				"The read did %ssucceed\r\n", success ? "" : "not ");
		serial.busyWrite(buffer);
		snprintf(buffer, sizeof(buffer),
				"Buffer is %sfull\r\n", testRingBuffer.full() ? "" : "not ");
		serial.busyWrite(buffer);
		dumpRingBufferState();
	}

	for (;;) {}

	return 0;
}
