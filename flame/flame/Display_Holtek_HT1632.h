/* Copyright (c) 2014, Inferno Embedded
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of the Inferno Embedded nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL INFERNO EMBEDDED BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef FLAME_DISPLAY_HOLTEK_HT1632_H_
#define FLAME_DISPLAY_HOLTEK_HT1632_H_

#include <flame/Display_Monochrome.h>
#include <flame/Shifter.h>

#define FLAME_HT1632_BRIGHTNESS_MIN	0
#define FLAME_HT1632_BRIGHTNESS_MED	7
#define FLAME_HT1632_BRIGHTNESS_MAX	15

namespace flame {

enum class HT1632Command : uint8_t {
	READ	=	0b110,
	WRITE	=	0b101,
	CMD		=	0b100
};

enum class HT1632Mode : uint8_t {
	NMOS_32x8	=	0b00,
	NMOS_24x16	=	0b01,
	PMOS_32x8	=	0b10,
	PMOS_24x16	=	0b11
};

/**
 * Create a new HT1632 driver to control an array of displays
 *
 * @param	__flameObjectName			the variable name of the object
 * @param	__flameClockPin			an FLAME_PIN macro for the clock line
 * @param	__flameDataPin			an FLAME_PIN macro for the data line
 * @param	__flameMode				the FLAME_HT1632Mode of the displays
 * @param	__flameDisplayBytes		the number of bytes in a single display
 * @param	__flameArrayX				the number of displays in the X axis
 * @param	__flameArrayY				the number of displays in the Y axis
 * @param	__flameSelector			the selector to choose the active display
 * @param	__flameTxBufferCount		the number of TX buffers
 */
#define FLAME_HOLTEK_HT1632_CREATE(__flameObjectName, __flameClockPin, __flameDataPin, __flameMode, __flameDisplayBytes, __flameArrayX, __flameArrayY, __flameSelector, __flameTxBufferCount) \
	Display_Holtek_HT1632<__flameClockPin, __flameDataPin, __flameMode, __flameArrayX, __flameArrayY, \
			__flameTxBufferCount> __flameObjectName(__flameSelector);

/**
 * Create a new HT1632 driver to control an array of displays
 *
 * @tparam	clock			The clock pin for the display
 * @tparam	data			The data pin for the display, must be on the same port as the clock
 * @tparam	mode			What mode the displays should be run in
 * @tparam	arrayX			the width of the array in number of displays
 * @tparam	arrayY			the height of the array in number of displays
 * @tparam	txBuffers		the number of output buffers
 */
#define MODULE_X ((mode == HT1632Mode::NMOS_32x8 || mode == HT1632Mode::PMOS_32x8) ? 32 : 24)
#define MODULE_Y ((mode == HT1632Mode::NMOS_32x8 || mode == HT1632Mode::PMOS_32x8) ? 8 : 16)
#define DISPLAY_X (arrayX * MODULE_X)
#define DISPLAY_Y (arrayY * MODULE_Y)
template <FLAME_DECLARE_PIN(clock), FLAME_DECLARE_PIN(data), HT1632Mode mode,
		uint8_t arrayX, uint8_t arrayY, uint8_t txBuffers>
class Display_Holtek_HT1632 : public Display_Monochrome<DISPLAY_Y, DISPLAY_X, txBuffers> {
private:
	ShifterImplementation<FLAME_PIN_PARMS(clock), FLAME_PIN_PARMS(data)>
							_shifter;
	Display_Selector	&_selector;
#define DISPLAY_BYTES ((mode == HT1632Mode::NMOS_32x8 || mode == HT1632Mode::PMOS_32x8) ? 32 : 48)
	uint8_t					_frameBuffer[arrayX * arrayY * DISPLAY_BYTES];

	/**
	 *  Send a command to the display module
	 * @param moduleX	the module to write to
	 * @param moduleY	the module to write to
	 * @param command	the command
	 */
	void sendCommand(uint8_t moduleX, uint8_t moduleY, HT1632Command command) {
		_selector.select(moduleX, moduleY, 1);
		_shifter.shiftOut((uint8_t)command, 3);
	}

	/**
	 * Mark a command as complete
	 * @param moduleX	the module to complete the command
	 * @param moduleY	the module to complete the command
	 */
	void commandComplete(uint8_t moduleX, uint8_t moduleY) {
		_selector.select(moduleX, moduleY, 0);
	}

	/**
	 * Set up the display to stream output
	 * @param moduleX	the module to write to
	 * @param moduleY	the module to write to
	 */
	void outputStart(uint8_t moduleX, uint8_t moduleY) {
		sendCommand(moduleX, moduleY, HT1632Command::WRITE);
		_shifter.shiftOut((uint8_t)0b0, (uint8_t)7);
	}

	/**
	 * Set a module to be the master
	 * @param module the module
	 */
	void master(uint8_t moduleX, uint8_t moduleY) {
		sendCommand(moduleX, moduleY, HT1632Command::CMD);
		_shifter.shiftOut(0b00010100);
		_shifter.shiftOut((uint8_t)0b0, (uint8_t)1);
		commandComplete(moduleX, moduleY);
	}

	/**
	 * Set a module to be an RC master (HT1632C chips)
	 * @param module the module
	 */
	void rcMaster(uint8_t moduleX, uint8_t moduleY) {
		sendCommand(moduleX, moduleY, HT1632Command::CMD);
		_shifter.shiftOut(0b00011000);
		_shifter.shiftOut((uint8_t)0b0, (uint8_t)1);
		commandComplete(moduleX, moduleY);
	}

	/**
	 * Set a module to be a slave
	 * @param module the module
	 */
	void slave(uint8_t moduleX, uint8_t moduleY) {
		sendCommand(moduleX, moduleY, HT1632Command::CMD);
		_shifter.shiftOut(0b00010000);
		_shifter.shiftOut((uint8_t)0b0, (uint8_t)1);
		commandComplete(moduleX, moduleY);
	}


	/**
	 * Set the mode of a display
	 * @param moduleX	the module
	 * @param moduleY	the module
	 */
	void setMode(uint8_t moduleX, uint8_t moduleY) {
		sendCommand(moduleX, moduleY, HT1632Command::CMD);
		_shifter.shiftOut(0b0010, 4);
		_shifter.shiftOut((uint8_t)mode, 2);
		_shifter.shiftOut((uint8_t)0b0, 3);
		commandComplete(moduleX, moduleY);
	}


public:
	/**
	 * Initialise the library
	 * @param	selector		a class that sets the enable lines to choose the display to operate
	 */
	Display_Holtek_HT1632(
			Display_Selector &selector) :
				_selector(selector) {
		uint8_t x, y;

		for (y = 0; y < arrayY; y++) {
			// Set the first module to be the master, rest will be slaves
			for (x = 0; x < arrayX; x++) {
				_selector.select(x, y, 0);

				poweron(x, y);
				if (!x && !y) {
					rcMaster(x, y);
				} else {
					slave(x, y);
				}

				// Set the display mode
				setMode(x, y);
			}
		}

		// Clear the framebuffer
		uint16_t bufSize = arrayX * arrayY;
		bufSize *= DISPLAY_BYTES;

		for (uint16_t i = 0; i < bufSize; i++) {
			_frameBuffer[i] = 0x0;
		}
	}

	/**
	 * Set the brightness of a module
	 * @param moduleX 		the X position of the module
	 * @param moduleY 		the Y position of the module
	 * @param brightness	the brightness (from 0 to 15)
	 */
	void brightness(uint8_t moduleX, uint8_t moduleY, uint8_t brightness) {
		sendCommand(moduleX, moduleY, HT1632Command::CMD);
		_shifter.shiftOut(0b1010, 4);
		_shifter.shiftOut(brightness << 1, 5);
		commandComplete(moduleX, moduleY);
	}

	/**
	 * Set the brightness of all modules
	 * @param brightness_in		the brightness (from 0 to 15)
	 */
	void brightness(uint8_t brightness_in) {
		uint8_t x, y;

		for (y = 0; y < arrayY; y++) {
			for (x = 0; x < arrayX; x++) {
				brightness(x, y, brightness_in);
			}
		}
	}

	/**
	 *  Put a module to sleep
	 * @param moduleX	the module to sleep
	 * @param moduleY	the module to sleep
	 */
	void poweroff(uint8_t moduleX, uint8_t moduleY) {
		// Shut down the oscillator & PWM generator
		sendCommand(moduleX, moduleY, HT1632Command::CMD);
		_shifter.shiftOut(0);
		_shifter.shiftOut((uint8_t)0b0, 1);
		commandComplete(moduleX, moduleY);
	}

	/**
	 * Wake a module up
	 * @param moduleX	the module to wake up
	 * @param moduleY	the module to wake up
	 */
	void poweron(uint8_t moduleX, uint8_t moduleY) {
	// Turn on the oscillator
		sendCommand(moduleX, moduleY, HT1632Command::CMD);
		_shifter.shiftOut(0b00000001);
		_shifter.shiftOut((uint8_t)0b0, 1);
		commandComplete(moduleX, moduleY);

	// Turn on the PWM generator
		sendCommand(moduleX, moduleY, HT1632Command::CMD);
		_shifter.shiftOut(0b00000011);
		_shifter.shiftOut((uint8_t)0b0, 1);
		commandComplete(moduleX, moduleY);
	}

	/**
	 *  Put all modules to sleep
	 */
	void poweroff() {
		uint8_t x, y;

		for (y = 0; y < arrayY; y++) {
			for (x = 0; x < arrayX; x++) {
				poweroff(x, y);
			}
		}
	}

	/**
	 *  Wake all modules up
	 */
	void poweron() {
		uint8_t x, y;

		for (y = 0; y < arrayY; y++) {
			for (x = 0; x < arrayX; x++) {
				poweron(x, y);
			}
		}
	}

	/**
	 * Flush the framebuffer to the displays
	 */
	void flush() {
		uint8_t x, y;
		uint8_t bytes;
		uint8_t *data;

		bytes = (HT1632Mode::NMOS_32x8 == mode || HT1632Mode::PMOS_32x8 == mode) ? 32 : 48;

		for (x = 0; x < arrayX; x++) {
			for (y = 0; y < arrayY; y++) {
				data = _frameBuffer + (uint16_t)bytes * (y * arrayY + x);
			}
		}

		for (x = 0; x < arrayX; x++) {
			for (y = 0; y < arrayY; y++) {
				data = _frameBuffer + (uint16_t)bytes * (y * arrayY + x);
				outputStart(x, y);
				_shifter.shiftOut(data, bytes);
				commandComplete(x, y);
			}
		}
	}

// Implements FLAME_Display_Monochrome
	/**
	 * Set a pixel
	 * @param	col		the column of the pixel
	 * @param	row		the row of the pixel
	 * @param	value	the value of the pixel
	 */
	void setPixel(uint16_t col, uint16_t row, uint8_t value) {
		if (row < DISPLAY_Y && col < DISPLAY_X) {
			// Coordinates of the display module
			uint8_t moduleX, moduleY;

			moduleX = col / MODULE_X;
			moduleY = row / MODULE_Y;

			// offsets within the module
			col -= moduleX * MODULE_X;
			row -= moduleY * MODULE_Y;

			uint8_t offset = col;
			uint8_t bit = row;
			switch (mode) {
			case HT1632Mode::NMOS_32x8:
			case HT1632Mode::PMOS_32x8:
				break;
			case HT1632Mode::NMOS_24x16:
			case HT1632Mode::PMOS_24x16:
				offset *= 2;
				if (bit > 7) {
					bit -= 8;
					offset++;
				}
				break;
			}

			// Change offset from the start of the display to the start of the framebuffer
			offset += (moduleY * arrayX + moduleX) * DISPLAY_BYTES;

			if (value) {
				_frameBuffer[offset] |= 1 << bit;
			} else {
				_frameBuffer[offset] &= ~(1 << bit);
			}
		}
	}

	/**
	 * Get a pixel
	 * @param	col		the column of the pixel
	 * @param	row		the row of the pixel
	 * @return the value of the pixel
	 */
	uint8_t getPixel(uint16_t col, uint16_t row) {
		if (row < DISPLAY_Y && col < DISPLAY_X) {
			// Coordinates of the display module
			uint8_t moduleX, moduleY;

			moduleX = row / MODULE_X;
			moduleY = col / MODULE_Y;

			// offsets within the module
			row -= moduleX * MODULE_X;
			col -= moduleY * MODULE_Y;

			uint8_t offset = col;
			uint8_t bit = row;
			switch (mode) {
			case HT1632Mode::NMOS_32x8:
			case HT1632Mode::PMOS_32x8:
				break;
			case HT1632Mode::NMOS_24x16:
			case HT1632Mode::PMOS_24x16:
				offset *= 2;
				if (bit > 7) {
					bit -= 8;
					offset++;
				}
				break;
			}

			// Change offset from the start of the display to the start of the framebuffer
			offset += (moduleY * arrayX + moduleX) * DISPLAY_BYTES;

			if (_frameBuffer[offset] & (1 << bit)) {
				return 1;
			}
		}

		return 0;
	}

};

}
#endif /* FLAME_DISPLAY_HOLTEK_HT1632_H_ */
