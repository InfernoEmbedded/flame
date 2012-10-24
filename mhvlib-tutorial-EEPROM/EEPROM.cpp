/*
 * Copyright (c) 2012, Make, Hack, Void Inc
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

/* Demonstrates EEPROM access
 * Displays the current EEPROM contents, then prompts the user to update it
 *
 * The data in EEPROM is stored as follows:
 * Address 0:	2 bytes representing the length in EEPROM
 * Address 2:	2 bytes representing
 * Address 4+:	String data
 */



#include <mhvlib/io.h>
#include <mhvlib/HardwareSerial.h>
#include <mhvlib/EEPROM.h>
#include <util/CRC16.h>
#include <util/delay.h>
#include <string.h>
#include <avr/power.h>
#include <avr/sleep.h>

using namespace mhvlib;

// Chosen to fit within all supported MCUs - yours may have more space available
#define DATA_MAX_LENGTH ((uint16_t)(128 - DATA_ADDRESS))

#define LENGTH_ADDRESS ((uint16_t)0)
#define CRC_ADDRESS (LENGTH_ADDRESS + (uint16_t)sizeof(uint16_t))
#define DATA_ADDRESS (CRC_ADDRESS + (uint16_t)sizeof(uint16_t))

// Create the EEPROM accessor
MHV_EEPROM_CREATE(eeprom);

// Create the serial port
#define RX_BUFFER_SIZE	DATA_MAX_LENGTH
#define TX_ELEMENTS_COUNT 1
MHV_HARDWARESERIAL_CREATE(serial, RX_BUFFER_SIZE, TX_ELEMENTS_COUNT, MHV_USART0, 115200);


/**
 * Calculate the CRC of the data string in EEPROM
 * Uses the CRC functions from AVR LibC
 *
 * @param length	the length of the data string
 * @return the CRC
 */
uint16_t calculateCRC(uint16_t length) {
	uint16_t crc = 0;

	for (uint16_t i = 0; i < length; i++) {
		crc = _crc16_update(crc, eeprom.busyRead(DATA_ADDRESS + i));
	}

	return crc;
}


MAIN {
	// Get the current length of the string in EEPROM
	uint16_t 	length;
	uint16_t 	crc;
	char		buf[DATA_MAX_LENGTH];

	// Disable all peripherals and enable just what we need
	power_all_disable();
	power_usart0_enable();

	// Enable interrupts
	sei();

	for (;;) {
		eeprom.read(&length, LENGTH_ADDRESS, (uint16_t) sizeof(length));

		if (length > DATA_MAX_LENGTH) {
			serial.busyWrite_P(PSTR("EEPROM length is invalid\r\n"));
		} else {
			// Validate the EEPROM contents
			eeprom.busyRead(&crc, CRC_ADDRESS, (uint16_t) sizeof(crc));
			uint16_t calculatedCRC = calculateCRC(length);
			if (crc == calculatedCRC) {
				serial.busyWrite_P(PSTR("EEPROM checksum is valid\r\n"));

				// Read the EEPROM contents into the buffer
				eeprom.busyRead(buf, DATA_ADDRESS, length);

				serial.busyWrite_P(PSTR("EEPROM contains: '"));
				serial.busyWrite(buf, length);
				serial.busyWrite_P(PSTR("'\r\n"));
			} else {
				serial.busyWrite_P(PSTR("EEPROM checksum is invalid\r\n"));
			}
		}

		// Prompt the user for data to write to the EEPROM
		serial.busyWrite_P(PSTR("What would you like to enter into the EEPROM? "));
		serial.echo(true);
		(void) serial.busyReadLine(buf, sizeof(buf));
		serial.echo(false);
		length = strlen(buf);

		// Write the length to the EEPROM
		serial.busyWrite_P(PSTR("\r\nWriting length\r\n"));
		eeprom.busyWrite(&length, LENGTH_ADDRESS, sizeof(length));

		// Write the data to the EEPROM asynchronously
		serial.busyWrite_P(PSTR("Writing data"));
		eeprom.write(buf, DATA_ADDRESS, length, NULL, NULL);
		while (eeprom.isBusy()) {
			serial.write_P(PSTR("."));
			_delay_ms(1);
		}
		serial.busyWrite_P(PSTR("\r\nDone\r\n"));

		// Write the checksum to the EEPROM
		uint16_t calculatedCRC = calculateCRC(length);
		serial.busyWrite_P(PSTR("Writing CRC\r\n"));
		eeprom.busyWrite(&calculatedCRC, CRC_ADDRESS, sizeof(calculatedCRC));
	}

	return 0;
}
