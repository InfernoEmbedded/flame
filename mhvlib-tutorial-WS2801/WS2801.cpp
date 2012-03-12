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

/* Drive a string of WS2801 controlled LEDs,
 */

#define MHVLIB_NEED_PURE_VIRTUAL

// Bring in the MHV IO header
#include <MHV_io.h>

// Bring in the AVR delay header (needed for _delay_ms)
#include <util/delay.h>
#include <avr/power.h>

// The number of LEDs in the string
#define LEDS	32*5

// The pins for comms with the string
#define MHV_SHIFT_ORDER_MSB
#define MHV_SHIFT_DELAY			_delay_us(2);
#define MHV_SHIFT_WRITECLOCK	MHV_PIN_B0
#define MHV_SHIFT_WRITEDATA		MHV_PIN_B1

// Bring in the WS2801 driver
#include <MHV_WS2801.h>

// Instantiate the driver
MHV_WS2801_CREATE(ws2801, LEDS);

MAIN {
	mhv_setOutput(MHV_SHIFT_WRITECLOCK);
	mhv_setOutput(MHV_SHIFT_WRITEDATA);

	ws2801.setAll(128, 128, 128);
	ws2801.flush();

	for (;;) {
		for (uint8_t i = 0; i < 255; i++) {
			ws2801.setAllGamma(0, 0, i);
			ws2801.flush();
			_delay_ms(5);
		}

		for (uint8_t i = 0; i < 255; i++) {
			ws2801.setAllGamma(i, 0, 255 - i);
			ws2801.flush();
			_delay_ms(5);
		}

		for (uint8_t i = 0; i < 255; i++) {
			ws2801.setAllGamma(255 - i, i, 0);
			ws2801.flush();
			_delay_ms(5);
		}

		for (uint8_t i = 0; i < 255; i++) {
			ws2801.setAllGamma(0, 255 - i, i);
			ws2801.flush();
			_delay_ms(5);
		}

		for (uint8_t i = 0; i < 255; i++) {
			ws2801.setAllGamma(i, 0, 255);
			ws2801.flush();
			_delay_ms(5);
		}

		for (uint8_t i = 0; i < 255; i++) {
			ws2801.setAllGamma(255, i, 255);
			ws2801.flush();
			_delay_ms(5);
		}

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
		for (i = 0; i < LEDS - led; i++) {
			ws2801.flush();
			_delay_ms(1);
			ws2801.rotate(true);
		}
		for (i = 0; i < LEDS - led; i++) {
			ws2801.flush();
			_delay_ms(1);
			ws2801.rotate(false);
		}
	}
}

