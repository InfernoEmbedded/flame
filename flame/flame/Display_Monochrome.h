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

#ifndef FLAME_DISPLAY_MONOCHROME_H_
#define FLAME_DISPLAY_MONOCHROME_H_

#include <inttypes.h>
#include <avr/pgmspace.h>
#include <flame/Device_TX.h>
#include <flame/io.h>
#include <flame/Font.h>

namespace flame {

class Display_Selector {
public:
	virtual void select(uint8_t displayX, uint8_t displayY, bool active) =0;
};

/**
 * A monochrome bitmap display
 * Origin (0,0) is bottom left
 * @tparam	cols		the number of columns
 * @tparam	rows		the number of rows
 * @tparam	txBuffers	the number of output buffers
 */
template<uint16_t cols, uint16_t rows, uint8_t txBuffers>
class Display_Monochrome : public Device_TXImplementation<txBuffers> {
protected:
	int16_t			_txOffset;

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
	bool writeChar(const FONT &font, int16_t *offsetX, int16_t offsetY,
			uint8_t onValue, uint8_t offValue, char character) {
		uint8_t c = (uint8_t)character;
		if (c < font.firstChar || c >= font.firstChar + font.charCount) {
			c = font.unknown;
		}

	// Normalize character to font
		c -= font.firstChar;
#ifdef __FLASH
		uint8_t charWidth = font.widths[c];

		const uint8_t *fontChar = font.fontData + font.offsets[c];
#else
		uint8_t charWidth = pgm_read_byte(font.widths + c);

		const uint8_t *fontChar = font.fontData + pgm_read_word(font.offsets + c);
#endif

	// Render each column
		bool ret = false;
		do {
			int8_t bit = 7;
			uint8_t y = 0;
#ifdef __FLASH
			uint8_t data = *(fontChar++);
#else
			uint8_t data = pgm_read_byte(fontChar++);
#endif

			if (*offsetX >= 0) {
	// Start at the bottom of the column and work up
				for (; y < font.maxHeight; y++, bit--) {
					ret = true;

					if (data & (1 << bit)) {
						setPixel(*offsetX, offsetY + y, onValue);
					} else {
						setPixel(*offsetX, offsetY + y, offValue);
					}

	// Support multiple-byte font images - fetch the next byte of data if we exhaust the current one
					if (-1 == bit && (font.maxHeight - y) > 0) {
						bit = 7;
#ifdef __FLASH
						data = *(fontChar++);
#else
						data = pgm_read_byte(fontChar++);
#endif
					}
				}
			}
		} while (++(*offsetX) < (int16_t)cols && --charWidth);

		*offsetX += charWidth;

		return ret;
	}

	/**
	 * Write a character seperator (a single column of off pixels) to the display
	 * @param	font		the font to use
	 * @param	offsetX		the horizontal pixel offset to start writing at (left side of char) will increment to the next position on return)
	 * @param	offsetY		the vertical pixel offset to start writing at (bottom of char)
	 * @param	offValue	the pixel value to use for off
	 * @return true if the seperator was written
	 */
	bool writeSeperator(const FONT &font, int16_t *offsetX, int16_t offsetY,
			uint8_t offValue) {
		bool ret = false;

		if (*offsetX >= 0 && *offsetX < (int16_t)cols) {
	// Start at the bottom of the column and work up
			uint8_t y = 0;
			uint8_t row;

			while (y++ < font.maxHeight) {
				row = offsetY + y;
				if (row < rows) {
					setPixel(*offsetX, row, offValue);
					ret = true;
				}
			}
		}

		(*offsetX)++;
		return ret;
	}

	/**
	 * Start rendering TX buffers
	 */
	void runTxBuffers() {
		_txOffset = cols - 1;
		Device_TX::moreTX();
	}

public:
	/**
	 * Get the width of the display
	 */
	uint16_t getWidth() CONST {
		return cols;
	}

	/**
	 * Get the width of the display
	 */
	uint16_t getHeight() CONST {
		return rows;
	}

	/**
	 * Clear the display to a particular value
	 * @param	value	the value to fill the display with
	 */
	void clear(uint8_t value) {
		uint16_t x, y;

		for (x = 0; x < cols; ++x) {
			for (y = 0; y < rows; ++y) {
				setPixel(x, y, value);
			}
		}
	}

	/**
	 * Write a TX Buffer to the display
	 * @param	font		the font to use
	 * @param	offsetX		the horizontal pixel offset to start writing at (left side of char) will increment to the next position on return)
	 * @param	offsetY		the vertical pixel offset to start writing at (bottom of char)
	 * @param	onValue		the pixel value to use for on
	 * @param	offValue	the pixel value to use for off
	 * @param	txBuffer	the buffer to write
	 * @return true if anything was written
	 */
	bool writeBuffer(const FONT &font, int16_t *offsetX, int16_t offsetY,
			uint8_t onValue, uint8_t offValue, TXBuffer &txBuffer) {
		bool ret = false;
		while (*offsetX < (int16_t)cols) {
			int c = txBuffer.nextCharacter();
			if (-1 == c) {
				break;
			}

			ret |= writeChar(font, offsetX, offsetY, onValue, offValue, (char) c);

			if (txBuffer.hasMore()) {
				ret |= writeSeperator(font, offsetX, offsetY, offValue);
			}
		}
		txBuffer.seek(0);

		return ret;
	}

	/**
	 * Write a buffer to the display
	 * @param	font		the font to use
	 * @param	offsetX		the horizontal pixel offset to start writing at (left side of char) will increment to the next position on return)
	 * @param	offsetY		the vertical pixel offset to start writing at (bottom of char)
	 * @param	onValue		the pixel value to use for on
	 * @param	offValue	the pixel value to use for off
	 * @param	buffer		the buffer to write
	 * @param	length		the length of the buffer
	 * @return true if anything was written
	 */
	bool writeBuffer(const FONT &font, int16_t *offsetX, int16_t offsetY,
			uint8_t onValue, uint8_t offValue, const char *buffer, uint16_t length) {
		bool ret = false;
		uint16_t i = 0;
		while (i < length && *offsetX < (int16_t)cols) {
			ret |= writeChar(font, offsetX, offsetY, onValue, offValue, buffer[i++]);
			if (i != length) {
				ret |= writeSeperator(font, offsetX, offsetY, offValue);
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
	bool writeString_P(const FONT &font, int16_t *offsetX, int16_t offsetY,
			uint8_t onValue, uint8_t offValue, PGM_P string) {
		const char *p = string;
		char val;

		bool ret = false;
		val = pgm_read_byte(p);
		while (val != '\0' && *offsetX < (int16_t)cols) {
			ret |= writeChar(font, offsetX, offsetY, onValue, offValue, val);
			val = pgm_read_byte(++p);
			if (val != '\0') {
				ret |= writeSeperator(font, offsetX, offsetY, offValue);
			}
		}

		return ret;
	}

	/**
	 * Write a PROGMEM buffer to the display
	 * @param	font		the font to use
	 * @param	offsetX		the horizontal pixel offset to start writing at (left side of char) will increment to the next position on return)
	 * @param	offsetY		the vertical pixel offset to start writing at (bottom of char)
	 * @param	onValue		the pixel value to use for on
	 * @param	offValue	the pixel value to use for off
	 * @param	buffer		the buffer to write
	 * @param	length		the length of the buffer
	 * @return true if anything was written
	 */
	bool writeBuffer_P(const FONT &font, int16_t *offsetX, int16_t offsetY,
			uint8_t onValue, uint8_t offValue, PGM_P buffer, uint16_t length) {

		bool ret = false;
		uint16_t i = 0;
		while (i < length && *offsetX < (int16_t)cols) {
			ret |= writeChar(font, offsetX, offsetY, onValue, offValue, pgm_read_byte(buffer + i++));
			if (i < length) {
				ret |= writeSeperator(font, offsetX, offsetY, offValue);
			}
		}

		return ret;
	}

#if 0 && defined __FLASH
	/**
	 * Write a flash string to the display
	 * @param	font		the font to use
	 * @param	offsetX		the horizontal pixel offset to start writing at (left side of char) will increment to the next position on return)
	 * @param	offsetY		the vertical pixel offset to start writing at (bottom of char)
	 * @param	onValue		the pixel value to use for on
	 * @param	offValue	the pixel value to use for off
	 * @param	string		the string to write
	 * @return true if anything was written
	 */
	bool writeString(const FONT &font, int16_t *offsetX, int16_t offsetY,
			uint8_t onValue, uint8_t offValue, const __flash char *string) {

		bool ret = false;
		while (*string != '\0' && *offsetX < (int16_t)cols) {
			ret |= writeChar(font, offsetX, offsetY, onValue, offValue, val);
			if (*(++p) != '\0') {
				ret |= writeSeperator(font, offsetX, offsetY, offValue);
			}
		}

		return ret;
	}

	/**
	 * Write a flash buffer to the display
	 * @param	font		the font to use
	 * @param	offsetX		the horizontal pixel offset to start writing at (left side of char) will increment to the next position on return)
	 * @param	offsetY		the vertical pixel offset to start writing at (bottom of char)
	 * @param	onValue		the pixel value to use for on
	 * @param	offValue	the pixel value to use for off
	 * @param	buffer		the buffer to write
	 * @param	length		the length of the buffer
	 * @return true if anything was written
	 */
	bool writeBuffer(const FONT &font, int16_t *offsetX, int16_t offsetY,
			uint8_t onValue, uint8_t offValue, const __flash char *buffer, uint16_t length) {

		bool ret = false;
		uint16_t i = 0;
		while (i < length && *offsetX < (int16_t)cols) {
			ret |= writeChar(font, offsetX, offsetY, onValue, offValue, buffer[i++]);
			if (i < length) {
				ret |= writeSeperator(font, offsetX, offsetY, offValue);
			}
		}

		return ret;
	}
#endif

#if 0 && defined __MEMX
	/**
	 * Write a memx string to the display
	 * @param	font		the font to use
	 * @param	offsetX		the horizontal pixel offset to start writing at (left side of char) will increment to the next position on return)
	 * @param	offsetY		the vertical pixel offset to start writing at (bottom of char)
	 * @param	onValue		the pixel value to use for on
	 * @param	offValue	the pixel value to use for off
	 * @param	string		the string to write
	 * @return true if anything was written
	 */
	bool writeString(const FONT &font, int16_t *offsetX, int16_t offsetY,
			uint8_t onValue, uint8_t offValue, const __memx char *string) {

		bool ret = false;
		while (*string != '\0' && *offsetX < (int16_t)cols) {
			ret |= writeChar(font, offsetX, offsetY, onValue, offValue, val);
			if (*(++p) != '\0') {
				ret |= writeSeperator(font, offsetX, offsetY, offValue);
			}
		}

		return ret;
	}

	/**
	 * Write a memx buffer to the display
	 * @param	font		the font to use
	 * @param	offsetX		the horizontal pixel offset to start writing at (left side of char) will increment to the next position on return)
	 * @param	offsetY		the vertical pixel offset to start writing at (bottom of char)
	 * @param	onValue		the pixel value to use for on
	 * @param	offValue	the pixel value to use for off
	 * @param	buffer		the buffer to write
	 * @param	length		the length of the buffer
	 * @return true if anything was written
	 */
	bool writeBuffer(const FONT &font, int16_t *offsetX, int16_t offsetY,
			uint8_t onValue, uint8_t offValue, const __memx char *buffer, uint16_t length) {

		bool ret = false;
		uint16_t i = 0;
		while (i < length && *offsetX < (int16_t)cols) {
			ret |= writeChar(font, offsetX, offsetY, onValue, offValue, buffer[i++]);
			if (i < length) {
				ret |= writeSeperator(font, offsetX, offsetY, offValue);
			}
		}

		return ret;
	}
#endif


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
	bool writeString(const FONT &font, int16_t *offsetX, int16_t offsetY,
			uint8_t onValue, uint8_t offValue, const char *string) {
		const char *p = string;

		bool ret = false;
		while (*p != '\0' && *offsetX < (int16_t)cols) {
			ret |= writeChar(font, offsetX, offsetY, onValue, offValue, *p);
			if (*(++p) != '\0') {
				ret |= writeSeperator(font, offsetX, offsetY, offValue);
			}
		}

		return ret;
	}



	/**
	 * Render a frame of TX buffer animation - scrolls text from right to left, before moving to the next buffer
	 * @param	font		the font to use
	 * @param	offsetY		the vertical pixel offset to start writing at (bottom of char)
	 * @param	onValue		the pixel value for on pixels
	 * @param	offValue	the pixel value for off pixels
	 * @return true if there are more frames to be rendered
	 */
	bool txAnimation(const FONT &font, int16_t offsetY, uint8_t onValue, uint8_t offValue) {
		int16_t offsetX = _txOffset--;

		clear(offValue);

		bool ret = writeBuffer(font, &offsetX, offsetY, onValue, offValue, Device_TX::_currentTx);

		if (!ret) {
			_txOffset = cols - 1;
			return Device_TX::moreTX();
		}

		return true;
	}

	virtual void setPixel(uint16_t row, uint16_t col, uint8_t value)=0;
	virtual uint8_t getPixel(uint16_t row, uint16_t col)=0;
};

}
#endif /* FLAME_DISPLAY_MONOCHROME_H_ */
