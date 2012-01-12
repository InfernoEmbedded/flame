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
#ifndef MHV_DISPLAY_HOLTEK_HT1632_H_
#define MHV_DISPLAY_HOLTEK_HT1632_H_

#include <MHV_Display_Monochrome.h>
#include <MHV_Shifter.h>

#define MHV_HT1632_BRIGHTNESS_MIN	0
#define MHV_HT1632_BRIGHTNESS_MED	7
#define MHV_HT1632_BRIGHTNESS_MAX	15

enum mhv_ht1632_command {
	MHV_HT1632_COMMAND_READ		=	0b110,
	MHV_HT1632_COMMAND_WRITE	=	0b101,
	MHV_HT1632_COMMAND_CMD		=	0b100
};
typedef enum mhv_ht1632_command MHV_HT1632_COMMAND;

enum mhv_ht1632_mode {
	MHV_HT1632_NMOS_32x8	=	0b00,
	MHV_HT1632_NMOS_24x16	=	0b01,
	MHV_HT1632_PMOS_32x8	=	0b10,
	MHV_HT1632_PMOS_24x16	=	0b11
};
typedef mhv_ht1632_mode MHV_HT1632_MODE;

/**
 * Create a new HT1632 driver to control an array of displays
 * The MHV_SHIFT_WRITECLOCK & MHV_SHIFT_WRITEDATA macros should be defined in advance
 *
 * @param	__mhvObjectName			the variable name of the object
 * @param	__mhvMode				the MHV_HT1632_MODE of the displays
 * @param	__mhvDisplayBytes		the number of bytes in a single display
 * @param	__mhvArrayX				the number of displays in the X axis
 * @param	__mhvArrayY				the number of displays in the Y axis
 * @param	__mhvSelector			the selector to choose the active display
 * @param	__mhvTxBufferCount		the number of TX buffers
 */
#define MHV_HOLTEK_HT1632_CREATE(__mhvObjectName,__mhvMode, __mhvDisplayBytes, __mhvArrayX, __mhvArrayY, __mhvSelector, __mhvTxBufferCount) \
	uint8_t __mhvObjectName ## _Buffer[__mhvDisplayBytes * __mhvArrayX * __mhvArrayY]; \
	MHV_SHIFTER_CLOCKED_RISING_CREATE(__mhvObjectName ## _Shifter); \
	MHV_TX_BUFFER_CREATE(__mhvObjectName ## TX, __mhvTxBufferCount); \
	MHV_Display_Holtek_HT1632 __mhvObjectName(__mhvObjectName ## _Shifter, __mhvSelector, \
			__mhvMode, __mhvArrayX, __mhvArrayY, __mhvObjectName ## _Buffer, __mhvObjectName ## TX);

class MHV_Display_Holtek_HT1632 : public MHV_Display_Monochrome {
private:
	MHV_Shifter				&_shifter;
	MHV_Display_Selector	&_selector;
	MHV_HT1632_MODE			_mode;
	uint8_t					_arrayX;
	uint8_t					_arrayY;
	uint8_t					_displayX;
	uint8_t					_displayY;
	uint8_t					_displayBytes;
	uint8_t					*_frameBuffer;

	void sendCommand(uint8_t moduleX, uint8_t moduleY, MHV_HT1632_COMMAND command);
	void commandComplete(uint8_t moduleX, uint8_t moduleY);
	void outputStart(uint8_t moduleX, uint8_t moduleY);
	void master(uint8_t moduleX, uint8_t moduleY);
	void slave(uint8_t moduleX, uint8_t moduleY);
	void brightness(uint8_t moduleX, uint8_t moduleY, uint8_t brightness);
	void poweroff(uint8_t moduleX, uint8_t moduleY);
	void poweron(uint8_t moduleX, uint8_t moduleY);
	void setMode(uint8_t moduleX, uint8_t moduleY, MHV_HT1632_MODE mode);

public:
	MHV_Display_Holtek_HT1632(MHV_Shifter &shifter, MHV_Display_Selector &selector,
			MHV_HT1632_MODE mode, uint8_t arrayX, uint8_t arrayY, uint8_t *frameBuffer, MHV_RingBuffer &txBuffers);
	void brightness(uint8_t brightness);
	void poweroff();
	void poweron();
	void flush();

// Implements MHV_Display_Monochrome
	void setPixel(uint16_t row, uint16_t col, uint8_t value);
	uint8_t getPixel(uint16_t row, uint16_t col);

};

#endif /* MHV_DISPLAY_HOLTEK_HT1632_H_ */
