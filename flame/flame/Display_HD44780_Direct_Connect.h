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
#ifndef FLAME_DISPLAY_HD44780_DIRECT_CONNECT_H_
#define FLAME_DISPLAY_HD44780_DIRECT_CONNECT_H_

#include <flame/Display_HD44780.h>
#include <flame/Timer.h>
#include <util/delay.h>

#define HD44780_DB4			(1 << dataPin)
#define HD44780_DB5			(1 << (dataPin + 1))
#define HD44780_DB6			(1 << (dataPin + 2))
#define HD44780_DB7			(1 << (dataPin + 3))
#define HD44780_RS			(1 << controlPin)
#define HD44780_RW			(1 << (controlPin + 1))
#define HD44780_E			(1 << (controlPin + 2))
#define HD44780_CONTRAST	(1 << visualPin)
#define HD44780_LED			(1 << (visualPin + 1))

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

namespace flame {

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
 * @tparam	cols		the number of columns
 * @tparam	rows		the number of rows
 * @tparam	txBuffers	the number of output buffers
 * @tparam	data...		pin declaration for the first bit of the data port DB4..DB7 (will use a nibble starting at this bit)
 * @tparam	control...	pin declaration for the first bit of the control port (will use 3 bits)
 * @tparam	visual...	pin declaration for the first bit of the visual port (will use 2 bits)
 */
template<uint16_t cols, uint16_t rows, uint8_t txBuffers, FLAME_DECLARE_PIN(data),
FLAME_DECLARE_PIN(control), FLAME_DECLARE_PIN(visual)>
class Display_HD44780_Direct_Connect : public Display_HD44780<cols, rows, txBuffers>,
	public TimerListener {
protected:
	uint8_t			_brightness;
	uint8_t			_contrast;
	uint8_t			_dataMask;
	uint8_t			_ticks;

	/**
	 * Write a byte to the display
	 * @param 	byte	the data to write
	 * @param	rs		true to set the RS pin
	 */
	void writeByte(uint8_t byte, bool rs) {
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
	void writeNibble(uint8_t nibble, bool rs) {
		uint8_t oldPort = _SFR_IO8(dataOut) & ~(_dataMask);
		_SFR_IO8(controlOut) &= ~(HD44780_RW);
		if (rs) {
			_SFR_IO8(controlOut) |= HD44780_RS;
		}

		_SFR_IO8(dataOut) = oldPort | ((nibble << dataPin) & _dataMask);

		_delay_loop_1(HD44780_TSU1);
		_SFR_IO8(controlOut) |= HD44780_E;
		_delay_loop_1(HD44780_TW);
		_SFR_IO8(controlOut) &= ~HD44780_E & ~HD44780_RS;
		_delay_loop_1(HD44780_TW);
	}


	/**
	 * Read a byte from the display
	 * @param	rs		true to set the RS pin
	 */
	uint8_t readByte(bool rs) {
		char character = readNibble(rs) << 4;
		// Read low order nibble
		character |= readNibble(rs);

		return character;
	}

	/**
	 * Read a nibble from the display
	 * @param	rs		true to set the RS pin
	 */
	uint8_t readNibble(bool rs) {
		_SFR_IO8(controlOut) |= HD44780_RW;
		if (rs) {
			_SFR_IO8(controlOut) |= HD44780_RS;
		}

		_SFR_IO8(dataDir) &= ~_dataMask;
		_delay_loop_1(HD44780_TSU1);
		_SFR_IO8(controlOut) |= HD44780_E;
		_delay_loop_1(HD44780_TW);
		uint8_t data = _SFR_IO8(dataIn);
		_SFR_IO8(controlOut) &= ~HD44780_E & ~HD44780_RS;
		_SFR_IO8(dataDir) |= _dataMask;
		_delay_loop_1(HD44780_TW);

		return data >> dataPin;
	}

	/**
	 * Check if the display is busy
	 * @return true if the display is busy
	 */
	bool isBusy() {
		_SFR_IO8(dataDir) &= ~(HD44780_DB7);
		_SFR_IO8(dataOut) &= ~(HD44780_DB7); // turn off pullup
		_SFR_IO8(controlOut) |= HD44780_RW;
		_delay_loop_1(HD44780_TSU1);
		_SFR_IO8(controlOut) |= HD44780_E;
		_delay_loop_1(HD44780_TW);

		bool busy = _SFR_IO8(dataIn) & HD44780_DB7;
		_SFR_IO8(controlOut) &= ~(HD44780_E | HD44780_RW);
		_SFR_IO8(dataDir) |= HD44780_DB7;

		Display_HD44780<cols, rows, txBuffers>::_mustDelay = true;

		return busy;
	}

	/**
	 * Delay function
	 * No delays required as we can check whether the display is busy
	 */
	void CONST delay(UNUSED HD44780Command command) {
		return;
	}

public:
	/**
	 * Create a new driver for a directly connected HD44780 display
	 */
	Display_HD44780_Direct_Connect() {
		// Set pins as output
			_dataMask = 0x0f << dataPin;
			_SFR_IO8(dataDir) |= _dataMask;
			_SFR_IO8(dataOut) &= ~_dataMask;

			uint8_t mask = 0x07 << controlPin;
			_SFR_IO8(controlDir) |= mask;
			_SFR_IO8(controlOut) &= ~mask;

			mask = 0x03 << visualPin;
			_SFR_IO8(visualDir) |= mask;
			_SFR_IO8(visualOut) &= ~(mask);

			_brightness = 5;
			_contrast = 6;
	}

	/**
	 * Manipulate the backlight
	 * @param	value	the brightness of the backlight (0..15)
	 */
	void setBacklight(uint8_t value) {
		if (value > 15) {
			value = 15;
		}
		_brightness = value;
	}

	/**
	 * Set the contrast
	 * @param	value	the contrast of the display(0..15)
	 */
	void setContrast(uint8_t value) {
		if (value > 15) {
			value = 15;
		}
		value = 15 - value;

		_contrast = value;
	}

	/**
	 * Tick the display for PWM - this should be called every 500 microseconds
	 */
	void alarm(UNUSED AlarmSource source) {
		if (0 == _ticks) {
			_SFR_IO8(visualOut) |= HD44780_LED;
			_SFR_IO8(visualOut) |= HD44780_CONTRAST;
		}

		if (_brightness == _ticks) {
			_SFR_IO8(visualOut) &= ~HD44780_LED;
		}

		if (_contrast == _ticks) {
			_SFR_IO8(visualOut) &= ~HD44780_CONTRAST;
		}

		if (++_ticks == 16) {
			_ticks = 0;
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
	void init(bool multiLine, bool bigFont, bool cursorOn, bool cursorBlink,
					bool left2right, bool scroll) {
		Display_HD44780<cols, rows, txBuffers>::init(
				false, multiLine, bigFont, cursorOn, cursorBlink, left2right, scroll);
	}

};
}

#endif /* FLAME_DISPLAY_HD44780_DIRECT_CONNECT_H_ */
