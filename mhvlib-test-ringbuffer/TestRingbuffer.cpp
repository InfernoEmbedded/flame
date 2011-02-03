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

/* A simple blinking LED on Arduino pin 13 - a loop with sleeps
 */

// Bring in the MHV IO header
#include <MHV_io.h>

// Bring in the MHV Serial header
#include <MHV_HardwareSerial.h>

// Bring in the AVR delay header (needed for _delay_ms)
#include <util/delay.h>

// Bring in the AVR interrupt header (needed for cli)
#include <avr/interrupt.h>

// Bring in the AVR PROGMEM header, needed to store data in PROGMEM
#include <avr/pgmspace.h>

// Bring in stdio, required for snprintf
#include <stdio.h>

#include <string.h>


/* Declare the serial object on UART0
 *
 * The NULL parameter is a buffer used for keeping track of data
 * sent asynchronously - see the Serial-Async tutorial for usage
 *
 * Use the USART0 hardware
 *
 * Set the baud rate to 115,200
 */
MHV_HardwareSerial serial(NULL, NULL, MHV_USART0, 115200);

// Assign interrupts to the serial object
MHV_HARDWARESERIAL_ASSIGN_INTERRUPTS(serial, MHV_USART0_INTERRUPTS);

#define TX_ELEMENTS_COUNT 3
#define TX_BUFFER_SIZE TX_ELEMENTS_COUNT * sizeof(MHV_TX_BUFFER) + 1
char txBuf[TX_BUFFER_SIZE];
MHV_RingBuffer txBuffer(txBuf, TX_BUFFER_SIZE);

int main(void) {
// Enable interrupts
	sei();

	char buffer[80];
	snprintf(buffer, sizeof(buffer), "Starting up, ring buffer is at %p\r\n", txBuf);
	serial.busyWrite(buffer);

	snprintf(buffer, sizeof(buffer), "Allocated %d bytes for buffer, it thinks it is %d bytes\r\n", TX_BUFFER_SIZE, txBuffer.size());
	serial.busyWrite(buffer);

	snprintf(buffer, sizeof(buffer), "Buffer contains %d out of %d bytes\r\n", txBuffer.length(), txBuffer.size());
	serial.busyWrite(buffer);

	MHV_TX_BUFFER object;
	memset(&object, '_', sizeof(object));

	int i;
	for (i = 0; i < 4; i++) {
		txBuffer.append(&object, sizeof(object));
		snprintf(buffer, sizeof(buffer), "Inserting object of size %d from %p, buffer now contains %d out of %d bytes\r\n",
				sizeof(object), &object, txBuffer.length(), txBuffer.size());
		serial.busyWrite(buffer);
		snprintf(buffer, sizeof(buffer), "Buffer is %sfull\r\n", txBuffer.full() ? "" : "not ");
		serial.busyWrite(buffer);
	}

	txBuffer.flush();

	const char *data[4];
	data[0] = "1 abcde";
	data[1] = "2 abcde";
	data[2] = "3 abcde";
	data[3] = "4 abcde";

	for (i = 0; i < 4; i++) {
		(void)txBuffer.append(data[i], 8);
		snprintf(buffer, sizeof(buffer), "A Inserting '%s' of size %d, buffer now contains %d out of %d bytes\r\n",
				data[i], 8, txBuffer.length(), txBuffer.size());
		serial.busyWrite(buffer);
		snprintf(buffer, sizeof(buffer), "Buffer is %sfull\r\n", txBuffer.full() ? "" : "not ");
		serial.busyWrite(buffer);
	}

	char buf[8];
	for (i = 0; i < 1; ++i) {
		snprintf(buffer, sizeof(buffer), "Consumed %d entries\r\n", i+1);
		serial.busyWrite(buffer);
		txBuffer.consume((void *)buf, 8);
	}

	snprintf(buffer, sizeof(buffer), "A Buffer now contains %d out of %d bytes\r\n",
			txBuffer.length(), txBuffer.size());
	serial.busyWrite(buffer);


	for (i = 0; i < 4; i++) {
		(void)txBuffer.append(data[i], 8);
		snprintf(buffer, sizeof(buffer), "A Inserting '%s' of size %d, buffer now contains %d out of %d bytes\r\n",
				data[i], 8, txBuffer.length(), txBuffer.size());
		serial.busyWrite(buffer);
		snprintf(buffer, sizeof(buffer), "Buffer is %sfull\r\n", txBuffer.full() ? "" : "not ");
		serial.busyWrite(buffer);
	}

	for (i = 0; i < 4; i++) {
		bool success = !txBuffer.consume(buf, 8);
		snprintf(buffer, sizeof(buffer), "read at %d '%s' buffer now contains %d out of %d bytes\r\n",
			i, buf, txBuffer.length(), txBuffer.size());
		serial.busyWrite(buffer);
		snprintf(buffer, sizeof(buffer), "The read did %ssucceed\r\n", success ? "" : "not ");
		serial.busyWrite(buffer);
		snprintf(buffer, sizeof(buffer), "Buffer is %sfull\r\n", txBuffer.full() ? "" : "not ");
		serial.busyWrite(buffer);
	}

	while (1) {}

// Main must return an int, even though we never get here
	return 0;
}
