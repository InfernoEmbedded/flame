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
#ifndef DISPLAY_HD44780_SHIFT_REGISTER_H_
#define DISPLAY_HD44780_SHIFT_REGISTER_H_

#include <flame/Display_HD44780.h>
#define FLAME_SHIFT_ORDER_MSB
#define FLAME_SHIFT_WRITECLOCK NULL,_clockOut,NULL,_clockPin,-1
#define FLAME_SHIFT_WRITEDATA NULL,_dataOut,NULL,_dataPin,-1
#include <flame/Shifter.h>
#include <util/delay.h>

#define HD44780_TSU1	(60 * 1000000 / F_CPU / 3 + 1)
#define HD44780_TW		(450 * 1000000 / F_CPU / 3 + 1)

namespace flame{

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
	FLAME_DECLARE_PIN(clock), FLAME_DECLARE_PIN(data), FLAME_DECLARE_PIN(enable)>
class Display_HD44780_Shift_Register : public Display_HD44780<cols, rows, txBuffers> {
private:
	ShifterImplementation<FLAME_PIN_PARMS(clock), FLAME_PIN_PARMS(data)> _shifter;

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
			pinOn(FLAME_PIN_PARMS(enable));
			_delay_loop_1(HD44780_TW);
			pinOff(FLAME_PIN_PARMS(enable));
			_delay_loop_1(HD44780_TW);
		}

	/**
	 * Write a byte to the display
	 * @param 	byte	the data to write
	 * @param	rs		true to set the RS pin (aka data pin )
	 */
	void writeByte(uint8_t byte, bool rs) {
		if (Display_HD44780<cols, rows, txBuffers>::_byteMode) {
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
	void delay(HD44780Command command) {
		switch (command) {
		case HD44780Command::CLEAR:
		case HD44780Command::HOME:
			_delay_ms(2);
			break;
		case HD44780Command::SET_ENTRY_MODE:
		case HD44780Command::SET_DISPLAY_MODE:
		case HD44780Command::SET_CURSOR_MODE:
		case HD44780Command::SET_CG_ADDR:
		case HD44780Command::SET_DD_ADDR:
		case HD44780Command::WRITE_CHAR:
		case HD44780Command::SET_FUNCTION:
			_delay_us(39);
			break;
		}
	}

public:
	/**
	 * A class for operating HD44780 based LCD displays via a shift register such as a 74HC164
	 */
	Display_HD44780_Shift_Register() {
		setOutput(FLAME_PIN_PARMS(data));
		setOutput(FLAME_PIN_PARMS(enable));
		setOutput(FLAME_PIN_PARMS(clock));
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
		Display_HD44780<cols, rows, txBuffers>::init(
				false, multiLine, bigFont, cursorOn, cursorBlink, left2right, scroll);
	}
};

}
#endif /* FLAME_DISPLAY_HD44780_SHIFT_REGISTER_H_ */
