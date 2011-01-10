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

#include "MHV_Display_Monochrome_Buffered.h"
#include <string.h>
#include <math.h>

#define pixel(pixelRow, pixelCol) _frameBuffer[pixelRow * _colCount + pixelCol]

/**
 * Create a new monochrome display
 * param:	colCount	the number of columns
 * param:	rowCount	the number of rows
 * param:	frameBuffer	memory to use for the framebuffer, must be at least rows * cols * uint8_t
 */
MHV_Display_Monochrome_Buffered::MHV_Display_Monochrome_Buffered(
		uint16_t colCount, uint16_t rowCount,
		uint8_t *frameBuffer, MHV_RingBuffer *txBuffers) :
			MHV_Display_Monochrome(rowCount, colCount, txBuffers) {
	_frameBuffer = frameBuffer;
	memset(_frameBuffer, 0, rowCount * colCount);
}

/* Set a pixel
 * param:	col		the column
 * param:	row		the row
 * param:	value	the intensity of the pixel
 */
void MHV_Display_Monochrome_Buffered::setPixel(uint16_t col, uint16_t row, uint8_t value) {
	if (row < _rowCount && col < _colCount) {
		pixel(row, col) = value;
	}
}

/* Get a pixel
 * param:	col		the column
 * param:	row		the row
 * return	the intensity of the pixel
 */
uint8_t MHV_Display_Monochrome_Buffered::getPixel(uint16_t col, uint16_t row) {
	if (row < _rowCount && col < _colCount) {
		return pixel(row, col);
	} else {
		return 0;
	}
}
