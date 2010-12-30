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

#ifndef MHV_DISPLAY_MONOCHROME_H_
#define MHV_DISPLAY_MONOCHROME_H_

#include <inttypes.h>
#include <avr/pgmspace.h>
#include <MHV_io.h>
#include <MHV_Font.h>

class MHV_Display_Monochrome {
protected:
	uint16_t		_colCount;
	uint16_t		_rowCount;

public:
	MHV_Display_Monochrome(uint16_t colCount, uint16_t rowCount);
	uint16_t getWidth();
	uint16_t getHeight();
	void clear(uint8_t value);
	bool writeChar(const MHV_FONT *font, int16_t *offsetX, int16_t offsetY,
			uint8_t onValue, uint8_t offValue, char character);
	bool writeSeperator(const MHV_FONT *font, int16_t *offsetX, int16_t offsetY,
			uint8_t onValue, uint8_t offValue);
	bool writeString(const MHV_FONT *font, int16_t *offsetX, int16_t offsetY,
			uint8_t onValue, uint8_t offValue, const char *string);
	bool writeString_P(const MHV_FONT *font, int16_t *offsetX, int16_t offsetY,
			uint8_t onValue, uint8_t offValue, const char *string);

	virtual void setPixel(uint16_t row, uint16_t col, uint8_t value)=0;
	virtual uint8_t getPixel(uint16_t row, uint16_t col)=0;
};

#endif /* MHV_DISPLAY_MONOCHROME_H_ */
