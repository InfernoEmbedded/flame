/*
 * Copyright (c) 2012, Make, Hack, Void Inc
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
#ifndef MHV_DISPLAY_CHARACTER_H_
#define MHV_DISPLAY_CHARACTER_H_

#include <mhvlib/Device_TX.h>

namespace mhvlib {

/**
 * A generic text display
 * Origin (0,0) is bottom left
 * @tparam	cols		the number of columns
 * @tparam	rows		the number of rows
 * @tparam	txBuffers	the number of output buffers
 */
template<uint8_t cols, uint8_t rows, uint8_t txBuffers>
class Display_Character : public Device_TXImplementation<txBuffers> {
protected:
	int16_t			_txOffset;
	bool			_wrap;
	bool			_scroll;
	uint16_t		_currentRow;
	uint16_t		_currentCol;

public:
	/**
	 * Create a new character display
	 */
	Display_Character() :
		_wrap(true),
		_scroll(true),
		_currentRow(0),
		_currentCol(0) {}


	/**
	 * Get the width of the display
	 */
	uint8_t getWidth() CONST {
		return cols;
	}

	/**
	 * Get the width of the display
	 */
	uint8_t getHeight() CONST {
		return rows;
	}

	/**
	 * Write a TX buffer to the display
	 * @param	offsetX		the horizontal offset to start writing at
	 * @param	offsetY		the vertical offset to start writing at
	 * @param	txBuffer	the buffer to write
	 * @return true if anything was written
	 */
	bool write(int16_t *offsetX, uint16_t offsetY, TXBuffer &txBuffer) {
		uint16_t currentPosition = txBuffer.getPosition();

		if (*offsetX < 0) {
			txBuffer.seek (0 - *offsetX);
			*offsetX = 0;
		} else {
			txBuffer.seek(0);
		}

		setCursor((uint16_t)*offsetX, offsetY);

		bool ret = false;
		if (offsetY < rows) {
			int c;
			while (-1 != (c = txBuffer.nextCharacter()) && (*offsetX)++ < (int16_t)cols) {
				ret = true;
				writeChar((char)c);
			}
		}

		txBuffer.seek(currentPosition);
		return ret;
	}

	/**
	 * Write a string to the display
	 * @param	offsetX		the horizontal offset to start writing at
	 * @param	offsetY		the vertical offset to start writing at
	 * @param	string		the string to write
	 * @return true if anything was written
	 */
	bool writeString(int16_t *offsetX, uint16_t offsetY, const char *string) {
		const char *p = string;

		while (*offsetX < 0) {
			(*offsetX)++;
			p++;
			if ('\0' == *p) {
				return false;
			}
		}

		setCursor((uint16_t)*offsetX, offsetY);

		bool ret = false;
		if (offsetY < rows) {
			while (*p != '\0' && (*offsetX)++ < (int16_t)cols) {
				ret = true;
				writeChar(*p++);
			}
		}

		return ret;
	}

	/**
	 * Write a buffer to the display
	 * @param	offsetX		the horizontal offset to start writing at
	 * @param	offsetY		the vertical offset to start writing at
	 * @param	buffer		the buffer to write
	 * @param	length		the length of the buffer
	 * @return true if anything was written
	 */
	bool writeBuffer(int16_t *offsetX, uint16_t offsetY,
			const char *buffer, uint16_t length) {
		bool ret = false;
		uint16_t i = 0;

		if (*offsetX < 0) {
			i -= *offsetX;
			*offsetX = 0;
		}

		setCursor((uint16_t)*offsetX, offsetY);

		while (i < length && (*offsetX)++ < (int16_t)cols) {
			ret = true;
			writeChar(buffer[i++]);
		}

		return ret;
	}


	/**
	 * Write a PROGMEM string to the display
	 * @param	offsetX		the horizontal offset to start writing at
	 * @param	offsetY		the vertical offset to start writing at
	 * @param	string		the string to write
	 * @return true if anything was written
	 */
	bool writeString_P(int16_t *offsetX, uint16_t offsetY, PGM_P string) {
		const char *p = string;
		char c;

		while (*offsetX < 0) {
			(*offsetX)++;
			p++;
			if ('\0' == pgm_read_byte(p)) {
				return false;
			}
		}
		setCursor((uint16_t)*offsetX, offsetY);

		bool ret = false;
		if (offsetY < rows) {
			while ('\0' != (c = pgm_read_byte(p++)) && (*offsetX)++ < (int16_t)cols) {
				ret = true;
				writeChar(c);
			}
		}

		return ret;
	}

	/**
	 * Write a PROGMEM buffer to the display
	 * @param	offsetX		the horizontal offset to start writing at
	 * @param	offsetY		the vertical offset to start writing at
	 * @param	buffer		the buffer to write
	 * @param	length		the length of the buffer
	 * @return true if anything was written
	 */
	bool writeBuffer_P(int16_t *offsetX, uint16_t offsetY,
			PGM_P buffer, uint16_t length) {
		bool ret = false;
		uint16_t i = 0;

		while (*offsetX < 0) {
			(*offsetX)++;
			i++;
		}
		setCursor((uint16_t)*offsetX, offsetY);

		while (i < length && (*offsetX)++ < (int16_t)cols) {
			ret = true;
			writeChar(pgm_read_byte(buffer + i++));
		}

		return ret;
	}

#ifdef UNSUPPORTED
#ifdef __FLASH
	/**
	 * Write a flash string to the display
	 * @param	offsetX		the horizontal offset to start writing at
	 * @param	offsetY		the vertical offset to start writing at
	 * @param	string		the string to write
	 * @return true if anything was written
	 */
	bool writeString(int16_t *offsetX, uint16_t offsetY, const __flash char *string) {
		char c;

		while (*offsetX < 0) {
			(*offsetX)++;
			p++;
			if ('\0' == *p) {
				return false;
			}
		}
		setCursor((uint16_t)*offsetX, offsetY);

		bool ret = false;
		if (offsetY < rows) {
			while ('\0' != (c = *p++) && (*offsetX)++ < (int16_t)cols) {
				ret = true;
				writeChar(c);
			}
		}

		return ret;
	}

	/**
	 * Write a PROGMEM buffer to the display
	 * @param	offsetX		the horizontal offset to start writing at
	 * @param	offsetY		the vertical offset to start writing at
	 * @param	buffer		the buffer to write
	 * @param	length		the length of the buffer
	 * @return true if anything was written
	 */
	bool writeBuffer(int16_t *offsetX, uint16_t offsetY, const __flash char *buffer, uint16_t length) {
		bool ret = false;
		uint16_t i = 0;

		while (*offsetX < 0) {
			(*offsetX)++;
			i++;
		}
		setCursor((uint16_t)*offsetX, offsetY);

		while (i < length && (*offsetX)++ < (int16_t)cols) {
			ret = true;
			writeChar(buffer[i++]);
		}

		return ret;
	}
#endif

#ifdef __MEMX
	/**
	 * Write a flash string to the display
	 * @param	offsetX		the horizontal offset to start writing at
	 * @param	offsetY		the vertical offset to start writing at
	 * @param	string		the string to write
	 * @return true if anything was written
	 */
	bool writeString(int16_t *offsetX, uint16_t offsetY, const __memx char *string) {
		char c;

		while (*offsetX < 0) {
			(*offsetX)++;
			p++;
			if ('\0' == *p) {
				return false;
			}
		}
		setCursor((uint16_t)*offsetX, offsetY);

		bool ret = false;
		if (offsetY < rows) {
			while ('\0' != (c = *p++) && (*offsetX)++ < (int16_t)cols) {
				ret = true;
				writeChar(c);
			}
		}

		return ret;
	}

	/**
	 * Write a PROGMEM buffer to the display
	 * @param	offsetX		the horizontal offset to start writing at
	 * @param	offsetY		the vertical offset to start writing at
	 * @param	buffer		the buffer to write
	 * @param	length		the length of the buffer
	 * @return true if anything was written
	 */
	bool writeBuffer(int16_t *offsetX, uint16_t offsetY, const __memx char *buffer, uint16_t length) {
		bool ret = false;
		uint16_t i = 0;

		while (*offsetX < 0) {
			(*offsetX)++;
			i++;
		}
		setCursor((uint16_t)*offsetX, offsetY);

		while (i < length && (*offsetX)++ < (int16_t)cols) {
			ret = true;
			writeChar(buffer[i++]);
		}

		return ret;
	}
#endif
#endif

	/**
	 * Start rendering TX buffers
	 */
	void runTxBuffers() {
		_txOffset = cols - 1;
		Device_TX::moreTX();
	}

	/**
	 * Render a frame of TX buffer animation - scrolls text from right to left, before moving to the next buffer
	 * @param	row		the vertical pixel offset to start writing at
	 * @return true if there are more frames to be rendered
	 */
	bool txAnimation(uint16_t row) {
		if (!Device_TX::_currentTx.hasMore()) {
			return false;
		}

		int16_t offsetX = _txOffset--;

		setCursor(0, row);

		bool ret;
		ret = write(&offsetX, row, Device_TX::_currentTx);

		while (offsetX++ < (int16_t)cols) {
			writeChar(' ');
		}

		if (!ret) {
			_txOffset = cols - 1;
			return Device_TX::moreTX();
		}

		return true;
	}

	/**
	 * Should the display automatically wrap characters?
	 * @param	shouldWrap	true to wrap
	 */
	void setWrap(bool shouldWrap) {
		_wrap = shouldWrap;
	}

	/**
	 * Should the automatically scroll characters vertically?
	 * @param	shouldScroll	true to scroll
	 */
	void setScroll(bool shouldScroll) {
		_scroll = shouldScroll;
	}

	/**
	 * Position the cursor
	 * @param	col		the column to set
	 * @param	row		the row to set
	 */
	void setCursor(uint16_t col, uint16_t row) {
		_currentRow = row;
		_currentCol = col;
		_setCursor(col, row);
	}

	/**
	 * Write a character to the display
	 * Will interpret the following special characters:
	 * 	\\b	Backspace
	 * 	\\t	Tab
	 * 	\\n	Newline
	 *
	 * @param	character	the character to write
	 */
	void writeChar(char character) {
		switch (character) {
		case '\b':
			if (_wrap && 0 == _currentCol) {
	// Go to the previous line
				if (_currentRow + 1 < rows) {
					_currentRow++;
				}
				setCursor(cols - 1, _currentRow);
			} else {
				setCursor(_currentCol - 1, _currentRow);
			}
			break;
		case '\t':
			_currentCol = (_currentCol + 4) & ~(4 - 1);
			if (_currentCol >= cols) {
				if (_wrap) {
					if (_currentRow > 0) {
						_currentRow--;
					} else if (_scroll) {
						scrollVertically();
						break;
					}
					_currentCol = 0;
				} else {
					_currentCol = cols - 1;
				}
			}
			setCursor(_currentCol, _currentRow);
			break;
		case '\n':
			if (_currentRow > 0) {
				_currentRow--;
			} else if (_scroll) {
				scrollVertically();
			}
			_currentCol = 0;
			setCursor(_currentCol, _currentRow);
			break;
		default:
			if (_currentCol++ >= cols - 1 && _wrap) {
				_currentCol = 0;
				if (_currentRow > 0) {
					_currentRow--;
				} else if (_scroll) {
					scrollVertically();
				} else {
					setCursor(_currentCol, _currentRow);
				}
			}
			_writeChar(character);
			break;
		}
	}

	/**
	 * Scroll the display up, leaving a blank line at the bottom
	 */
	void scrollVertically() {
		uint16_t x, y;
		char	c;

		for (y = rows - 2; y > 0; y--) {
	// Copy each character in this row to the row above
			for (x = 0; x < cols; ++x) {
				_setCursor(x, y + 1);
				c = _readChar();
				_setCursor(x, y);
				_writeChar(c);
			}
		}

	// Blank the last row
		_setCursor(0, 0);
		for (x = 0; x < cols; ++x) {
			_writeChar(' ');
		}

	// Set the cursor ready to write another line
		setCursor(0, 0);
	}



// Subclasses must implement these
	virtual void _writeChar(char character)=0;
	virtual char _readChar()=0;
	virtual void _setCursor(uint8_t col, uint8_t row)=0;
	virtual void clear()=0;
};

}

#endif /* MHV_DISPLAY_CHARACTER_H_ */
