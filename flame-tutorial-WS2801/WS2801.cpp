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

/* Drive a string of WS2801 controlled LEDs,
 */

// Bring in the FLAME IO header
#include <flame/io.h>

// Bring in the AVR delay header (needed for _delay_ms)
#include <util/delay.h>
#include <avr/power.h>

// The number of LEDs in the string
#define LEDS	32*5

// Bring in the WS2801 driver
#include <flame/WS2801.h>

using namespace flame;

// Instantiate the driver
WS2801<FLAME_PIN_B0, FLAME_PIN_B1, LEDS> ws2801;

// Bring in the FLAME Serial header
#include <flame/HardwareSerial.h>

// Create a buffer we will use for a receive buffer
#define RX_BUFFER_SIZE	3
// The number of elements we want to be able to store to send asynchronously
#define TX_ELEMENTS_COUNT 10
/* Declare the serial object on USART0 using the above ring buffer
 * Set the baud rate to 115,200
 */
FLAME_HARDWARESERIAL_CREATE(serial, RX_BUFFER_SIZE, TX_ELEMENTS_COUNT, FLAME_USART0, 115200);


MAIN {
	// Enable interrupts
	sei();

	setOutput(FLAME_PIN_B3);

	serial.write_P(PSTR("Setting to 50% brightness\r\n"));

	ws2801.setAll(128, 128, 128);
	ws2801.flush();

	for (;;) {
		serial.write_P(PSTR("Fading up blue\r\n"));
		for (uint8_t i = 0; i < 255; i++) {
			ws2801.setAllGamma(0, 0, i);
			ws2801.flush();
			_delay_ms(5);
		}

		serial.write_P(PSTR("Fading from blue to red\r\n"));
		for (uint8_t i = 0; i < 255; i++) {
			ws2801.setAllGamma(i, 0, 255 - i);
			ws2801.flush();
			_delay_ms(5);
		}

		serial.write_P(PSTR("Fading from red to green\r\n"));
		for (uint8_t i = 0; i < 255; i++) {
			ws2801.setAllGamma(255 - i, i, 0);
			ws2801.flush();
			_delay_ms(5);
		}

		serial.write_P(PSTR("Fading from green to blue\r\n"));
		for (uint8_t i = 0; i < 255; i++) {
			ws2801.setAllGamma(0, 255 - i, i);
			ws2801.flush();
			_delay_ms(5);
		}

		serial.write_P(PSTR("Fading from blue to magenta\r\n"));
		for (uint8_t i = 0; i < 255; i++) {
			ws2801.setAllGamma(i, 0, 255);
			ws2801.flush();
			_delay_ms(5);
		}

		serial.write_P(PSTR("Fading from magenta to white\r\n"));
		for (uint8_t i = 0; i < 255; i++) {
			ws2801.setAllGamma(255, i, 255);
			ws2801.flush();
			_delay_ms(5);
		}

		serial.write_P(PSTR("Fading from white to black\r\n"));
		for (uint8_t i = 0; i < 255; i++) {
			ws2801.setAllGamma(255 - i, 255 - i, 255 - i);
			ws2801.flush();
			_delay_ms(5);
		}

		ws2801.setAll(0, 0, 0);
		int16_t i;
		uint8_t led = 0;
		for (i = 0; i < 240; i += 10) {
			ws2801.setPixelGamma(led++, (uint8_t)i / 2, (uint8_t)i, 0);
		}
		for (; i > 0; i -= 10) {
			ws2801.setPixelGamma(led++, (uint8_t)i / 1.5, (uint8_t)i, 0);
		}
		serial.write_P(PSTR("Rotating forward\r\n"));
		for (i = 0; i < LEDS - led; i++) {
			ws2801.flush();
			_delay_ms(1);
			ws2801.rotate(true);
		}

		serial.write_P(PSTR("Rotating backwards\r\n"));
		for (i = 0; i < LEDS - led; i++) {
			ws2801.flush();
			_delay_ms(1);
			ws2801.rotate(false);
		}
	}

	return 0;
}

