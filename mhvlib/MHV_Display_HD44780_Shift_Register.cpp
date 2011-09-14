/* Copyright (c) 2011, Make, Hack, Void Inc
 * All rights reserved.
 *
 * Contributed by Armel
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

#include <util/delay.h>

#include <MHV_Display_HD44780_Shift_Register.h>

#define MHV_SHIFT_ORDER_MSB
#define MHV_SHIFT_WRITECLOCK NULL,_clockOut,NULL,_clockPin,-1
#define MHV_SHIFT_WRITEDATA NULL,_dataOut,NULL,_dataPin,-1
#include <MHV_Shifter.h>

#define HD44780_TSU1	(60 * 1000000 / F_CPU / 3 + 1)
#define HD44780_TW		(450 * 1000000 / F_CPU / 3 + 1)


/**
 * A class for operating HD44780 based LCD displays via a shift register as 74HC164
 *
 * @param	data*		pin declaration for the data port on the MCU
 * @param	enable*		pin declaration for the enable port on the MCU
 * @param	clock*		pin declaration for the clock port on the MCU
 * @param	colCount	the number of columns on the display
 * @param	rowCount	the number of rows on the display
 * @param	txBuffers	buffers for async writing
 */
MHV_Display_HD44780_Shift_Register::MHV_Display_HD44780_Shift_Register(volatile uint8_t *dataDir,
		volatile uint8_t *dataOut, volatile uint8_t *dataIn, uint8_t dataPin, int8_t dataPinchangeInterrupt,
		volatile uint8_t *enableDir, volatile uint8_t *enableOut, volatile uint8_t *enableIn, uint8_t enablePin,
		int8_t enablePinchangeInterrupt, volatile uint8_t *clockDir, volatile uint8_t *clockOut,
		volatile uint8_t *clockIn, uint8_t clockPin, int8_t clockPinchangeInterrupt, uint8_t colCount,
		uint16_t rowCount, MHV_RingBuffer *txBuffers) :
	MHV_Display_HD44780(colCount, rowCount, txBuffers) {
	_dataOut = dataOut;
	_dataPin = dataPin;
	mhv_setOutput(dataDir, dataOut, dataIn, dataPin, -1);

	_enableOut = enableOut;
	_enablePin = enablePin;
	mhv_setOutput(enableDir, enableOut, enableIn, enablePin, -1);

	_clockOut = clockOut;
	_clockPin = clockPin;
	mhv_setOutput(clockDir, clockOut, clockIn, clockPin, -1);

}

/**
 * Write a byte to the display
 * @param 	byte	the data to write
 * @param	rs		true to set the RS pin (aka data pin )
 */
void MHV_Display_HD44780_Shift_Register::writeByte(uint8_t byte, bool rs) {
// setup RS
	byte |= (rs ? 0x01 : 0x00);

// Load the byte into the shift register
	MHV_SHIFTOUT_BYTE(byte);

// strobe Enable pin
	_delay_loop_1(HD44780_TSU1);
	mhv_pinOn(0, _enableOut, 0, _enablePin, -1);
	_delay_loop_1(HD44780_TW);
	mhv_pinOff(0, _enableOut, 0, _enablePin, -1);
	_delay_loop_1(HD44780_TW);
}

/**
 * Read a byte from the display
 * @param	rs		true to set the RS pin
 */
uint8_t MHV_Display_HD44780_Shift_Register::readByte(bool rs) {
	return 0;
}

/**
 * Check if the display is busy
 * @return true if the display is busy
 */
bool MHV_Display_HD44780_Shift_Register::isBusy() {
	return false;
}

/**
 * Post-command delays
 */
void MHV_Display_HD44780_Shift_Register::delay(MHV_HD44780_COMMAND command) {
	switch (command) {
	case MHV_44780_CMD_CLEAR:
	case MHV_44780_CMD_RETURN_HOME:
		_delay_ms(2);
		break;
	case MHV_44780_CMD_SET_ENTRY_MODE:
	case MHV_44780_CMD_SET_DISPLAY_MODE:
	case MHV_44780_CMD_SET_CURSOR_MODE:
	case MHV_44780_CMD_SET_CG_ADDR:
	case MHV_44780_CMD_SET_DD_ADDR:
	case MHV_44780_WRITE_CHAR:
	case MHV_44780_CMD_SET_FUNCTION:
		_delay_us(39);
		break;
	}
}