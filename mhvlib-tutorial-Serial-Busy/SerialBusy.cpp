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

/* Demonstrates busy writing to a serial port
 */

// Bring in the MHV IO header
#include <MHV_io.h>

// Bring in the MHV Serial header
#include <MHV_HardwareSerial.h>

// Bring in the AVR interrupt header (needed for cli)
#include <avr/interrupt.h>

// Bring in the AVR PROGMEM header, needed to store data in PROGMEM
#include <avr/pgmspace.h>

// Bring in stdio, required for snprintf
#include <stdlib.h>
#include <stdio.h>

// Create a buffer we will use for a receive buffer
#define RX_BUFFER_SIZE	81
char rxBuf[RX_BUFFER_SIZE];

// Create a ring buffer wrapping the memory allocated above
MHV_RingBuffer rxBuffer(rxBuf, RX_BUFFER_SIZE);

/* Declare the serial object on UART0 using the above ring buffer
 * No TX buffers provided, async write operations will not work
 * Use the USART0 hardware
 * Set the baud rate to 115,200
 */
MHV_HardwareSerial serial(&rxBuffer, NULL, MHV_USART0, 115200);

// Assign interrupts to the serial object
MHV_HARDWARESERIAL_ASSIGN_INTERRUPTS(serial, MHV_USART0_INTERRUPTS);

int main(void) {
// Enable interrupts
	sei();

	while (1) {
/* Write a literal string out
 * If there is no possibility of the write failing, you can cast it to void
 * instead
 *   (void)serial.busyWrite("some string");
 */
		serial.busyWrite("busyWrite: A string has been written\r\n");

/* Write a literal PROGMEM string out
 * PROGMEM data is only stored in flash, and does not consume any RAM, allowing
 * you to write tighter programs and make better use of the space available on
 * the microcontroller. See the avr/pgmspace.h documentation for more info
 */
		serial.busyWrite_P(PSTR("busyWrite_P: A string has been written"));

// Write single characters (the CR/LF for the PROGMEM string)
		(void)serial.busyWrite('\r');
		(void)serial.busyWrite('\n');

/* Write a PROGMEM buffer out
 * Note that we specify a length of how many bytes we want written
 */
		(void)serial.busyWrite_P(
			PSTR("busyWrite_P: A buffer has been written\r\n this should not be seen"), 40);


// Prompt the user for a single character and tell them what they typed
		serial.busyWrite("Ok, now give me a character: ");
// Turn on echo so the user can see what they typed
		serial.echo(true);
		int c;
		while (-1 == (c = serial.read())) {}
		serial.echo(false);

/* Use sprintf to populate a buffer
 */
		char buffer[30];
		snprintf(buffer, sizeof(buffer), "\r\nYou typed '%c'\r\n", (char)c);
		serial.busyWrite(buffer);

// Prompt the user for a line and tell them what type typed
		serial.busyWrite("Ok, now give me some text and press enter: ");
// Turn on echo so the user can see what they typed
		serial.echo(true);
// Read the line and store it in the buffer we allocated above
		switch (serial.busyReadLine(buffer, sizeof(buffer))) {
		case -2:
// Overflowed the output buffer
			serial.busyWrite("\r\nYou gave me way too much information, I have truncated it.\r\n");
// Discard the remaining user data
			serial.flush();
			break;
		case -3:
/* Overflowed the ringbuffer - this should not occur in this example as the
 * output buffer overflow takes precedence
 */
			serial.busyWrite("\nYou gave me way too much information and overflowed the ringbuffer.\r\n");
			break;
		}
		serial.echo(false);
		serial.busyWrite("\nYou said: '");
		serial.busyWrite(buffer);
		serial.busyWrite("'\r\n");
	} // Loop

// Main must return an int, even though we never get here
	return 0;
}
