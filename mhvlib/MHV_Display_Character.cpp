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

#include <MHV_Display_Character.h>

/**
 * A Character charcter display
 * Origin (0,0) is bottom left
 */

/**
 * Create a new character display
 * @param	colCount	the number of columns
 * @param	rowCount	the number of rows
 * @param	txBuffers	buffers to use for text writing
 */
MHV_Display_Character::MHV_Display_Character(uint16_t colCount, uint16_t rowCount,
		MHV_RingBuffer &txBuffers) :
			MHV_Device_TX(txBuffers){
	_colCount = colCount;
	_rowCount = rowCount;
	_wrap = true;
	_scroll = true;
	_currentRow = 0;
	_currentCol = 0;
}

/**
 * Get the width of the display
 */
uint8_t MHV_Display_Character::getWidth() {
	return _colCount;
}

/**
 * Get the width of the display
 */
uint8_t MHV_Display_Character::getHeight() {
	return _rowCount;
}

/**
 * Write a string to the display
 * @param	offsetX		the horizontal offset to start writing at
 * @param	offsetY		the vertical offset to start writing at
 * @param	string		the string to write
 * @return true if anything was written
 */
bool MHV_Display_Character::writeString(int16_t *offsetX, uint16_t offsetY, const char *string) {
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
	if (offsetY >= 0 && offsetY < _rowCount) {
		while (*p != '\0' && (*offsetX)++ < (int16_t)_colCount) {
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
bool MHV_Display_Character::writeBuffer(int16_t *offsetX, uint16_t offsetY,
		const char *buffer, uint16_t length) {
	bool ret = false;
	uint16_t i = 0;

	if (*offsetX < 0) {
		i -= *offsetX;
		*offsetX = 0;
	}

	setCursor((uint16_t)*offsetX, offsetY);

	while (i < length && (*offsetX)++ < (int16_t)_colCount) {
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
bool MHV_Display_Character::writeString_P(int16_t *offsetX, uint16_t offsetY, PGM_P string) {
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
	if (offsetY >= 0 && offsetY < _rowCount) {
		while ('\0' != (c = pgm_read_byte(p++)) && (*offsetX)++ < (int16_t)_colCount) {
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
bool MHV_Display_Character::writeBuffer_P(int16_t *offsetX, uint16_t offsetY,
		PGM_P buffer, uint16_t length) {
	bool ret = false;
	uint16_t i = 0;

	while (*offsetX < 0) {
		(*offsetX)++;
		i++;
	}
	setCursor((uint16_t)*offsetX, offsetY);

	while (i < length && (*offsetX)++ < (int16_t)_colCount) {
		ret = true;
		writeChar(pgm_read_byte(buffer + i++));
	}

	return ret;
}

/**
 * Start rendering TX buffers
 */
void MHV_Display_Character::runTxBuffers() {
	_txOffset = _colCount - 1;
	moreTX();
}

/**
 * Render a frame of TX buffer animation - scrolls text from right to left, before moving to the next buffer
 * @param	row		the vertical pixel offset to start writing at
 * @return true if there are more frames to be rendered
 */
bool MHV_Display_Character::txAnimation(uint16_t row) {
	if (!_tx) {
		return false;
	}

	int16_t offsetX = _txOffset--;

	setCursor(0, row);

	bool ret;
	if (_currentTx.progmem) {
		if (_currentTx.isString) {
			ret = writeString_P(&offsetX, row, _tx);
		} else {
			ret = writeBuffer_P(&offsetX, row, _tx, _currentTx.length);
		}
	} else {
		if (_currentTx.isString) {
			ret = writeString(&offsetX, row, _tx);
		} else {
			ret = writeBuffer(&offsetX, row, _tx, _currentTx.length);
		}
	}

	while (offsetX++ < (int16_t)_colCount) {
		writeChar(' ');
	}

	if (!ret) {
		_txOffset = _colCount - 1;
		return moreTX();
	}

	return true;
}

/**
 * Should the display automatically wrap characters?
 * @param	shouldWrap	true to wrap
 */
void MHV_Display_Character::setWrap(bool shouldWrap) {
	_wrap = shouldWrap;
}

/**
 * Should the automatically scroll characters vertically?
 * @param	shouldScroll	true to scroll
 */
void MHV_Display_Character::setScroll(bool shouldScroll) {
	_scroll = shouldScroll;
}

/**
 * Position the cursor
 * @param	col		the column to set
 * @param	row		the row to set
 */
void MHV_Display_Character::setCursor(uint16_t col, uint16_t row) {
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
void MHV_Display_Character::writeChar(char character) {
	switch (character) {
	case '\b':
		if (_wrap && 0 == _currentCol) {
// Go to the previous line
			if (_currentRow + 1 < _rowCount) {
				_currentRow++;
			}
			setCursor(_colCount - 1, _currentRow);
		} else {
			setCursor(_currentCol - 1, _currentRow);
		}
		break;
	case '\t':
		_currentCol = (_currentCol + 4) & ~(4 - 1);
		if (_currentCol >= _colCount) {
			if (_wrap) {
				if (_currentRow > 0) {
					_currentRow--;
				} else if (_scroll) {
					scrollVertically();
					break;
				}
				_currentCol = 0;
			} else {
				_currentCol = _colCount - 1;
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
		if (_currentCol++ >= _colCount - 1 && _wrap) {
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
	}
}

/**
 * Scroll the display up, leaving a blank line at the bottom
 */
void MHV_Display_Character::scrollVertically() {
	uint16_t x, y;
	char	c;

	for (y = _rowCount - 2; y > 0; y--) {
// Copy each character in this row to the row above
		for (x = 0; x < _colCount; ++x) {
			_setCursor(x, y + 1);
			c = _readChar();
			_setCursor(x, y);
			_writeChar(c);
		}
	}

// Blank the last row
	_setCursor(0, 0);
	for (x = 0; x < _colCount; ++x) {
		_writeChar(' ');
	}

// Set the cursor ready to write another line
	setCursor(0, 0);
}
