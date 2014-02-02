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
#ifndef FLAME_DISPLAY_HD44780_H_
#define FLAME_DISPLAY_HD44780_H_

#include <flame/Display_Character.h>
#define HD44780_TINIT	300		// ms


namespace flame{

#ifdef __FLASH
static const __flash uint8_t FLAME_hd44780AddressTable[] = {
#else
static const uint8_t FLAME_hd44780AddressTable[] PROGMEM = {
#endif
		64+20,	20,	64,	0
};


enum class HD44780Command : uint8_t {
	CLEAR					= 0x001,
	HOME					= 0x002,
	SET_ENTRY_MODE			= 0x004,
	SET_DISPLAY_MODE		= 0x008,
	SET_CURSOR_MODE			= 0x010,
	SET_FUNCTION			= 0x020,
	SET_CG_ADDR				= 0x040,
	SET_DD_ADDR				= 0x080,
	WRITE_CHAR				= 0xff /* not a real command, only here to support delay */
};

INLINE uint8_t operator| (HD44780Command command, uint8_t oredWith) {
	const uint8_t myCommand = static_cast<uint8_t>(command);
	return(myCommand | oredWith);
}

/**
 * A class for operating HD44780 based LCD displays (and compatible)
 * @tparam	cols		the number of columns
 * @tparam	rows		the number of rows
 * @tparam	txBuffers	the number of output buffers
 */
template<uint16_t cols, uint16_t rows, uint8_t txBuffers>
class Display_HD44780 : public Display_Character<cols, rows, txBuffers> {
protected:
	uint16_t			_ticks;
	uint16_t			_animateTicks;
	bool 				_mustDelay;
	bool				_byteMode;

	/**
	 * Send a command to the display
	 */
	void writeCommand(HD44780Command command, uint8_t data) {
		uint8_t byte = command | data;

		if (_mustDelay) {
			_delay_us(19);
			_mustDelay = false;
		}

		writeByte(byte, false);

		delay(command);
	}

	/**
	 * Initialise the display
	 * @param	byteMode	true to use 8 bit protocol
	 * @param	multiLine	true if there is more than 1 line
	 * @param	bigFont		true to use 5x11 fonts, false for 5x8
	 */
	void function(bool byteMode, bool multiLine, bool bigFont) {
		uint8_t data = byteMode << 4 | multiLine << 3 | bigFont << 2;

		if (!byteMode) {
			// Set 4 bit transfer
			writeByte(0b00100010, false);
			_delay_ms(4.1);
		}

		writeCommand(HD44780Command::SET_FUNCTION, data);
	}

	/**
	 * Set the CGRAM address
	 * @param	address	the CGRAM address
	 */
	void addressCGRAM(uint8_t address) {
		while (isBusy()) {};

		writeCommand(HD44780Command::SET_CG_ADDR, address);
	}

	/**
	 * Set the DDRAM address
	 * @param	address	the DDRAM address
	 */
	void addressDDRAM(uint8_t address) {
		while (isBusy()) {};

		writeCommand(HD44780Command::SET_DD_ADDR, address);
	}

	virtual void writeByte(uint8_t byte, bool rs)=0;
	virtual uint8_t readByte(bool rs)=0;

	/**
	 * Move the cursor to a location, so the next writeChar will write a character at that location
	 * (Origin is at the bottom left)
	 * @param	col		the column to put the character
	 * @param	row		the row to put the character
	 */
	void _setCursor(uint8_t col, uint8_t row) {
		uint8_t address;

#ifdef __FLASH
		if (4 == rows && 20 == cols) {
			address = FLAME_hd44780AddressTable [row];
		} else {
	// Display type unsupported - assume 2 line display
			address = FLAME_hd44780AddressTable [row + 2];
		}
#else
		if (4 == rows && 20 == cols) {
			address = pgm_read_byte(FLAME_hd44780AddressTable + row);
		} else {
	// Display type unsupported - assume 2 line display
			address = pgm_read_byte(FLAME_hd44780AddressTable + row + 2);
		}
#endif
		address += col;

		while (isBusy()) {};

		addressDDRAM(address);
	}

	/**
	 * Write a character to the display at the current location, incrementing the location by 1
	 * @param	character	the character to write
	 */
	void _writeChar(char character) {
		while (isBusy()) {};
		writeByte(character, true);
		delay(HD44780Command::WRITE_CHAR);
	}

	/**
	 * Read a character from the display at the current location, incrementing the location by 1
	 * @return	the character at the current location
	 */
	char _readChar() {
		while (isBusy()) {};
		return readByte(true);
	}

	virtual bool isBusy()=0;
	virtual void delay(HD44780Command command)=0;

public:
	/**
	 * Create a new display
	 */
	Display_HD44780() {
		_ticks = 0;
		_animateTicks = 64;
		_mustDelay = false;
	}

	/**
	 * Initialise the display
	 * @param	byteMode	true to use 8 bit transfers
	 * @param	multiLine	true if there is more than 1 line
	 * @param	bigFont		true to use 5x11 fonts, false for 5x8
	 * @param	cursorOn	turn the curson on
	 * @param	cursorBlink	blink the cursor
	 * @param	left2right	true for text reading left to right
	 * @param	scroll		true to scroll text rather than moving the cursor
	 */
	void init(bool byteMode, bool multiLine, bool bigFont, bool cursorOn, bool cursorBlink,
			bool left2right, bool scroll) {
	/* Assume the worst case, we are called immediately after poweron
	 * Also assume that the MCU init takes 0 time
	 */
		_delay_ms(HD44780_TINIT);

	// hardware initialization always set 8 bits mode
		_byteMode = true;
		uint8_t resetData = 1 << 4 | multiLine << 3 | bigFont << 2 | 1;
		writeCommand(HD44780Command::SET_FUNCTION, resetData);
		writeCommand(HD44780Command::SET_FUNCTION, resetData);
		writeCommand(HD44780Command::SET_FUNCTION, resetData);

		_byteMode = byteMode;


		function(byteMode, multiLine, bigFont);
		_delay_us(39);
		control(true, cursorOn, cursorBlink);
		clear();
		entryMode(left2right, scroll);
		Display_Character<cols, rows, txBuffers>::_currentRow = rows - 1;
		Display_Character<cols, rows, txBuffers>::_currentCol = 0;
	}

	/**
	 * Clear the display
	 */
	void clear() {
		while (isBusy()) {};

		writeCommand(HD44780Command::CLEAR, 0);
		Display_Character<cols, rows, txBuffers>::_currentRow = rows - 1;
		Display_Character<cols, rows, txBuffers>::_currentCol = 0;
	}

	/**
	 * Set the entry mode - allows for left or right printing, allows for scrolling display or moving cursor
	 * @param	left2Right	true for text reading left to right
	 * @param	scroll		true to scroll text rather than moving the cursor
	 */
	void entryMode(bool left2Right, bool scroll) {
		uint8_t data = left2Right << 1 | scroll;

		while (isBusy()) {};

		writeCommand(HD44780Command::SET_ENTRY_MODE, data);
	}

	/**
	 * Set parameters on the display
	 * @param	displayOn	turn the display on
	 * @param	cursorOn	turn the cursor on
	 * @param	cursorBlink	blink the cursor
	 */
	void control(bool displayOn, bool cursorOn, bool cursorBlink) {
		uint8_t data = displayOn << 2 | cursorOn << 1 | cursorBlink;

		while (isBusy()) {};

		writeCommand(HD44780Command::SET_DISPLAY_MODE, data);
	}
};

}
#endif /* FLAME_DISPLAY_HD44780_H_ */
