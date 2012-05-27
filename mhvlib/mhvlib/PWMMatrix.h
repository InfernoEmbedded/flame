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

#ifndef MHV_PWMMATRIX_H_
#define MHV_PWMMATRIX_H_

#include <inttypes.h>
#include <string.h>
#include <math.h>
#include <mhvlib/Display_Monochrome_Buffered.h>
#include <mhvlib/Timer.h>

enum MHV_PWMMatrix_Mode {
	MHV_PWMMATRIX_MODE_AUTO,
	MHV_PWMMATRIX_MODE_ROWS,
	MHV_PWMMATRIX_MODE_COLS,
	MHV_PWMMATRIX_MODE_INDIVIDUAL
};
typedef enum MHV_PWMMatrix_Mode MHV_PWMMATRIX_MODE;

class MHV_PWMMatrixDriver {
public:
	virtual void rowOn(uint16_t row) =0;
	virtual void rowOff(uint16_t row) =0;
	virtual void colOn(uint16_t col) =0;
	virtual void colOff(uint16_t col) =0;
};


#define _MODE ((MHV_PWMMATRIX_MODE_AUTO == mode) ? \
	((rows <= cols) ? MHV_PWMMATRIX_MODE_ROWS : MHV_PWMMATRIX_MODE_COLS) : mode)


/**
 * A driver for bitbashed LED matrices
 * @tparam	cols		the number of columns
 * @tparam	rows		the number of rows
 * @tparam	txBuffers	the number of output buffers
 * @tparam	mode		whether to scan rows, cols, individual pixels or auto
 */
template<uint16_t cols, uint16_t rows, uint8_t txBuffers, MHV_PWMMATRIX_MODE mode>
class MHV_PWMMatrix : public MHV_Display_Monochrome_Buffered<cols, rows, txBuffers>,
	public MHV_TimerListener {
private:
	uint16_t				_currentRow;
	uint16_t				_currentCol;
	uint8_t					_currentLevel;
	MHV_PWMMatrixDriver		&_driver;

	/**
	 * Render the display row by row
	 */
	inline void tickRow(void) {
		uint16_t		i;

		if (0 == _currentLevel) {
			// Turn on the current row
			_driver.rowOn(_currentRow);
			for (i = 0; i < cols; i++) {
				if (pixel(i, _currentRow) > 0) {
					_driver.colOn(i);
				}
			}
		} else {
			// Turn off pixels that get switched off on this pass
			for (i = 0; i < cols; i++) {
				if (pixel(i, _currentRow) <= _currentLevel) {
					_driver.colOff(i);
				}
			}
		}

		if (255 == ++_currentLevel) {
			// Turn off the current row & advance
			_driver.rowOff(_currentRow);

			_currentLevel = 0;
			if (++_currentRow == rows) {
				_currentRow = 0;
			}
		}
	}

	/**
	 * Render the display column by column
	 */
	inline void tickCol(void) {
		uint16_t		i;

		if (0 == _currentLevel) {
			// Turn on the current column
			_driver.colOn(_currentCol);
			for (i = 0; i < rows; i++) {
				if (pixel(_currentCol, i) > 0) {
					_driver.rowOn(i);
				}
			}
		} else {
			// Turn off pixels that get switched off on this pass
			for (i = 0; i < rows; i++) {
				if (pixel(_currentCol, i) <= _currentLevel) {
					_driver.rowOff(i);
				}
			}
		}

		if (255 == ++_currentLevel) {
			// Turn off the current column & advance
			_driver.colOff(_currentCol);

			_currentLevel = 0;
			if (++_currentCol == cols) {
				_currentCol = 0;
			}
		}
	}

	/**
	 * Render the display pixel by pixel
	 */
	inline void tickPixel(void) {
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
			if (++_currentCol == cols) {
				_currentCol = 0;

				// Advance the row
				if (++_currentRow == rows) {
					_currentRow = 0;
				}
			}
		}
	}

public:
	/**
	 * Establish a new matrix
	 * @param	driver		the driver to turn on/off rows and columns
	 */
	MHV_PWMMatrix(MHV_PWMMatrixDriver &driver) :
				_currentRow(0),
				_currentCol(0),
				_currentLevel(0),
				_driver(driver) {
		uint8_t	i;

		for (i = 0; i < rows; i++) {
			_driver.rowOff(i);
		}
		for (i = 0; i < cols; i++) {
			_driver.colOff(i);
		}
	}

	/* Process a timer tick
	 */
	void alarm() {
		switch (_MODE) {
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
};

#endif /* MHV_PWMMATRIX_H_ */
