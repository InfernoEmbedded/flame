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
 *  * Neither the name of the Inferno Embedded nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL INFERNO EMBEDDED BE LIABLE FOR ANY
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
#include <flame/Pin.h>
#include <boards/Arduino.h>

// Bring in the FLAME Serial header
#include <flame/HardwareSerial.h>

// Bring in the AVR interrupt header (needed for sei)
#include <avr/interrupt.h>

// Bring in the AVR PROGMEM header, needed to store data in PROGMEM
#include <avr/pgmspace.h>

// Bring in the power management header
#include <avr/power.h>

using namespace flame;

PinImplementation<FLAME_ARDUINO_PIN_13> failureLED;


// Create a buffer we will use for a receive buffer
#define RX_BUFFER_SIZE	81
// The number of elements we want to be able to store to send asynchronously
#define TX_ELEMENTS_COUNT 10
/* Declare the serial object on USART0 using the above ring buffer
 * Set the baud rate to 115,200
 */
FLAME_HARDWARESERIAL_CREATE(serial, RX_BUFFER_SIZE, TX_ELEMENTS_COUNT, FLAME_USART0, 115200);


/* We will call this if a write fails
 * Just turn on the LED and stop execution
 */
void NORETURN writeFailed() {
	failureLED.setOutput();
	failureLED.on();

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
		if (serial.write("asyncWrite: 1 A string has been written\r\n")) {
			writeFailed();
		}

/* Wait until there is more space to send - you can do other work
 * at this point - busy will return false when the serial device is ready
 * to accept another string
 */
		while (!serial.canWrite()) {}

		// Wait until there is space to send
		if (serial.write("asyncWrite: 2 A buffer has been written\r\n this will not show", 42)) {
			writeFailed();
		}


/* Wait until there is more space to send - you can do other work
 * at this point - busy will return false when the serial device is ready
 * to accept another string
 */
		while (!serial.canWrite()) {}

// Write a literal PROGMEM string out
		if (serial.write_P(PSTR("asyncWrite_P: 3 A string has been written\r\n"))) {
			writeFailed();
		}

// Wait until there is space to send
		while (!serial.canWrite()) {}

// Write a PROGMEM buffer out
		(void)serial.write_P(PSTR("asyncWrite_P: 4 A buffer has been written\r\n this will not show"),
				44);

// Wait until there is space to send
		while (!serial.canWrite()) {}
		(void)serial.printf(PSTR("You can use printf to format output containing numbers such as %i and other %s\r\n"),
				5, "strings");
	} // Loop

	return 0;
}
