/* Copyright (c) 2011, Make, Hack, Void Inc
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

/* Demonstrate the HT1632 library, using a 2x1 array of
 * Sure Electronics DE-DP105 displays
 */

#define MHVLIB_NEED_PURE_VIRTUAL

#include <avr/interrupt.h>
#include <util/delay.h>
#include <mhvlib/io.h>
#include <mhvlib/Font_SansSerif_10x8.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <boards/MHV_io_Arduino.h>

#include <mhvlib/Display_Holtek_HT1632.h>

using namespace mhvlib;

/* We will use the following pins to communicate with the display
 * Signal	328	1280		Arduino
 * Data		C0	F0			Arduino pin A0
 * WriteClk	C1	F1			Arduino pin A1
 * CS1		C2	F2			Arduino pin A2
 * CS2		C3	F3			Arduino pin A3
 */

// The number of elements we want to be able to store to send asynchronously
#define TX_ELEMENTS_COUNT 10

class DisplaySelector: public Display_Selector {
public:
	/**
	 * Constructor - set the pins to output
	 */
	DisplaySelector() {
		setOutput(MHV_ARDUINO_PIN_A2);
		setOutput(MHV_ARDUINO_PIN_A3);
	}

	/**
	 * Callback for the display selection - sets CS lines appropriately
	 * @param moduleX	the x coordinate of the module
	 * @param moduleY	the y coordinate of the module
	 */
	void select(uint8_t moduleX, UNUSED uint8_t moduleY, bool active) {
		if (active) {
			switch (moduleX) {
			case 0:
				pinOn (MHV_ARDUINO_PIN_A3);
				pinOff (MHV_ARDUINO_PIN_A2);
				break;
			case 1:
				pinOn(MHV_ARDUINO_PIN_A2);
				pinOff(MHV_ARDUINO_PIN_A3);
				break;
			}
		} else {
			pinOn (MHV_ARDUINO_PIN_A2);
			pinOn (MHV_ARDUINO_PIN_A3);
		}
	}
};

DisplaySelector displaySelector;

MHV_HOLTEK_HT1632_CREATE(display, MHV_ARDUINO_PIN_A1, MHV_ARDUINO_PIN_A0, HT1632_MODE::PMOS_24x16,
		24 * 16, 2, 1, displaySelector, TX_ELEMENTS_COUNT);

#define DISPLAY_TYPE Display_Holtek_HT1632< \
	MHV_ARDUINO_PIN_A1, MHV_ARDUINO_PIN_A0, HT1632_MODE::PMOS_24x16, 2, 1, TX_ELEMENTS_COUNT>


/**
 *  Fill up the display column by column, starting from the bottom left
 *  @param	display	the display to draw on
 */
void slowFill(DISPLAY_TYPE &display) {
	uint8_t height = display.getHeight();
	uint8_t width = display.getWidth();

	display.brightness(MHV_HT1632_BRIGHTNESS_MAX);
	display.clear(0);
	display.flush();

	for (uint8_t x = 0; x < width; x++) {
		for (uint8_t y = 0; y < height; y++) {
			display.setPixel(x, y, 1);
			display.flush();
			_delay_ms(100);
		}
	}
}

/**
 * Fade the display in and out
 *  @param	display	the display to draw on
 */
void fader(DISPLAY_TYPE &display) {
	display.brightness(0);
	display.clear(1);
	display.flush();

	uint8_t i;
	uint8_t brightness;

	for (i = 0; i < 3; i++) {
		for (brightness = 0; brightness < MHV_HT1632_BRIGHTNESS_MAX; brightness++) {
			display.brightness(brightness);
			_delay_ms(200);
		}
		for (; brightness > 0; brightness--) {
			display.brightness(brightness);
			_delay_ms(200);
		}
	}
}

/**
 * Render travelling horizontal & vertical lines
 *  @param	display	the display to draw on
 */
void lines(DISPLAY_TYPE &display) {
	uint8_t height = display.getHeight();
	uint8_t width = display.getWidth();

	display.brightness(MHV_HT1632_BRIGHTNESS_MAX);
	display.clear(0);
	display.flush();

	uint16_t x, y;

	for (y = 0; y < height; y++) {
// Draw a horizontal line
		for (x = 0; x < width; x++) {
			display.setPixel(x, y, 1);
		}
		display.flush();
		_delay_ms(200);
// Draw the inverse of the line - cheaper than blanking the whole display
		for (x = 0; x < width; x++) {
			display.setPixel(x, y, 0);
		}
	}
	for (x = 0; x < width; x++) {
// Draw a vertical line
		for (y = 0; y < height; y++) {
			display.setPixel(x, y, 1);
		}
		display.flush();
		_delay_ms(200);
// Draw the inverse of the line - cheaper than blanking the whole display
		for (y = 0; y < height; y++) {
			display.setPixel(x, y, 0);
		}
	}
}

/**
 * Render text - a homage to Portal
 *  @param	display	the display to draw on
 */
void manualTextAnimation(DISPLAY_TYPE &display) {
	display.brightness(MHV_HT1632_BRIGHTNESS_MAX);
	display.clear(0);
	display.flush();

	int16_t offsetX = display.getWidth() - 1;
	int16_t offsetY = 0;

	int16_t startPos = offsetX;

	while (display.writeString_P(fontSansSerif8x10, &offsetX, offsetY, 1, 0, PSTR("This was a triumph!"))) {
		display.flush();
		_delay_ms(80);
		display.clear(0);
		offsetX = startPos--;
	}

	offsetX = display.getWidth() - 1;
	startPos = offsetX;

	bool toggle = true;
	bool more = true;
	while (more) {
		more = display.writeString_P(fontSansSerif8x10, &offsetX, offsetY, 1, 0, PSTR("I'm making a note here: "));

		if (toggle) {
			display.writeString_P(fontSansSerif8x10, &offsetX, offsetY, 1, 0, PSTR("HUGE SUCCESS"));
		}
		toggle = !toggle;
		offsetX = startPos--;

		display.flush();
		_delay_ms(80);
		display.clear(0);
	}

	for (uint8_t i = 0; i < 25; i++) {
		offsetX = 0;
		if (toggle) {
			display.writeString_P(fontSansSerif8x10, &offsetX, offsetY, 1, 0, PSTR("HUGE SUCCESS!!"));
		}

		display.flush();
		_delay_ms(80);
		display.clear(0);

		toggle = !toggle;
	}

	for (offsetY = -7; offsetY < 16; ++offsetY) {
		offsetX = 0;
		display.writeString_P(fontSansSerif8x10, &offsetX, offsetY, 1, 0, PSTR("It's hard to"));
		offsetX = 0;
		display.writeString_P(fontSansSerif8x10, &offsetX, offsetY - 8, 1, 0, PSTR("overstate my"));
		offsetX = 0;
		display.writeString_P(fontSansSerif8x10, &offsetX, offsetY - 16, 1, 0, PSTR("satisfaction"));
		display.flush();
		_delay_ms(100);
		display.clear(0);
	}

	offsetX = 0;
	display.writeString_P(fontSansSerif8x10, &offsetX, 0, 1, 0, PSTR("satisfaction"));
	display.flush();
	for (uint8_t i = 0; i < 3; i++) {
		uint8_t brightness;

		for (brightness = MHV_HT1632_BRIGHTNESS_MAX; brightness > 0 ; brightness--) {
			display.brightness(brightness);
			_delay_ms(70);
		}
		for (; brightness < MHV_HT1632_BRIGHTNESS_MAX; brightness++) {
			display.brightness(brightness);
			_delay_ms(70);
		}
	}

	display.clear(0);
}

/**
 * Render text using the asynchronous buffers
 *  @param	display	the display to draw on
 */
void textAnimation(DISPLAY_TYPE &display) {
	display.write("1. Here is a string of text");
	display.write_P(PSTR("2. Here is string of text in PROGMEM"));
	display.write("3. Here is a buffer containing some data//This will not show", 40);
	display.write_P(PSTR("4. Here is a buffer in PROGMEM containing some data//This will not show"), 51);
	while (display.txAnimation(fontSansSerif8x10, 0, 1, 0)) {
		display.flush();
		_delay_ms(40);
	}
	display.flush();
}

MAIN {
	// Disable all peripherals and enable just what we need
	power_all_disable();

	// Enable global interrupts
	sei();

	for (;;) {
		display.setPixel(0, 0, 1);
		display.setPixel(23, 15, 1);
		display.setPixel(0, 15, 1);
		display.setPixel(23, 0, 1);
		lines(display);
		textAnimation(display);
		fader(display);
		manualTextAnimation(display);
		slowFill(display);
	}

	return 0;
}
