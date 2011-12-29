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

#ifndef MHV_WS2801_H_
#define MHV_WS2801_H_

#define MHV_RGB_ORDER 5
#include <MHV_RGB.h>
#include <MHV_GammaCorrect.h>

// Squelch warnings about MHV_SHIFT_WRITECLOCK & MHV_SHIFT_WRITEDATA being undefined
#ifndef MHV_SHIFT_WRITECLOCK
#define MHV_SHIFT_WRITECLOCK
#define MHV_SHIFT_WRITEDATA
#endif
#include <MHV_Shifter.h>

/**
 * Create a new WS2801 object to control a string of LED drivers
 * The MHV_SHIFT_WRITECLOCK & MHV_SHIFT_WRITEDATA macros should be defined in advance
 *
 * @param	__mhvObjectName		the variable name of the object
 * @param	__mhvLength			the number of chips in the string
 */
#define MHV_WS2801_CREATE(__mhvObjectName,__mhvLength) \
	MHV_RGB __mhvObjectName ## _Buffer[__mhvLength]; \
	MHV_SHIFTER_CLOCKED_RISING_CREATE(__mhvObjectName ## _Shifter); \
	MHV_WS2801 __mhvObjectName(__mhvObjectName ## _Buffer, __mhvLength, __mhvObjectName ## _Shifter);

class MHV_WS2801 {
private:
	uint16_t			_length;
	MHV_RGB				*_data;

	MHV_Shifter			&_shifter;

public:
	MHV_WS2801(MHV_RGB buf[], uint16_t length, MHV_Shifter &shifter);
	void setPixel(uint16_t pixel, uint8_t red, uint8_t green, uint8_t blue);
	void setPixel(uint16_t pixel, MHV_RGB *value);
	void setAll(uint8_t red, uint8_t green, uint8_t blue);
	void setAll(MHV_RGB *value);
	void setPixelGamma(uint16_t pixel, uint8_t red, uint8_t green, uint8_t blue);
	void setPixelGamma(uint16_t pixel, MHV_RGB *value);
	void setAllGamma(uint8_t red, uint8_t green, uint8_t blue);
	void setAllGamma(MHV_RGB *value);
	void rotate(bool forwards);
	void flush();

};

#endif /* MHV_WS2801_H_ */
