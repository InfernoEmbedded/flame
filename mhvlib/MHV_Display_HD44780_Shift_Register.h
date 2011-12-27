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
#ifndef DISPLAY_HD44780_SHIFT_REGISTER_H_
#define DISPLAY_HD44780_SHIFT_REGISTER_H_

#include <MHV_Display_HD44780.h>

class MHV_Display_HD44780_Shift_Register : public MHV_Display_HD44780 {
protected:
	volatile uint8_t	*_dataOut;
	uint8_t				_dataPin;

	volatile uint8_t	*_enableOut;
	uint8_t 			_enablePin;

	volatile uint8_t	*_clockOut;
	uint8_t 			_clockPin;

	void pushBits(uint8_t byte, bool rs);
	void writeByte(uint8_t byte, bool rs);
	uint8_t readByte(bool rs);

	bool isBusy();
	void delay(MHV_HD44780_COMMAND command);

public:
	MHV_Display_HD44780_Shift_Register(
			MHV_DECLARE_PIN(data),
			MHV_DECLARE_PIN(enable),
			MHV_DECLARE_PIN(clock),
			uint8_t colCount, uint16_t rowCount, MHV_RingBuffer &txBuffers);
	void init(bool multiLine, bool bigFont, bool cursorOn, bool cursorBlink,
			bool left2right, bool scroll);
};

#endif /* MHV_DISPLAY_HD44780_SHIFT_REGISTER_H_ */
