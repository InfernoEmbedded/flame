/*
 * Copyright (c) 2010, Make, Hack, Void Inc
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
#ifndef MHV_DISPLAY_HD44780_H_
#define MHV_DISPLAY_HD44780_H_

#include <MHV_Display_Character.h>

enum mhv_hd44780_command {
	MHV_44780_CMD_CLEAR				= 0x001,
	MHV_44780_CMD_RETURN_HOME		= 0x002,
	MHV_44780_CMD_SET_ENTRY_MODE	= 0x004,
	MHV_44780_CMD_SET_DISPLAY_MODE	= 0x008,
	MHV_44780_CMD_SET_CURSOR_MODE	= 0x010,
	MHV_44780_CMD_SET_FUNCTION		= 0x020,
	MHV_44780_CMD_SET_CG_ADDR		= 0x040,
	MHV_44780_CMD_SET_DD_ADDR		= 0x080
};
typedef enum mhv_hd44780_command MHV_HD44780_COMMAND;

class MHV_Display_HD44780 : public MHV_Display_Character {
protected:
	bool 				_mustDelay;
	uint16_t			_ticks;
	uint16_t			_animateTicks;

	void writeCommand(MHV_HD44780_COMMAND command, uint8_t data);
	void function(bool multiLine, bool bigFont);
	void addressCGRAM(uint8_t address);
	void addressDDRAM(uint8_t address);
	virtual void writeNibble(uint8_t nibble, bool rs)=0;
	virtual uint8_t readNibble(bool rs)=0;
	void _setCursor(uint8_t col, uint8_t row);
	void _setCursor(uint16_t col, uint16_t row);
	void _writeChar(char character);
	char _readChar();

public:
	MHV_Display_HD44780(uint8_t colCount, uint16_t rowCount, MHV_RingBuffer *txBuffers);
	void init(bool multiLine, bool bigFont, bool cursorOn, bool cursorBlink,
			bool left2right, bool scroll);
	void clear();
	void entryMode(bool left2Right, bool scroll);
	void control(bool displayOn, bool cursorOn, bool cursorBlink);
	virtual bool isBusy()=0;
};

#endif /* MHV_DISPLAY_HD44780_H_ */
