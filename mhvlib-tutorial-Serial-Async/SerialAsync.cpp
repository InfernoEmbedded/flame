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

#define MHVLIB_NEED_PURE_VIRTUAL

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

// Bring in the power management header
#include <avr/power.h>

// Bring in stdio, required for snprintf
#include <stdio.h>

// Create a buffer we will use for a receive buffer
#define RX_BUFFER_SIZE	81
// The number of elements we want to be able to store to send asynchronously
#define TX_ELEMENTS_COUNT 10
/* Declare the serial object on USART0 using the above ring buffer
 * Set the baud rate to 115,200
 */
MHV_HARDWARESERIAL_CREATE(serial, RX_BUFFER_SIZE, TX_ELEMENTS_COUNT, MHV_USART0, 115200);


/* We will call this if a write fails
 * Just turn on the LED and stop execution
 */
void NORETURN writeFailed() {
	mhv_setOutput(MHV_ARDUINO_PIN_13);
	mhv_pinOn(MHV_ARDUINO_PIN_13);

	for (;;);
}

MAIN {
	// Disable all peripherals and enable just what we need
	power_all_disable();
	power_usart0_enable();

// Enable interrupts
	sei();

	for (;;) {
// Wait until there is space to send
		while (!serial.canWrite()) {}

/* Write a literal string out
 * If there is no possibility of the write failing, you can cast it to void
 * instead
 *   (void)serial.asyncWrite("some string");
 */
		if (serial.write("asyncWrite: A string has been written\r\n")) {
			writeFailed();
		}

/* Wait until there is more space to send - you can do other work
 * at this point - busy will return false when the serial device is ready
 * to accept another string
 */
		while (!serial.canWrite()) {}

		// Wait until there is space to send
		if (serial.write("asyncWrite: A buffer has been written\r\n this will not show", 39)) {
			writeFailed();
		}


/* Wait until there is more space to send - you can do other work
 * at this point - busy will return false when the serial device is ready
 * to accept another string
 */
		while (!serial.canWrite()) {}

// Write a literal PROGMEM string out
		if (serial.write_P(PSTR("asyncWrite_P: A string has been written\r\n"))) {
			writeFailed();
		}

// Wait until there is space to send
		while (!serial.canWrite()) {}

// Write a PROGMEM buffer out
		(void)serial.write_P(PSTR("asyncWrite_P: A buffer has been written\r\n this will not show"),
				41);
	} // Loop

	return 0;
}
