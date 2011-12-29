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

#include "MHV_PWMMatrix.h"
#include <string.h>
#include <math.h>

#define pixel(pixelCol, pixelRow) _frameBuffer[pixelRow * _colCount + pixelCol]

/**
 * Establish a new matrix
 * @param	mode		whether to scan rows, cols, individual pixels or auto
 * @param	rowCount	the number of rows
 * @param	colCount	the number of columns
 * @param	frameBuffer	memory to use for the framebuffer, must be at least rows * cols * uint8_t
 * @param	txBuffers	buffers used for text transmission
 * @param	driver		the driver to turn on/off rows and columns
 */
MHV_PWMMatrix::MHV_PWMMatrix(uint16_t rowCount, uint16_t colCount, uint8_t *frameBuffer,
		MHV_RingBuffer &txBuffers, MHV_PWMMatrixDriver &driver,
		MHV_PWMMATRIX_MODE mode) :
			MHV_Display_Monochrome_Buffered (rowCount, colCount, frameBuffer, txBuffers),
			_currentRow(0),
			_currentCol(0),
			_currentLevel(0),
			_driver(driver) {
	uint8_t	i;

	if (MHV_PWMMATRIX_MODE_AUTO == mode) {
		_mode = (rowCount <= colCount) ? MHV_PWMMATRIX_MODE_ROWS : MHV_PWMMATRIX_MODE_COLS;
	} else {
		_mode = mode;
	}

	for (i = 0; i < _rowCount; i++) {
		_driver.rowOff(i);
	}
	for (i = 0; i < _colCount; i++) {
		_driver.colOff(i);
	}
}

/**
 * Render the display row by row
 */
inline void MHV_PWMMatrix::tickRow(void) {
	uint16_t		i;

	if (0 == _currentLevel) {
		// Turn on the current row
		_driver.rowOn(_currentRow);
		for (i = 0; i < _colCount; i++) {
			if (pixel(i, _currentRow) > 0) {
				_driver.colOn(i);
			}
		}
	} else {
		// Turn off pixels that get switched off on this pass
		for (i = 0; i < _colCount; i++) {
			if (pixel(i, _currentRow) <= _currentLevel) {
				_driver.colOff(i);
			}
		}
	}

	if (255 == ++_currentLevel) {
		// Turn off the current row & advance
		_driver.rowOff(_currentRow);

		_currentLevel = 0;
		if (++_currentRow == _rowCount) {
			_currentRow = 0;
		}
	}
}

/**
 * Render the display column by column
 */
inline void MHV_PWMMatrix::tickCol(void) {
	uint16_t		i;

	if (0 == _currentLevel) {
		// Turn on the current column
		_driver.colOn(_currentCol);
		for (i = 0; i < _rowCount; i++) {
			if (pixel(_currentCol, i) > 0) {
				_driver.rowOn(i);
			}
		}
	} else {
		// Turn off pixels that get switched off on this pass
		for (i = 0; i < _rowCount; i++) {
			if (pixel(_currentCol, i) <= _currentLevel) {
				_driver.rowOff(i);
			}
		}
	}

	if (255 == ++_currentLevel) {
		// Turn off the current column & advance
		_driver.colOff(_currentCol);

		_currentLevel = 0;
		if (++_currentCol == _colCount) {
			_currentCol = 0;
		}
	}
}

/**
 * Render the display pixel by pixel
 */
inline void MHV_PWMMatrix::tickPixel(void) {
	if (0 == _currentLevel) {
		// Turn on the pixel at the current row & column
		_driver.colOn(_currentCol);
		_driver.rowOn(_currentRow);
	} else if (pixel(_currentRow, _currentCol) <= _currentLevel) {
		// Turn off the pixel
		_driver.colOff(_currentCol);
		_driver.rowOff(_currentRow);
	}

	if (255 == ++_currentLevel) {
		// Advance the column
		if (++_currentCol == _colCount) {
			_currentCol = 0;

			// Advance the row
			if (++_currentRow == _rowCount) {
				_currentRow = 0;
			}
		}
	}
}


/* Process a timer tick
 */
void MHV_PWMMatrix::alarm() {
	switch (_mode) {
	case MHV_PWMMATRIX_MODE_ROWS:
		tickRow();
		break;
	case MHV_PWMMATRIX_MODE_COLS:
		tickCol();
		break;
	case MHV_PWMMATRIX_MODE_INDIVIDUAL:
		tickPixel();
		break;
	default:
		break;
	}
}
