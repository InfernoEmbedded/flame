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
#ifndef MHV_DISPLAY_HD44780_DIRECT_CONNECT_H_
#define MHV_DISPLAY_HD44780_DIRECT_CONNECT_H_

#include <MHV_Display_HD44780.h>

class MHV_Display_HD44780_Direct_Connect : public MHV_Display_HD44780 {
protected:
	volatile uint8_t	*_dataDir;
	volatile uint8_t	*_dataOut;
	volatile uint8_t	*_dataIn;
	uint8_t				_dataPin;
	uint8_t				_dataMask;
	volatile uint8_t	*_controlOut;
	uint8_t				_controlPin;
	volatile uint8_t	*_visualOut;
	uint8_t				_visualPin;

	uint8_t				_brightness;
	uint8_t				_contrast;

	void writeNibble(uint8_t nibble, bool rs);
	uint8_t readNibble(bool rs);

public:
	MHV_Display_HD44780_Direct_Connect(volatile uint8_t *dataDir, volatile uint8_t *dataOut, volatile uint8_t *dataIn, uint8_t dataPin, int8_t dataPinchangeInterrupt,
			volatile uint8_t *controlDir, volatile uint8_t *controlOut, volatile uint8_t *controlIn, uint8_t controlPin, int8_t controlPinchangeInterrupt,
			volatile uint8_t *visualDir, volatile uint8_t *visualOut, volatile uint8_t *visualIn, uint8_t visualPin, int8_t visualPinchangeInterrupt,
			uint8_t colCount, uint16_t rowCount, MHV_RingBuffer *txBuffers);
	void setBacklight(uint8_t value);
	void setContrast(uint8_t value);
	void tickPWM();
	bool isBusy();
};

#endif /* MHV_DISPLAY_HD44780_DIRECT_CONNECT_H_ */
