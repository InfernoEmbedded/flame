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
#include <MHV_Display_HD44780.h>

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
 * A class for operating HD44780 based LCD displays (and compatible)
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
MHV_Display_HD44780::MHV_Display_HD44780(volatile uint8_t *dataDir, volatile uint8_t *dataOut, volatile uint8_t *dataIn, uint8_t dataPin, int8_t dataPinchangeInterrupt,
		volatile uint8_t *controlDir, volatile uint8_t *controlOut, volatile uint8_t *controlIn, uint8_t controlPin, int8_t controlPinchangeInterrupt,
		volatile uint8_t *visualDir, volatile uint8_t *visualOut, volatile uint8_t *visualIn, uint8_t visualPin, int8_t visualPinchangeInterrupt,
		uint8_t colCount, uint16_t rowCount, MHV_RingBuffer *txBuffers) :
		MHV_Display_Character(colCount, rowCount, txBuffers) {
	_dataDir = dataDir;
	_dataOut = dataOut;
	_dataIn = dataIn;
	_dataPin = dataPin;
	_dataMask = 0;
	_controlOut = controlOut;
	_controlPin = controlPin;
	_visualOut = visualOut;
	_visualPin = visualPin;

	_ticks = 0;
	_animateTicks = 64;

	uint8_t mask;
// Set pins as output
	_dataMask |= 0x0f << _dataPin;
	*_dataDir |= _dataMask;
	*_dataOut &= ~(_dataMask);

	mask = 0x07 << _controlPin;
	*controlDir |= mask;
	*_controlOut &= ~mask;

	mask |= 0x03 << _visualPin;
	*visualDir |= mask;
	*_visualOut &= ~(mask);

	_mustDelay = false;
}

/**
 * Write a nibble to the display
 * @param 	nibble	the data to write (lower 4 bits)
 * @param	rs		true to set the RS pin
 */
void MHV_Display_HD44780::writeNibble(uint8_t nibble, bool rs) {
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
 * Read a nibble from the display
 * @param	rs		true to set the RS pin
 */
uint8_t MHV_Display_HD44780::readNibble(bool rs) {
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
 * Write a character to the display at the current location, incrementing the location by 1
 * @param	character	the character to write
 */
void MHV_Display_HD44780::_writeChar(char character) {
	while (isBusy()) {};

	// Write high order nibble
	writeNibble(character >> 4, true);
	// Write low order nibble
	writeNibble(character & 0x0f, true);
}

/**
 * Read a character from the display at the current location, incrementing the location by 1
 * @return	the character at the current location
 */
char MHV_Display_HD44780::_readChar() {
	while (isBusy()) {};
	// Read high order nibble
	char character = readNibble(true) << 4;
	// Read low order nibble
	character |= readNibble(true);

	return character;
}

/**
 * Move the cursor to a location, so the next writeChar will write a character at that location
 * (Origin is at the bottom left)
 * @param	col		the column to put the character
 * @param	row		the row to put the character
 */
void MHV_Display_HD44780::_setCursor(uint16_t col, uint16_t row) {
	_setCursor((uint8_t)col, (uint8_t)row);
}


static const uint8_t mhv_hd44780AddressTable[] PROGMEM = {
		64+20,	20,	64,	0
};

/**
 * Move the cursor to a location, so the next writeChar will write a character at that location
 * (Origin is at the bottom left)
 * @param	col		the column to put the character
 * @param	row		the row to put the character
 */
void MHV_Display_HD44780::_setCursor(uint8_t col, uint8_t row) {
	uint8_t address;

	if (4 == _rowCount && 20 == _colCount) {
		address = pgm_read_byte(mhv_hd44780AddressTable + row);
	} else {
// Display type unsupported - assume 2 line display
		address = pgm_read_byte(mhv_hd44780AddressTable + row + 2);
	}

	address += col;

	while (isBusy()) {};

	addressDDRAM(address);
}

/**
 * Send a command to the display
 */
void MHV_Display_HD44780::writeCommand(MHV_HD44780_COMMAND command, uint8_t data) {
	uint8_t byte = command | data;

	char buf[9];
	buf[8] = '\0';
	for (uint8_t i = 0; i < 8; ++i) {
		buf[i] = ((byte >> (7 - i)) & 0x01) ? '1' : '0';
	}

	if (_mustDelay) {
		_delay_us(19);
		_mustDelay = false;
	}

	// Write high order nibble
	writeNibble(byte >> 4, false);
	// Write low order nibble
	writeNibble(byte & 0x0f, false);
}

/**
 * Clear the display
 */
void MHV_Display_HD44780::clear() {
	while (isBusy()) {};

	writeCommand(MHV_44780_CMD_CLEAR, 0);
	_currentRow = _rowCount - 1;
	_currentCol = 0;
}

/**
 * Set the entry mode - allows for left or right printing, allows for scrolling display or moving cursor
 * @param	left2right	true for text reading left to right
 * @param	scroll		true to scroll text rather than moving the cursor
 */
void MHV_Display_HD44780::entryMode(bool left2Right, bool scroll) {
	uint8_t data = left2Right << 1 | scroll;

	while (isBusy()) {};

	writeCommand(MHV_44780_CMD_SET_ENTRY_MODE, data);
}

/**
 * Set parameters on the display
 * @param	displayOn	turn the display on
 * @param	cursorOn	turn the curson on
 * @param	cursorBlink	blink the cursor
 */
void MHV_Display_HD44780::control(bool displayOn, bool cursorOn, bool cursorBlink) {
	uint8_t data = displayOn << 2 | cursorOn << 1 | cursorBlink;

	while (isBusy()) {};

	writeCommand(MHV_44780_CMD_SET_DISPLAY_MODE, data);
}

/**
 * Initialise the display
 * @param	multiLine	true if there is more than 1 line
 * @param	bigFont		true to use 5x11 fonts, false for 5x8
 */
void MHV_Display_HD44780::function(bool multiLine, bool bigFont) {
	uint8_t data = multiLine << 3 | bigFont << 2;

// Set 4 bit transfer
	writeNibble(0b0010, false);
	_delay_ms(4.1);
//	writeNibble(0b0010, false);
//	_delay_ms(1);
//	writeNibble(0b0010, false);
//	_delay_ms(1);

	writeCommand(MHV_44780_CMD_SET_FUNCTION, data);
}

/**
 * Set the CGRAM address
 * @param	address	the CGRAM address
 */
void MHV_Display_HD44780::addressCGRAM(uint8_t address) {
	while (isBusy()) {};

	writeCommand(MHV_44780_CMD_SET_CG_ADDR, address);
}

/**
 * Set the DDRAM address
 * @param	address	the DDRAM address
 */
void MHV_Display_HD44780::addressDDRAM(uint8_t address) {
	while (isBusy()) {};

	writeCommand(MHV_44780_CMD_SET_DD_ADDR, address);
}

/**
 * Check if the display is busy
 * @return true if the display is busy
 */
bool MHV_Display_HD44780::isBusy() {
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
void MHV_Display_HD44780::setBacklight(uint8_t value) {
	if (value > 15) {
		value = 16;
	}
	_brightness = value;
}

/**
 * Set the contrast
 * @param	value	the contrast of the display(0..15)
 */
void MHV_Display_HD44780::setContrast(uint8_t value) {
	if (value > 15) {
		value = 16;
	}
	value = 15 - value;

	_contrast = value;
}

/**
 * Initialise the display
 * @param	multiLine	true if there is more than 1 line
 * @param	bigFont		true to use 5x11 fonts, false for 5x8
 * @param	cursorOn	turn the curson on
 * @param	cursorBlink	blink the cursor
 * @param	left2right	true for text reading left to right
 * @param	scroll		true to scroll text rather than moving the cursor
 */
void MHV_Display_HD44780::init(bool multiLine, bool bigFont, bool cursorOn, bool cursorBlink,
		bool left2right, bool scroll) {
/* Assume the worst case, we are called immediately after poweron
 * Also assume that the MCU init takes 0 time
 */
	_delay_ms(HD44780_TINIT);

	function(multiLine, bigFont);
	_delay_us(39);
	control(true, cursorOn, cursorBlink);
	clear();
	entryMode(left2right, scroll);
	setBacklight(5);
	setContrast(9);
	_currentRow = _rowCount - 1;
	_currentCol = 0;
}

/**
 * Tick the display for animation & PWM - this should be called every millisecond
 */
void MHV_Display_HD44780::tick1ms() {
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
