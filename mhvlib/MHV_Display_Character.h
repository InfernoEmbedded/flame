/*
 * Copyright (c) 2011, Make, Hack, Void Inc
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

#include <MHV_Device_TX.h>

class MHV_Display_Character : public MHV_Device_TX {
protected:
	uint16_t		_rowCount;
	uint16_t		_colCount;
	int16_t			_txOffset;
	uint16_t		_currentRow;
	uint16_t		_currentCol;
	bool			_wrap;
	bool			_scroll;


public:
	MHV_Display_Character(uint16_t cols, uint16_t rows, MHV_RingBuffer &txBuffer);
	void writeChar(char character);
	void scrollVertically();
	void setCursor(uint16_t col, uint16_t row);
	void runTxBuffers();
	bool txAnimation(uint16_t row);
	uint8_t getWidth();
	uint8_t getHeight();
	bool writeString(int16_t *offsetX, uint16_t offsetY, const char *string);
	bool writeBuffer(int16_t *offsetX, uint16_t offsetY, const char *buffer, uint16_t length);
	bool writeString_P(int16_t *offsetX, uint16_t offsetY, PGM_P string);
	bool writeBuffer_P(int16_t *offsetX, uint16_t offsetY, PGM_P buffer, uint16_t length);
	void setWrap(bool shouldWrap);
	void setScroll(bool shouldScroll);
// Subclasses must implement these
	virtual void _writeChar(char character)=0;
	virtual char _readChar()=0;
	virtual void _setCursor(uint16_t col, uint16_t row)=0;
	virtual void clear()=0;
};

#endif /* MHV_HD44780_H_ */
