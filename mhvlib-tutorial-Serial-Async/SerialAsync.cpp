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


/* Required for C++
 * Stop processing if a pure virtual function is called
 */
extern "C" void __cxa_pure_virtual() {
// Disable interrupts
	cli();
// Do nothing forever
	for (;;);
}

// Create a buffer we will use for a receive buffer
#define RX_BUFFER_SIZE	81
char rxBuf[RX_BUFFER_SIZE];

// Create a ring buffer wrapping the memory allocated above
MHV_RingBuffer rxBuffer(rxBuf, RX_BUFFER_SIZE);

/* Declare the serial object on UART0 using the above ring buffer
 * Set the baud rate to 115,200
 */
MHV_HardwareSerial serial(&rxBuffer, MHV_USART0, 115200);

// Assign interrupts to the serial object
MHV_HARDWARESERIAL_ASSIGN_INTERRUPTS(serial, MHV_USART0_INTERRUPTS);

// Allocate a string in PROGMEM - this does not consume any RAM, only flash
char progmemString[] PROGMEM = "asyncWrite_P: A string has been written";

// Allocate a buffer in PROGMEM - this does not consume any RAM, only flash
char progmemBuffer[] PROGMEM = "asyncWrite_P: A buffer has been written\r\n";


/* We will call this if a write fails
 * Just turn on the LED and stop execution
 */
void writeFailed(void) {
	mhv_setOutput(MHV_ARDUINO_PIN_13);
	mhv_pinOn(MHV_ARDUINO_PIN_13);

	for (;;);
}

/* We will call this if a read fails
 * Just turn on the LED and stop execution
 */
void readFailed(void) {
	mhv_setOutput(MHV_ARDUINO_PIN_13);
	mhv_pinOn(MHV_ARDUINO_PIN_13);

	for (;;);
}


int main(void) {
// Enable interrupts
	sei();

	while (1) {
/* Write a literal string out
 * If there is no possibility of the write failing, you can cast it to void
 * instead
 *   (void)serial.asyncWrite("some string");
 */
		if (serial.asyncWrite("asyncWrite: A string has been written\r\n")) {
			writeFailed();
		}

/* Wait until the previous write has been completed - you can do other work
 * at this point - busy will return false when the serial device is ready
 * to accept another string
 */
		while (!serial.canSend()) {}

// Write a literal PROGMEM string out
		if (serial.asyncWrite_P(progmemString)) {
			writeFailed();
		}

		while (!serial.canSend()) {}

/* Write single characters (the CR/LF for the PROGMEM string)
 * Since single characters return as soon as the hardware has the byte,
 * there is no need for a seperate async version of it
 */
		(void)serial.busyWrite('\r');
		(void)serial.busyWrite('\n');

// Wait for the last busyWrite to complete
		while (!serial.canSend()) {}

// Write a buffer out
		(void)serial.asyncWrite_P(progmemBuffer, sizeof(progmemBuffer));

// Wait for the last asyncWrite to complete
		while (!serial.canSend()) {}
	} // Loop

// Main must return an int, even though we never get here
	return 0;
}
