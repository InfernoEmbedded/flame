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

#include "MHV_PWMMatrix.h"
#include <string.h>
#include <math.h>

#define pixel(pixelRow, pixelCol) _frameBuffer[pixelRow * _colCount + pixelCol]

/* Constructor
 * param:	rowCount	the number of rows
 * param:	colCount	the number of columns
 * param:	frameBuffer	memory to use for the framebuffer, must be at least rows * cols * uint8_t
 */
MHV_PWMMatrix::MHV_PWMMatrix(uint8_t rowCount, uint8_t colCount, uint8_t *frameBuffer,
		void (*rowOn)(uint8_t row),
		void (*rowOff)(uint8_t row),
		void (*colOn)(uint8_t column),
		void (*colOff)(uint8_t column)) {
	uint8_t	i;

	_rowCount = rowCount;
	_colCount = colCount;
	_frameBuffer = frameBuffer;
	_current = 0;
	_currentLevel = 0;

	_rowOn = rowOn;
	_rowOff = rowOff;
	_colOn = colOn;
	_colOff = colOff;

	_scanRows = (rowCount <= colCount);

	memset(_frameBuffer, 0, rowCount * colCount);

	for (i = 0; i < _rowCount; i++) {
		_rowOff(i);
	}
	for (i = 0; i < _colCount; i++) {
		_colOff(i);
	}
}

/* Set a pixel
 * param:	row		the row
 * param:	col		the column
 * param:	value	the intensity of the pixel
 */
void MHV_PWMMatrix::setPixel(uint8_t row, uint8_t col, uint8_t value) {
	pixel(row, col) = value;
}

/* Get a pixel
 * param:	row		the row
 * param:	col		the column
 * return	the intensity of the pixel
 */
uint8_t MHV_PWMMatrix::getPixel(uint8_t row, uint8_t col) {
	return pixel(row, col);
}

inline void MHV_PWMMatrix::tickRow(void) {
	uint8_t		i;

	if (0 == _currentLevel) {
		// Turn on the current column
		_rowOn(_current);
		for (i = 0; i < _colCount; i++) {
			if (pixel(_current, i) > 0) {
				_colOn(i);
			}
		}
	} else {
		// Turn off pixels that get switched off on this pass
		for (i = 0; i < _colCount; i++) {
			if (pixel(_current, i) <= _currentLevel) {
				_colOff(i);
			}
		}
	}

	if (255 == ++_currentLevel) {
		// Turn off the current row & advance
		_rowOff(_current);

		_currentLevel = 0;
		if (++_current == _rowCount) {
			_current = 0;
		}
	}
}

inline void MHV_PWMMatrix::tickCol(void) {
	uint8_t		i;

	if (0 == _currentLevel) {
		// Turn on the current column
		_colOn(_current);
		for (i = 0; i < _rowCount; i++) {
			if (pixel(i, _current) > 0) {
				_rowOn(i);
			}
		}
	} else {
		// Turn off pixels that get switched off on this pass
		for (i = 0; i < _rowCount; i++) {
			if (pixel(i, _current) <= _currentLevel) {
				_rowOff(i);
			}
		}
	}

	if (255 == ++_currentLevel) {
		// Turn off the current column & advance
		_colOff(_current);

		_currentLevel = 0;
		if (++_current == _colCount) {
			_current = 0;
		}
	}
}

/* Process a timer tick
 */
void MHV_PWMMatrix::tick(void) {
	if (_scanRows) {
		tickRow();
	} else {
		tickCol();
	}
}