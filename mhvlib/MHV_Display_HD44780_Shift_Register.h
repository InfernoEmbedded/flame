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
#ifndef DISPLAY_HD44780_SHIFT_REGISTER_H_
#define DISPLAY_HD44780_SHIFT_REGISTER_H_

#include <MHV_Display_HD44780.h>
#define MHV_SHIFT_ORDER_MSB
#define MHV_SHIFT_WRITECLOCK NULL,_clockOut,NULL,_clockPin,-1
#define MHV_SHIFT_WRITEDATA NULL,_dataOut,NULL,_dataPin,-1
#include <MHV_Shifter.h>
#include <util/delay.h>

#define HD44780_TSU1	(60 * 1000000 / F_CPU / 3 + 1)
#define HD44780_TW		(450 * 1000000 / F_CPU / 3 + 1)


/**
 * A class for operating HD44780 based LCD displays via a shift register such as a 74HC164
 *
 * @tparam	cols		the number of columns
 * @tparam	rows		the number of rows
 * @tparam	txBuffers	the number of output buffers
 * @tparam clock...		the clock pin
 * @tparam data...		the data pin (should be on the same port as clock)
 * @tparam enable...	the enable pin
 */
template<uint16_t cols, uint16_t rows, uint8_t txBuffers,
	MHV_DECLARE_PIN(clock), MHV_DECLARE_PIN(data), MHV_DECLARE_PIN(enable)>
class MHV_Display_HD44780_Shift_Register : public MHV_Display_HD44780<cols, rows, txBuffers> {
private:
	MHV_ShifterImplementation<MHV_PIN_PARMS(clock), MHV_PIN_PARMS(clock)> _shifter;

protected:
	/**
	 * Write 8 data bits to the display
	 * @param 	byte	the data to write (nibble or true byte)
	 * @param	rs		true to set the RS pin
	 */
	void pushBits(uint8_t byte, bool rs) {
		// setup RS
			byte |= rs;

		// Load the byte into the shift register
			_shifter.shiftOut(byte);

		// strobe Enable pin
			_delay_loop_1(HD44780_TSU1);
			mhv_pinOn(MHV_PIN_PARMS(enable));
			_delay_loop_1(HD44780_TW);
			mhv_pinOff(MHV_PIN_PARMS(enable));
			_delay_loop_1(HD44780_TW);
		}

	/**
	 * Write a byte to the display
	 * @param 	byte	the data to write
	 * @param	rs		true to set the RS pin (aka data pin )
	 */
	void writeByte(uint8_t byte, bool rs) {
		if (MHV_Display_HD44780<cols, rows, txBuffers>::_byteMode) {
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
	uint8_t CONST readByte(UNUSED bool rs) {
		return 0;
	}

	/**
	 * Check if the display is busy
	 * @return true if the display is busy
	 */
	bool CONST isBusy() {
		return false;
	}

	/**
	 * Post-command delays
	 */
	void delay(MHV_HD44780_COMMAND command) {
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

public:
	/**
	 * A class for operating HD44780 based LCD displays via a shift register such as a 74HC164
	 */
	MHV_Display_HD44780_Shift_Register() {
		mhv_setOutput(MHV_PIN_PARMS(data));
		mhv_setOutput(MHV_PIN_PARMS(enable));
		mhv_setOutput(MHV_PIN_PARMS(clock));
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
	void init(bool multiLine, bool bigFont, bool cursorOn, bool cursorBlink,
					bool left2right, bool scroll) {
		MHV_Display_HD44780<cols, rows, txBuffers>::init(
				false, multiLine, bigFont, cursorOn, cursorBlink, left2right, scroll);
	}
};

#endif /* MHV_DISPLAY_HD44780_SHIFT_REGISTER_H_ */
