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

#ifndef MHV_DISPLAY_MONOCHROME_BUFFERED_H_
#define MHV_DISPLAY_MONOCHROME_BUFFERED_H_

#include <inttypes.h>
#include <mhvlib/Display_Monochrome.h>
#include <string.h>
#include <math.h>

#define pixel(pixelRow, pixelCol) Display_Monochrome_Buffered<cols, rows, txBuffers>::_frameBuffer[pixelRow * cols + pixelCol]

namespace mhvlib {

/**
 * A monochrome bitmap display
 * Origin (0,0) is bottom left
 * @tparam	cols		the number of columns
 * @tparam	rows		the number of rows
 * @tparam	txBuffers	the number of output buffers
 */
template<uint16_t cols, uint16_t rows, uint8_t txBuffers>
class Display_Monochrome_Buffered : public Display_Monochrome<cols, rows, txBuffers> {
protected:
	uint8_t		_frameBuffer[cols * rows];

public:
	/**
	 * Create a new monochrome display
	 */
	Display_Monochrome_Buffered() {
		memset(_frameBuffer, 0, rows * cols);
	}

	/* Set a pixel
	 * param:	col		the column
	 * param:	row		the row
	 * param:	value	the intensity of the pixel
	 */
	void setPixel(uint16_t col, uint16_t row, uint8_t value) {
		if (row < rows && col < cols) {
			pixel(row, col) = value;
		}
	}

	/* Get a pixel
	 * param:	col		the column
	 * param:	row		the row
	 * return	the intensity of the pixel
	 */
	uint8_t getPixel(uint16_t col, uint16_t row) {
		if (row < rows && col < cols) {
			return pixel(row, col);
		} else {
			return 0;
		}
	}
};

}
#endif /* MHV_DISPLAY_MONOCHROME_BUFFERED_H_ */
