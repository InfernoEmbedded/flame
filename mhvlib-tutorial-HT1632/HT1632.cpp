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

/* Demonstrate the HT1632 library, using a 2x1 array of
 * Sure Electronics DE-DP105 displays
 */

#include <avr/interrupt.h>
#include <util/delay.h>
#include <MHV_io.h>
#include <MHV_Display_Holtek_HT1632.h>
#include <MHV_Font_SansSerif_10x8.h>

#include <avr/interrupt.h>


#include <MHV_HardwareSerial.h>
// Create a buffer we will use for a receive buffer
#define RX_BUFFER_SIZE	81
char rxBuf[RX_BUFFER_SIZE];

// Create a ring buffer wrapping the memory allocated above
MHV_RingBuffer rxBuffer(rxBuf, RX_BUFFER_SIZE);


// The number of elements we want to be able to store to send asynchronously
#define TX_ELEMENTS_COUNT 10
#define TX_BUFFER_SIZE TX_ELEMENTS_COUNT * sizeof(MHV_TX_BUFFER) + 1
// A buffer for the serial port to send data, it only contains pointers
char serTxBuf[TX_BUFFER_SIZE];
MHV_RingBuffer serTxBuffer(serTxBuf, TX_BUFFER_SIZE);

/* Declare the serial object on USART0 using the above ring buffer
 * Set the baud rate to 115,200
 */
MHV_HardwareSerial serial(&rxBuffer, &serTxBuffer, MHV_USART0, 115200);

// Assign interrupts to the serial object
MHV_HARDWARESERIAL_ASSIGN_INTERRUPTS(serial, MHV_USART0_INTERRUPTS);



/*
 * Required as the display classes have pure virtual methods
 * This will only get called if a pure virtual method is called in a constructor (never in MHVlib)
 */
extern "C" void __cxa_pure_virtual() {
	cli();
	for (;;);
}

/* We will use the following pins to communicate with the display
 * Signal	328	1280		Arduino
 * Data		C0	F0			Arduino pin A0
 * WriteClk	C1	F1			Arduino pin A1
 * CS1		C2	F2			Arduino pin A2
 * CS2		C3	F3			Arduino pin A3
 */

// The number of elements we want to be able to store to send asynchronously
#define TX_ELEMENTS_COUNT 10
#define TX_BUFFER_SIZE TX_ELEMENTS_COUNT * sizeof(MHV_TX_BUFFER) + 1
// A buffer for the display to send data, it only contains pointers
char txBuf[TX_BUFFER_SIZE];
MHV_RingBuffer txBuffer(txBuf, TX_BUFFER_SIZE);


/**
 * Callback for the display selection - sets CS lines appropriately
 * @param moduleX	the x coordinate of the module
 * @param moduleY	the y coordinate of the module
 */
void displaySelect(uint8_t moduleX, uint8_t moduleY, bool active) {
	if (active) {
		switch (moduleX) {
		case 0:
			mhv_pinOn(MHV_ARDUINO_PIN_A3);
			mhv_pinOff(MHV_ARDUINO_PIN_A2);
			break;
		case 1:
			mhv_pinOn(MHV_ARDUINO_PIN_A2);
			mhv_pinOff(MHV_ARDUINO_PIN_A3);
			break;
		}
	} else {
		mhv_pinOn(MHV_ARDUINO_PIN_A2);
		mhv_pinOn(MHV_ARDUINO_PIN_A3);
	}
}

/**
 *  Fill up the display column by column, starting from the bottom left
 *  @param	display	the display to draw on
 */
void slowFill(MHV_Display_Holtek_HT1632 *display) {
	uint8_t height = display->getHeight();
	uint8_t width = display->getWidth();

	display->brightness(MHV_HT1632_BRIGHTNESS_MAX);
	display->clear(0);
	display->flush();

	for (uint8_t x = 0; x < width; x++) {
		for (uint8_t y = 0; y < height; y++) {
			display->setPixel(x, y, 1);
			display->flush();
			_delay_ms(40);
		}
	}
}

/**
 * Fade the display in and out
 *  @param	display	the display to draw on
 */
void fader(MHV_Display_Holtek_HT1632 *display) {
	display->brightness(0);
	display->clear(1);
	display->flush();

	uint8_t i;
	uint8_t brightness;

	for (i = 0; i < 3; i++) {
		for (brightness = 0; brightness < MHV_HT1632_BRIGHTNESS_MAX; brightness++) {
			display->brightness(brightness);
			_delay_ms(200);
		}
		for (; brightness > 0; brightness--) {
			display->brightness(brightness);
			_delay_ms(200);
		}
	}
}

/**
 * Render travelling horizontal & vertical lines
 *  @param	display	the display to draw on
 */
void lines(MHV_Display_Holtek_HT1632 *display) {
	uint8_t height = display->getHeight();
	uint8_t width = display->getWidth();

	display->brightness(MHV_HT1632_BRIGHTNESS_MAX);
	display->clear(0);
	display->flush();

	uint16_t x, y;

	for (y = 0; y < height; y++) {
// Draw a horizontal line
		for (x = 0; x < width; x++) {
			display->setPixel(x, y, 1);
		}
		display->flush();
		_delay_ms(200);
// Draw the inverse of the line - cheaper than blanking the whole display
		for (x = 0; x < width; x++) {
			display->setPixel(x, y, 0);
		}
	}
	for (x = 0; x < width; x++) {
// Draw a vertical line
		for (y = 0; y < height; y++) {
			display->setPixel(x, y, 1);
		}
		display->flush();
		_delay_ms(200);
// Draw the inverse of the line - cheaper than blanking the whole display
		for (y = 0; y < height; y++) {
			display->setPixel(x, y, 0);
		}
	}
}

/**
 * Render text - a homage to Portal
 *  @param	display	the display to draw on
 */
void manualTextAnimation(MHV_Display_Holtek_HT1632 *display) {
	display->brightness(MHV_HT1632_BRIGHTNESS_MAX);
	display->clear(0);
	display->flush();

	int16_t offsetX = display->getWidth() - 1;
	int16_t offsetY = 0;

	int16_t startPos = offsetX;

	while (display->writeString_P(&mhv_fontSansSerif8x10, &offsetX, offsetY, 1, 0, PSTR("This was a triumph!"))) {
		display->flush();
		_delay_ms(80);
		display->clear(0);
		offsetX = startPos--;
	}

	offsetX = display->getWidth() - 1;
	startPos = offsetX;

	bool toggle = true;
	bool more = true;
	while (more) {
		more = display->writeString_P(&mhv_fontSansSerif8x10, &offsetX, offsetY, 1, 0, PSTR("I'm making a note here: "));

		if (toggle) {
			display->writeString_P(&mhv_fontSansSerif8x10, &offsetX, offsetY, 1, 0, PSTR("HUGE SUCCESS"));
		}
		toggle = !toggle;
		offsetX = startPos--;

		display->flush();
		_delay_ms(80);
		display->clear(0);
	}

	for (uint8_t i = 0; i < 25; i++) {
		offsetX = 0;
		if (toggle) {
			display->writeString_P(&mhv_fontSansSerif8x10, &offsetX, offsetY, 1, 0, PSTR("HUGE SUCCESS!!"));
		}

		display->flush();
		_delay_ms(80);
		display->clear(0);

		toggle = !toggle;
	}

	for (offsetY = -7; offsetY < 16; ++offsetY) {
		offsetX = 0;
		display->writeString_P(&mhv_fontSansSerif8x10, &offsetX, offsetY, 1, 0, PSTR("It's hard to"));
		offsetX = 0;
		display->writeString_P(&mhv_fontSansSerif8x10, &offsetX, offsetY - 8, 1, 0, PSTR("overstate my"));
		offsetX = 0;
		display->writeString_P(&mhv_fontSansSerif8x10, &offsetX, offsetY - 16, 1, 0, PSTR("satisfaction"));
		display->flush();
		_delay_ms(100);
		display->clear(0);
	}

	offsetX = 0;
	display->writeString_P(&mhv_fontSansSerif8x10, &offsetX, 0, 1, 0, PSTR("satisfaction"));
	display->flush();
	for (uint8_t i = 0; i < 3; i++) {
		uint8_t brightness;

		for (brightness = MHV_HT1632_BRIGHTNESS_MAX; brightness > 0 ; brightness--) {
			display->brightness(brightness);
			_delay_ms(70);
		}
		for (; brightness < MHV_HT1632_BRIGHTNESS_MAX; brightness++) {
			display->brightness(brightness);
			_delay_ms(70);
		}
	}


	display->clear(0);
}

/**
 * Render text using the asynchronous buffers
 *  @param	display	the display to draw on
 */
void textAnimation(MHV_Display_Holtek_HT1632 *display) {
	display->write("1. Here is a string of text");
	display->write_P(PSTR("2. Here is string of text in PROGMEM"));
	display->write("3. Here is a buffer containing some data//This will not show", 40);
	display->write_P(PSTR("4. Here is a buffer in PROGMEM containing some data//This will not show"), 51);
	while (display->txAnimation(&mhv_fontSansSerif8x10, 0, 1, 0)) {
		display->flush();
		_delay_ms(40);
	}
	display->flush();
}

int main(void) {
	// Enable output on the display pins
	mhv_setOutput(MHV_ARDUINO_PIN_A0);
	mhv_setOutput(MHV_ARDUINO_PIN_A1);
	mhv_setOutput(MHV_ARDUINO_PIN_A2);
	mhv_setOutput(MHV_ARDUINO_PIN_A3);

	// Enable global interrupts
	sei();

	uint8_t frameBuffer[2 * 1 * 32 * 8 / 8];
	MHV_Display_Holtek_HT1632 display(MHV_ARDUINO_PIN_A0, MHV_ARDUINO_PIN_A1,
			MHV_HT1632_PMOS_32x8, 2, 1, &displaySelect, frameBuffer, &txBuffer);

	while (1) {
		lines(&display);
		textAnimation(&display);
		fader(&display);
		manualTextAnimation(&display);
		slowFill(&display);
	}

// Main must return an int, even though we never get here
	return 0;
}
