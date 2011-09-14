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

#include <util/delay.h>
#include <MHV_Display_HD44780_Direct_Connect.h>

#define HD44780_DB4			(1 << _dataPin)
#define HD44780_DB5			(1 << (_dataPin + 1))
#define HD44780_DB6			(1 << (_dataPin + 2))
#define HD44780_DB7			(1 << (_dataPin + 3))
#define HD44780_RS			(1 << _controlPin)
#define HD44780_RW			(1 << (_controlPin + 1))
#define HD44780_E			(1 << (_controlPin + 2))
#define HD44780_CONTRAST	(1 << _visualPin)
#define HD44780_LED			(1 << (_visualPin + 1))

/* Delay times in iterations of delay_loop_1 - note that each pass of delay_loop_1 is 3 clocks
 */
#define HD44780_TC		(1000 * 1000000 / F_CPU / 3 + 1)
#define HD44780_TSU1	(60 * 1000000 / F_CPU / 3 + 1)
#define HD44780_TSU2	(195 * 1000000 / F_CPU / 3 + 1)
#define HD44780_TDH		(5 * 1000000 / F_CPU / 3 + 1)
#define HD44780_TW		(450 * 1000000 / F_CPU / 3 + 1)
#define HD44780_TINIT	300		// ms
#define HD44780_TCLEAR	1530	// us
#define HD44780_TINSTR	39		// us
#define HD44780_TRAM	430		// us

/**
 * A class for operating HD44780 based LCD displays (and compatible) in 4 bit mode
 * Data port layout:
 * Bit	description
 * n	DB4
 * n+1	DB5
 * n+2	DB6
 * n+3	DB7
 *
 * Control port layout:
 * n	RS Register Select
 * n+1	R/W Read/Write
 * n+2	E Enable
 *
 * Visual port layout:
 * n	Contrast (V0)
 * n+1	LED Positive
 *
 * @param	data*		pin declaration for the first bit of the data port DB4..DB7 (will use a nibble starting at this bit)
 * @param	control*	pin declaration for the first bit of the control port (will use 3 bits)
 * @param	visual*		pin declaration for the first bit of the visual port (will use 2 bits)
 * @param	dir			the direction register for the port
 * @param	colCount	the number of columns on the display
 * @param	rowCount	the number of rows on the display
 * @param	txBuffers	buffers for async writing
 */
MHV_Display_HD44780_Direct_Connect::MHV_Display_HD44780_Direct_Connect(
		volatile uint8_t *dataDir, volatile uint8_t *dataOut, volatile uint8_t *dataIn, uint8_t dataPin, int8_t dataPinchangeInterrupt,
		volatile uint8_t *controlDir, volatile uint8_t *controlOut, volatile uint8_t *controlIn, uint8_t controlPin, int8_t controlPinchangeInterrupt,
		volatile uint8_t *visualDir, volatile uint8_t *visualOut, volatile uint8_t *visualIn, uint8_t visualPin, int8_t visualPinchangeInterrupt,
		uint8_t colCount, uint16_t rowCount, MHV_RingBuffer *txBuffers) :
		MHV_Display_HD44780(colCount, rowCount, txBuffers) {
	_dataDir = dataDir;
	_dataOut = dataOut;
	_dataIn = dataIn;
	_dataPin = dataPin;
	_controlOut = controlOut;
	_controlPin = controlPin;
	_visualOut = visualOut;
	_visualPin = visualPin;

// Set pins as output
	_dataMask = 0x0f << _dataPin;
	*_dataDir |= _dataMask;
	*_dataOut &= ~(_dataMask);

	uint8_t mask = 0x07 << _controlPin;
	*controlDir |= mask;
	*_controlOut &= ~mask;

	mask = 0x03 << _visualPin;
	*visualDir |= mask;
	*_visualOut &= ~(mask);

	_brightness = 5;
	_contrast = 6;
}

/**
 * An alternate constructor without visual pins - if this constructor is used, tickPWM behaviour is
 * undefined and will like overwrite random bits of memory, so don't call it
 *
 * @param	data*		pin declaration for the first bit of the data port DB4..DB7 (will use a nibble starting at this bit)
 * @param	control*	pin declaration for the first bit of the control port (will use 3 bits)
 * @param	dir			the direction register for the port
 * @param	colCount	the number of columns on the display
 * @param	rowCount	the number of rows on the display
 * @param	txBuffers	buffers for async writing
 */
MHV_Display_HD44780_Direct_Connect::MHV_Display_HD44780_Direct_Connect(
		volatile uint8_t *dataDir, volatile uint8_t *dataOut, volatile uint8_t *dataIn, uint8_t dataPin, int8_t dataPinchangeInterrupt,
		volatile uint8_t *controlDir, volatile uint8_t *controlOut, volatile uint8_t *controlIn, uint8_t controlPin, int8_t controlPinchangeInterrupt,
		uint8_t colCount, uint16_t rowCount, MHV_RingBuffer *txBuffers) :
		MHV_Display_HD44780(colCount, rowCount, txBuffers) {
	_dataDir = dataDir;
	_dataOut = dataOut;
	_dataIn = dataIn;
	_dataPin = dataPin;
	_controlOut = controlOut;
	_controlPin = controlPin;

// Set pins as output
	_dataMask = 0x0f << _dataPin;
	*_dataDir |= _dataMask;
	*_dataOut &= ~(_dataMask);

	uint8_t mask = 0x07 << _controlPin;
	*controlDir |= mask;
	*_controlOut &= ~mask;
}


/**
 * Write a byte to the display
 * @param 	byte	the data to write
 * @param	rs		true to set the RS pin
 */
void MHV_Display_HD44780_Direct_Connect::writeByte(uint8_t byte, bool rs) {
	// Write high order nibble
	writeNibble(byte >> 4, rs);
	// Write low order nibble
	writeNibble(byte & 0x0f, rs);
}

/**
 * Write a nibble to the display
 * @param 	nibble	the data to write (lower 4 bits)
 * @param	rs		true to set the RS pin
 */
void MHV_Display_HD44780_Direct_Connect::writeNibble(uint8_t nibble, bool rs) {
	uint8_t oldPort = *_dataOut & ~(_dataMask);
	*_controlOut &= ~(HD44780_RW);
	if (rs) {
		*_controlOut |= HD44780_RS;
	}

	*_dataOut = oldPort | ((nibble << _dataPin) & _dataMask);

	_delay_loop_1(HD44780_TSU1);
	*_controlOut |= HD44780_E;
	_delay_loop_1(HD44780_TW);
	*_controlOut &= ~HD44780_E & ~HD44780_RS;
	_delay_loop_1(HD44780_TW);
}


/**
 * Read a byte from the display
 * @param	rs		true to set the RS pin
 */
uint8_t MHV_Display_HD44780_Direct_Connect::readByte(bool rs) {
	char character = readNibble(rs) << 4;
	// Read low order nibble
	character |= readNibble(rs);

	return character;
}

/**
 * Read a nibble from the display
 * @param	rs		true to set the RS pin
 */
uint8_t MHV_Display_HD44780_Direct_Connect::readNibble(bool rs) {
	*_controlOut |= HD44780_RW;
	if (rs) {
		*_controlOut |= HD44780_RS;
	}

	*_dataDir &= ~_dataMask;
	_delay_loop_1(HD44780_TSU1);
	*_controlOut |= HD44780_E;
	_delay_loop_1(HD44780_TW);
	uint8_t data = *_dataIn;
	*_controlOut &= ~HD44780_E & ~HD44780_RS;
	*_dataDir |= _dataMask;
	_delay_loop_1(HD44780_TW);

	return data >> _dataPin;
}

/**
 * Check if the display is busy
 * @return true if the display is busy
 */
bool MHV_Display_HD44780_Direct_Connect::isBusy() {
	*_dataDir &= ~(HD44780_DB7);
	*_dataOut &= ~(HD44780_DB7); // turn off pullup
	*_controlOut |= HD44780_RW;
	_delay_loop_1(HD44780_TSU1);
	*_controlOut |= HD44780_E;
	_delay_loop_1(HD44780_TW);

	bool busy = *_dataIn & HD44780_DB7;
	*_controlOut &= ~(HD44780_E | HD44780_RW);
	*_dataDir |= HD44780_DB7;

	_mustDelay = true;

	return busy;
}

/**
 * Manipulate the backlight
 * @param	value	the brightness of the backlight (0..15)
 */
void MHV_Display_HD44780_Direct_Connect::setBacklight(uint8_t value) {
	if (value > 15) {
		value = 15;
	}
	_brightness = value;
}

/**
 * Set the contrast
 * @param	value	the contrast of the display(0..15)
 */
void MHV_Display_HD44780_Direct_Connect::setContrast(uint8_t value) {
	if (value > 15) {
		value = 15;
	}
	value = 15 - value;

	_contrast = value;
}

/**
 * Tick the display for PWM - this should be called every 500 microseconds
 */
void MHV_Display_HD44780_Direct_Connect::tickPWM() {
	if (0 == _ticks % 16) {
		*_visualOut |= HD44780_LED;
		*_visualOut |= HD44780_CONTRAST;
	}

	if (_brightness == _ticks % 16) {
		*_visualOut &= ~HD44780_LED;
	}

	if (_contrast == _ticks % 16) {
		*_visualOut &= ~HD44780_CONTRAST;
	}

	_ticks++;
}

/**
 * Delay function
 * No delays required as we can check whether the display is busy
 */
void MHV_Display_HD44780_Direct_Connect::delay(MHV_HD44780_COMMAND command) {
	return;
}

/**
 * Initialise the display
 * @param	bits8		true to use 8 bit transfers
 * @param	multiLine	true if there is more than 1 line
 * @param	bigFont		true to use 5x11 fonts, false for 5x8
 * @param	cursorOn	turn the curson on
 * @param	cursorBlink	blink the cursor
 * @param	left2right	true for text reading left to right
 * @param	scroll		true to scroll text rather than moving the cursor
 */
void MHV_Display_HD44780_Direct_Connect::init(bool multiLine, bool bigFont, bool cursorOn, bool cursorBlink,
				bool left2right, bool scroll) {
	MHV_Display_HD44780::init(false, multiLine, bigFont, cursorOn, cursorBlink, left2right, scroll);
}

