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

#define HD44780_TINIT	300		// ms

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
MHV_Display_HD44780::MHV_Display_HD44780(uint8_t colCount, uint16_t rowCount, MHV_RingBuffer *txBuffers) :
		MHV_Display_Character(colCount, rowCount, txBuffers) {
	_ticks = 0;
	_animateTicks = 64;
	_mustDelay = false;
}

/**
 * Write a character to the display at the current location, incrementing the location by 1
 * @param	character	the character to write
 */
void MHV_Display_HD44780::_writeChar(char character) {
	while (isBusy()) {};
	writeByte(character, true);
	delay(MHV_44780_WRITE_CHAR);
}

/**
 * Read a character from the display at the current location, incrementing the location by 1
 * @return	the character at the current location
 */
char MHV_Display_HD44780::_readChar() {
	while (isBusy()) {};
	return readByte(true);
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

	if (_mustDelay) {
		_delay_us(19);
		_mustDelay = false;
	}

	writeByte(byte, false);

	delay(command);
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
 * @param	cursorOn	turn the cursor on
 * @param	cursorBlink	blink the cursor
 */
void MHV_Display_HD44780::control(bool displayOn, bool cursorOn, bool cursorBlink) {
	uint8_t data = displayOn << 2 | cursorOn << 1 | cursorBlink;

	while (isBusy()) {};

	writeCommand(MHV_44780_CMD_SET_DISPLAY_MODE, data);
}

/**
 * Initialise the display
 * @param	byteMode	true to use 8 bit protocol
 * @param	multiLine	true if there is more than 1 line
 * @param	bigFont		true to use 5x11 fonts, false for 5x8
 */
void MHV_Display_HD44780::function(bool byteMode, bool multiLine, bool bigFont) {
	uint8_t data = byteMode << 4 | multiLine << 3 | bigFont << 2;

	if (!byteMode) {
		// Set 4 bit transfer
		writeByte(0b00100010, false);
		_delay_ms(4.1);
	}

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
 * Initialise the display
 * @param	bits8		true to use 8 bit transfers
 * @param	multiLine	true if there is more than 1 line
 * @param	bigFont		true to use 5x11 fonts, false for 5x8
 * @param	cursorOn	turn the curson on
 * @param	cursorBlink	blink the cursor
 * @param	left2right	true for text reading left to right
 * @param	scroll		true to scroll text rather than moving the cursor
 */
void MHV_Display_HD44780::init(bool byteMode, bool multiLine, bool bigFont, bool cursorOn, bool cursorBlink,
		bool left2right, bool scroll) {
/* Assume the worst case, we are called immediately after poweron
 * Also assume that the MCU init takes 0 time
 */
	_delay_ms(HD44780_TINIT);

// hardware initialization always set 8 bits mode
	_byteMode = true;
	uint8_t resetData = 1 << 4 | multiLine << 3 | bigFont << 2 | 1;
	writeCommand(MHV_44780_CMD_SET_FUNCTION, resetData);
	writeCommand(MHV_44780_CMD_SET_FUNCTION, resetData);
	writeCommand(MHV_44780_CMD_SET_FUNCTION, resetData);

	_byteMode = byteMode;


	function(byteMode, multiLine, bigFont);
	_delay_us(39);
	control(true, cursorOn, cursorBlink);
	clear();
	entryMode(left2right, scroll);
	_currentRow = _rowCount - 1;
	_currentCol = 0;
}
