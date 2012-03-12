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

#include <stdio.h>
#include <MHV_Display_Holtek_HT1632.h>

/**
 *  Initialise the library
 * @param	shifter			The shifter to output to
 * @param	mode			What mode the displays should be run in
 * @param	arrayX			the width of the array in number of displays
 * @param	arrayY			the height of the array in number of displays
 * @param	frameBuffer		memory for a framebuffer, must be at least arrayX * arrayY * displayX * displayY / 8 bytes long
 * @param	txBuffers		A ringbuffer used for text printing
 */
MHV_Display_Holtek_HT1632::MHV_Display_Holtek_HT1632(
		MHV_Shifter &shifter,
		MHV_Display_Selector &selector,
		MHV_HT1632_MODE mode,
		uint8_t arrayX, uint8_t arrayY,
		uint8_t *frameBuffer, MHV_RingBuffer &txBuffers) :
		MHV_Display_Monochrome(
				((MHV_HT1632_NMOS_32x8 == mode || MHV_HT1632_PMOS_32x8 == mode) ? 32 : 24) * arrayX,
				((MHV_HT1632_NMOS_32x8 == mode || MHV_HT1632_PMOS_32x8 == mode) ? 8 : 16) * arrayY,
				txBuffers),
			_shifter(shifter),
			_selector(selector),
			_mode(mode),
			_arrayX(arrayX),
			_arrayY(arrayY),
			_frameBuffer(frameBuffer) {

	uint8_t x, y;

	for (y = 0; y < _arrayY; y++) {
		// Set the first module to be the master, rest will be slaves
		for (x = 0; x < _arrayX; x++) {
			_selector.select(x, y, 0);

			poweron(x, y);
			if (!x && !y) {
				rcMaster(x, y);
			} else {
				slave(x, y);
			}

			// Set the display mode
			setMode(x, y, mode);
		}
	}

	// Clear the framebuffer
	uint16_t bufSize = arrayX * arrayY;
	switch (mode) {
		case MHV_HT1632_NMOS_32x8:
		case MHV_HT1632_PMOS_32x8:
			_displayX = 32;
			_displayY = 8;
			_displayBytes = 32 * 8 / 8;
			break;
		case MHV_HT1632_NMOS_24x16:
		case MHV_HT1632_PMOS_24x16:
			_displayX = 24;
			_displayY = 16;
			_displayBytes = 24 * 16 / 8;
			break;
	}
	bufSize *= _displayBytes;

	for (uint16_t i = 0; i < bufSize; i++) {
		_frameBuffer[i] = 0x0;
	}
}

/**
 * Set a pixel
 * @param	col		the column of the pixel
 * @param	row		the row of the pixel
 * @param	value	the value of the pixel
 */
void MHV_Display_Holtek_HT1632::setPixel(uint16_t col, uint16_t row, uint8_t value) {
	if (row < _rowCount && col < _colCount) {

		// Coordinates of the display module
		uint8_t moduleX, moduleY;

		moduleX = col / _displayX;
		moduleY = row / _displayY;

		// offsets within the module
		col -= moduleX * _displayX;
		row -= moduleY * _displayY;

		uint8_t offset = col;
		uint8_t bit = row;
		switch (_mode) {
		case MHV_HT1632_NMOS_32x8:
		case MHV_HT1632_PMOS_32x8:
			break;
		case MHV_HT1632_NMOS_24x16:
		case MHV_HT1632_PMOS_24x16:
			offset *= 2;
			if (bit > 7) {
				bit -= 8;
				offset++;
			}
			break;
		}

		// Change offset from the start of the display to the start of the framebuffer
		offset += (moduleY * _arrayX + moduleX) * _displayBytes;

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
uint8_t PURE MHV_Display_Holtek_HT1632::getPixel(uint16_t col, uint16_t row) {
	if (row < _rowCount && col < _colCount) {
		// Coordinates of the display module
		uint8_t moduleX, moduleY;

		moduleX = row / _displayX;
		moduleY = col / _displayY;

		// offsets within the module
		row -= moduleX * _displayX;
		col -= moduleY * _displayY;

		uint8_t offset = col;
		uint8_t bit = row;
		switch (_mode) {
		case MHV_HT1632_NMOS_32x8:
		case MHV_HT1632_PMOS_32x8:
			break;
		case MHV_HT1632_NMOS_24x16:
		case MHV_HT1632_PMOS_24x16:
			offset *= 2;
			if (bit > 7) {
				bit -= 8;
				offset++;
			}
			break;
		}

		// Change offset from the start of the display to the start of the framebuffer
		offset += (moduleY * _arrayX + moduleX) * _displayBytes;

		if (_frameBuffer[offset] & (1 << bit)) {
			return 1;
		}
	}

	return 0;
}

/**
 *  Send a command to the display module
 * @param moduleX	the module to write to
 * @param moduleY	the module to write to
 * @param command	the command
 */
void MHV_Display_Holtek_HT1632::sendCommand(uint8_t moduleX, uint8_t moduleY, MHV_HT1632_COMMAND command) {
	_selector.select(moduleX, moduleY, 1);
	_shifter.shiftOut(command, 3);
}

/**
 * Mark a command as complete
 * @param moduleX	the module to complete the command
 * @param moduleY	the module to complete the command
 */
void MHV_Display_Holtek_HT1632::commandComplete(uint8_t moduleX, uint8_t moduleY) {
	_selector.select(moduleX, moduleY, 0);
}

/**
 * Set up the display to stream output
 * @param moduleX	the module to write to
 * @param moduleY	the module to write to
 */
void MHV_Display_Holtek_HT1632::outputStart(uint8_t moduleX, uint8_t moduleY) {
	sendCommand(moduleX, moduleY, MHV_HT1632_COMMAND_WRITE);
	_shifter.shiftOut((uint8_t)0b0, (uint8_t)7);
}

/**
 * Flush the framebuffer to the displays
 */
void MHV_Display_Holtek_HT1632::flush() {
	uint8_t x, y;
	uint8_t bytes;
	uint8_t *data;

	bytes = (MHV_HT1632_NMOS_32x8 == _mode || MHV_HT1632_PMOS_32x8 == _mode) ? 32 : 48;

	for (x = 0; x < _arrayX; x++) {
		for (y = 0; y < _arrayY; y++) {
			data = _frameBuffer + (uint16_t)bytes * (y * _arrayY + x);
		}
	}

	for (x = 0; x < _arrayX; x++) {
		for (y = 0; y < _arrayY; y++) {
			data = _frameBuffer + (uint16_t)bytes * (y * _arrayY + x);
			outputStart(x, y);
			_shifter.shiftOut(data, bytes);
			commandComplete(x, y);
		}
	}
}

/**
 * Set a module to be the master
 * @param module the module
 */
void MHV_Display_Holtek_HT1632::master(uint8_t moduleX, uint8_t moduleY) {
	sendCommand(moduleX, moduleY, MHV_HT1632_COMMAND_CMD);
	_shifter.shiftOut(0b00010100);
	_shifter.shiftOut((uint8_t)0b0, (uint8_t)1);
	commandComplete(moduleX, moduleY);
}

/**
 * Set a module to be an RC master (HT1632C chips)
 * @param module the module
 */
void MHV_Display_Holtek_HT1632::rcMaster(uint8_t moduleX, uint8_t moduleY) {
	sendCommand(moduleX, moduleY, MHV_HT1632_COMMAND_CMD);
	_shifter.shiftOut(0b00011000);
	_shifter.shiftOut((uint8_t)0b0, (uint8_t)1);
	commandComplete(moduleX, moduleY);
}

/**
 * Set a module to be a slave
 * @param module the module
 */
void MHV_Display_Holtek_HT1632::slave(uint8_t moduleX, uint8_t moduleY) {
	sendCommand(moduleX, moduleY, MHV_HT1632_COMMAND_CMD);
	_shifter.shiftOut(0b00010000);
	_shifter.shiftOut((uint8_t)0b0, (uint8_t)1);
	commandComplete(moduleX, moduleY);
}

/**
 * Set the brightness of a module
 * @param module 		the module
 * @param brightness	the brightness (from 0 to 15)
 */
void MHV_Display_Holtek_HT1632::brightness(uint8_t moduleX, uint8_t moduleY, uint8_t brightness) {
	sendCommand(moduleX, moduleY, MHV_HT1632_COMMAND_CMD);
	_shifter.shiftOut(0b1010, 4);
	_shifter.shiftOut(brightness << 1, 5);
	commandComplete(moduleX, moduleY);
}

/**
 * Set the brightness of all modules
 * @param brightness_in		the brightness (from 0 to 15)
 */
void MHV_Display_Holtek_HT1632::brightness(uint8_t brightness_in) {
	uint8_t x, y;

	for (y = 0; y < _arrayY; y++) {
		for (x = 0; x < _arrayX; x++) {
			brightness(x, y, brightness_in);
		}
	}
}

/**
 *  Put a module to sleep
 * @param moduleX	the module to sleep
 * @param moduleY	the module to sleep
 */
void MHV_Display_Holtek_HT1632::poweroff(uint8_t moduleX, uint8_t moduleY) {
	// Shut down the oscillator & PWM generator
	sendCommand(moduleX, moduleY, MHV_HT1632_COMMAND_CMD);
	_shifter.shiftOut(0);
	_shifter.shiftOut((uint8_t)0b0, 1);
	commandComplete(moduleX, moduleY);
}

/**
 * Wake a module up
 * @param moduleX	the module to wake up
 * @param moduleY	the module to wake up
 */
void MHV_Display_Holtek_HT1632::poweron(uint8_t moduleX, uint8_t moduleY) {
// Turn on the oscillator
	sendCommand(moduleX, moduleY, MHV_HT1632_COMMAND_CMD);
	_shifter.shiftOut(0b00000001);
	_shifter.shiftOut((uint8_t)0b0, 1);
	commandComplete(moduleX, moduleY);

// Turn on the PWM generator
	sendCommand(moduleX, moduleY, MHV_HT1632_COMMAND_CMD);
	_shifter.shiftOut(0b00000011);
	_shifter.shiftOut((uint8_t)0b0, 1);
	commandComplete(moduleX, moduleY);
}

/**
 * Set the mode of a display
 * @param moduleX	the module
 * @param moduleY	the module
 * @param mode		the mode
 */
void MHV_Display_Holtek_HT1632::setMode(uint8_t moduleX, uint8_t moduleY, MHV_HT1632_MODE mode) {
	sendCommand(moduleX, moduleY, MHV_HT1632_COMMAND_CMD);
	_shifter.shiftOut(0b0010, 4);
	_shifter.shiftOut(mode, 2);
	_shifter.shiftOut((uint8_t)0b0, 3);
	commandComplete(moduleX, moduleY);
}


/**
 *  Put all modules to sleep
 */
void MHV_Display_Holtek_HT1632::poweroff() {
	uint8_t x, y;

	for (y = 0; y < _arrayY; y++) {
		for (x = 0; x < _arrayX; x++) {
			poweroff(x, y);
		}
	}
}

/**
 *  Wake all modules up
 */
void MHV_Display_Holtek_HT1632::poweron() {
	uint8_t x, y;

	for (y = 0; y < _arrayY; y++) {
		for (x = 0; x < _arrayX; x++) {
			poweron(x, y);
		}
	}
}
