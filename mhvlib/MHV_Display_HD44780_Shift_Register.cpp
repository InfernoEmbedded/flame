/* Copyright (c) 2011, Make, Hack, Void Inc
 * All rights reserved.
 *
 * Contributed by Armel
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

#include <MHV_Display_HD44780_Shift_Register.h>

#define MHV_SHIFT_ORDER_MSB
#define MHV_SHIFT_WRITECLOCK NULL,_clockOut,NULL,_clockPin,-1
#define MHV_SHIFT_WRITEDATA NULL,_dataOut,NULL,_dataPin,-1
#include <MHV_Shifter.h>

#define HD44780_TSU1	(60 * 1000000 / F_CPU / 3 + 1)
#define HD44780_TW		(450 * 1000000 / F_CPU / 3 + 1)


/**
 * A class for operating HD44780 based LCD displays via a shift register such as a 74HC164
 *
 * @param	data						An MHV_PIN_* macro for the data line of the shift register
 * @param	enable						An MHV_PIN_* macro for the enable line of the shift register
 * @param	clock						An MHV_PIN_* macro for the clock line of the shift register
 * @param	colCount					the number of columns on the display
 * @param	rowCount					the number of rows on the display
 * @param	txBuffers					buffers for async writing
 */
#pragma GCC diagnostic ignored "-Wunused-parameter"
MHV_Display_HD44780_Shift_Register::MHV_Display_HD44780_Shift_Register(
		MHV_DECLARE_PIN(data), MHV_DECLARE_PIN(enable), MHV_DECLARE_PIN(clock),
		uint8_t colCount, uint16_t rowCount, MHV_RingBuffer &txBuffers) :
	MHV_Display_HD44780(colCount, rowCount, txBuffers) {
	_dataOut = dataOut;
	_dataPin = dataPin;
	mhv_setOutput(dataDir, dataOut, dataIn, dataPin, -1);

	_enableOut = enableOut;
	_enablePin = enablePin;
	mhv_setOutput(enableDir, enableOut, enableIn, enablePin, -1);

	_clockOut = clockOut;
	_clockPin = clockPin;
	mhv_setOutput(clockDir, clockOut, clockIn, clockPin, -1);
}
#pragma GCC diagnostic warning "-Wunused-parameter"


/**
 * Write 8 data bits to the display
 * @param 	byte	the data to write (nibble or true byte)
 * @param	rs		true to set the RS pin
 */
void MHV_Display_HD44780_Shift_Register::pushBits(uint8_t byte, bool rs) {
// setup RS
	byte |= (rs ? 0x01 : 0x00);

// Load the byte into the shift register
	MHV_SHIFTOUT_BYTE(byte);

// strobe Enable pin
	_delay_loop_1(HD44780_TSU1);
	mhv_pinOn(0, _enableOut, 0, _enablePin, -1);
	_delay_loop_1(HD44780_TW);
	mhv_pinOff(0, _enableOut, 0, _enablePin, -1);
	_delay_loop_1(HD44780_TW);
}

/**
 * Write a byte to the display
 * @param 	byte	the data to write
 * @param	rs		true to set the RS pin (aka data pin )
 */
void MHV_Display_HD44780_Shift_Register::writeByte(uint8_t byte, bool rs) {
	if (_byteMode) {
// 8 bits interface mode
		pushBits(byte, rs);
	} else {
// 4 bits interface mode
// Write high order nibble
		pushBits(byte & 0xF0, rs);
// Write low order nibble
		pushBits(byte << 4, rs);
	}
}

/**
 * Read a byte from the display
 * @param	rs		true to set the RS pin
 */
#pragma GCC diagnostic ignored "-Wunused-parameter"
uint8_t CONST MHV_Display_HD44780_Shift_Register::readByte(bool rs) {
	return 0;
}
#pragma GCC diagnostic warning "-Wunused-parameter"


/**
 * Check if the display is busy
 * @return true if the display is busy
 */
bool CONST MHV_Display_HD44780_Shift_Register::isBusy() {
	return false;
}

/**
 * Post-command delays
 */
void MHV_Display_HD44780_Shift_Register::delay(MHV_HD44780_COMMAND command) {
	switch (command) {
	case MHV_44780_CMD_CLEAR:
	case MHV_44780_CMD_RETURN_HOME:
		_delay_ms(2);
		break;
	case MHV_44780_CMD_SET_ENTRY_MODE:
	case MHV_44780_CMD_SET_DISPLAY_MODE:
	case MHV_44780_CMD_SET_CURSOR_MODE:
	case MHV_44780_CMD_SET_CG_ADDR:
	case MHV_44780_CMD_SET_DD_ADDR:
	case MHV_44780_WRITE_CHAR:
	case MHV_44780_CMD_SET_FUNCTION:
		_delay_us(39);
		break;
	}
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
void MHV_Display_HD44780_Shift_Register::init(bool multiLine, bool bigFont, bool cursorOn, bool cursorBlink,
				bool left2right, bool scroll) {
	MHV_Display_HD44780::init(false, multiLine, bigFont, cursorOn, cursorBlink, left2right, scroll);
}

