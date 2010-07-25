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

#ifndef MHV_PWMMATRIX_H_
#define MHV_PWMMATRIX_H_

#include <inttypes.h>

class MHV_PWMMatrix {
private:
	uint8_t		_rowCount;
	uint8_t		_colCount;
	uint8_t		*_frameBuffer;
	uint8_t		_current; // current row or col, depending on scan mode
	uint8_t		_currentLevel;
	bool		_scanRows;
	void		(*_rowOn)(uint8_t row);
	void 		(*_rowOff)(uint8_t row);
	void 		(*_colOn)(uint8_t column);
	void 		(*_colOff)(uint8_t column);

	void tickRow(void);
	void tickCol(void);

public:
	MHV_PWMMatrix(uint8_t rowCount, uint8_t colCount, uint8_t *frameBuffer,
		void (*rowOn)(uint8_t row),
		void (*rowOff)(uint8_t row),
		void (*colOn)(uint8_t column),
		void (*colOff)(uint8_t column));
	void setPixel(uint8_t row, uint8_t col, uint8_t value);
	uint8_t getPixel(uint8_t row, uint8_t col);
	void tick(void);
};

#endif /* MHV_PWMMATRIX_H_ */