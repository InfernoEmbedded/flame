/* Copyright (c) 2010, Make, Hack, Void Inc
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

#include "MHV_Display_Monochrome.h"
#include <string.h>
#include <math.h>

#include <MHV_HardwareSerial.h>
extern MHV_HardwareSerial serial;


/**
 * A monochrome bitmap display
 * Origin (0,0) is bottom left
 */

/**
 * Create a new monochrome display
 * @param	colCount	the number of columns
 * @param	rowCount	the number of rows
 */
MHV_Display_Monochrome::MHV_Display_Monochrome(uint16_t colCount, uint16_t rowCount) {
	_colCount = colCount;
	_rowCount = rowCount;
}

/**
 * Get the width of the display
 */
uint16_t MHV_Display_Monochrome::getWidth() {
	return _colCount;
}

/**
 * Get the width of the display
 */
uint16_t MHV_Display_Monochrome::getHeight() {
	return _rowCount;
}

/**
 * Clear the display to a particular value
 * @param	value	the value to fill the display with
 */
void MHV_Display_Monochrome::clear(uint8_t value) {
	uint16_t x, y;

	for (x = 0; x < _colCount; ++x) {
		for (y = 0; y < _rowCount; ++y) {
			setPixel(x, y, value);
		}
	}
}

/**
 * Write a character to the display
 * @param	font		the font to use
 * @param	offsetX		the horizontal pixel offset to start writing at (left side of char) will increment to the next position on return)
 * @param	offsetY		the vertical pixel offset to start writing at (bottom of char)
 * @param	onValue		the pixel value to use for on
 * @param	offValue	the pixel value to use for off
 * @param	character	the character to write
 * @return	true if a character was written
 */
bool MHV_Display_Monochrome::writeChar(const MHV_FONT *font, int16_t *offsetX, int16_t offsetY,
		uint8_t onValue, uint8_t offValue, char character) {
	uint8_t c = (uint8_t)character;
	if (c < font->firstChar || c >= font->firstChar + font->charCount) {
		c = font->unknown;
	}

// Normalize character to font
	c -= font->firstChar;
	uint8_t charWidth = pgm_read_byte(font->widths + c);

	const uint8_t *fontChar = font->fontData + pgm_read_word(font->offsets + c);

// Render each column
	bool ret = false;
	do {
		int8_t bit = 7;
		uint8_t y = 0;
		uint8_t data = pgm_read_byte(fontChar++);

		if (*offsetX >= 0) {
// Start at the bottom of the column and work up
			for (; y < font->maxHeight; y++, bit--) {
				ret = true;

				if (data & (1 << bit)) {
					setPixel(*offsetX, offsetY + y, onValue);
				} else {
					setPixel(*offsetX, offsetY + y, offValue);
				}

// Support multiple-byte font images - fetch the next byte of data if we exhaust the current one
				if (-1 == bit && (font->maxHeight - y) > 0) {
					bit = 7;
					data = pgm_read_byte(fontChar++);
				}
			}
		}
	} while (++(*offsetX) < (int16_t)_colCount && --charWidth);

	*offsetX += charWidth;

	return ret;
}

/**
 * Write a character seperator (a single column of off pixels) to the display
 * @param	font		the font to use
 * @param	offsetX		the horizontal pixel offset to start writing at (left side of char) will increment to the next position on return)
 * @param	offsetY		the vertical pixel offset to start writing at (bottom of char)
 * @param	onValue		the pixel value to use for on
 * @param	offValue	the pixel value to use for off
 * @return true if the seperator was written
 */
bool MHV_Display_Monochrome::writeSeperator(const MHV_FONT *font, int16_t *offsetX, int16_t offsetY,
		uint8_t onValue, uint8_t offValue) {
	bool ret = false;

	if (*offsetX >= 0 && *offsetX < (int16_t)_colCount) {
// Start at the bottom of the column and work up
		uint8_t y = 0;
		uint8_t row;

		while (y++ < font->maxHeight) {
			row = offsetY + y;
			if (row < _rowCount) {
				setPixel(*offsetX, row, offValue);
				ret = true;
			}
		}
	}

	(*offsetX)++;
	return ret;
}

/**
 * Write a string to the display
 * @param	font		the font to use
 * @param	offsetX		the horizontal pixel offset to start writing at (left side of char) will increment to the next position on return)
 * @param	offsetY		the vertical pixel offset to start writing at (bottom of char)
 * @param	onValue		the pixel value to use for on
 * @param	offValue	the pixel value to use for off
 * @param	string		the string to write
 * @return true if anything was written
 */
bool MHV_Display_Monochrome::writeString(const MHV_FONT *font, int16_t *offsetX, int16_t offsetY,
		uint8_t onValue, uint8_t offValue, const char *string) {
	const char *p = string;

	bool ret = false;
	while (*p != '\0' && *offsetX < (int16_t)_colCount) {
		ret |= writeChar(font, offsetX, offsetY, onValue, offValue, *p);
		if (*(++p) != '\0') {
			ret |= writeSeperator(font, offsetX, offsetY, onValue, offValue);
		}
	}

	return ret;
}

/**
 * Write a PROGMEM string to the display
 * @param	font		the font to use
 * @param	offsetX		the horizontal pixel offset to start writing at (left side of char) will increment to the next position on return)
 * @param	offsetY		the vertical pixel offset to start writing at (bottom of char)
 * @param	onValue		the pixel value to use for on
 * @param	offValue	the pixel value to use for off
 * @param	string		the string to write
 * @return true if anything was written
 */
bool MHV_Display_Monochrome::writeString_P(const MHV_FONT *font, int16_t *offsetX, int16_t offsetY,
		uint8_t onValue, uint8_t offValue, const char *string) {
	const char *p = string;
	char val;

	bool ret = false;
	val = pgm_read_byte(p);
	while (val != '\0' && *offsetX < (int16_t)_colCount) {
		ret |= writeChar(font, offsetX, offsetY, onValue, offValue, val);
		val = pgm_read_byte(++p);
		if (val != '\0') {
			ret |= writeSeperator(font, offsetX, offsetY, onValue, offValue);
		}
	}

	return ret;
}

