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

#ifndef MHV_SHIFTER_H_
#define MHV_SHIFTER_H_

#include "mhv_io.h"

#ifndef MHV_SHIFT_WRITECLOCK
#warning MHV_SHIFT_WRITECLOCK not defined
#endif

#ifndef MHV_SHIFT_WRITEDATA
#warning MHV_SHIFT_WRITEDATA not defined
#endif

#ifdef MHV_SHIFT_ORDER_MSB
#define SHIFTOUT_BYTE_LOOP (mhv_shift_i = 7; mhv_shift_i >= 0; mhv_shift_i--)
#define MHV_BIT_1 _BV(8)
#define MHV_BIT_2 _BV(7)
#define MHV_BIT_3 _BV(6)
#define MHV_BIT_4 _BV(5)
#define MHV_BIT_5 _BV(4)
#define MHV_BIT_6 _BV(3)
#define MHV_BIT_7 _BV(2)
#define MHV_BIT_8 _BV(1)
#else
#define SHIFTOUT_BYTE_LOOP (mhv_shift_i = 0; mhv_shift_i < 8; mhv_shift_i++)
#define MHV_BIT_1 _BV(1)
#define MHV_BIT_2 _BV(2)
#define MHV_BIT_3 _BV(3)
#define MHV_BIT_4 _BV(4)
#define MHV_BIT_5 _BV(5)
#define MHV_BIT_6 _BV(6)
#define MHV_BIT_7 _BV(7)
#define MHV_BIT_8 _BV(8)
#endif

/* Shift a byte out, works both for devices that read on rising clock
 * & falling clock
 */
#define MHV_SHIFTOUT_BYTE(mhv_data) \
do { \
	uint8_t		mhv_shift_i; \
\
for SHIFTOUT_BYTE_LOOP { \
		if ((mhv_data >> mhv_shift_i) & 0x01) { \
			_mhv_pin_on(MHV_SHIFT_WRITEDATA); \
		} else { \
			_mhv_pin_off(MHV_SHIFT_WRITEDATA); \
		} \
		_mhv_pin_on(MHV_SHIFT_WRITECLOCK); \
		_mhv_pin_off(MHV_SHIFT_WRITECLOCK); \
	} \
} while (0)

/* Write an array of data as clocked serial, where the reading device
 * reads data when the clock rises
 * Clock and data must be on the same port.
 * Other pins on this port should not be manipulated by interrupts or
 * this macro will corrupt them.
 * data & dataLength are sacrificial, upon completion, data will point to the
 * end of the array, and dataLength will be 0
 * mhv_dataCopy is used to avoid pointer indirection on each bit test
 *
 * param	data		a pointer to the data to write
 * param	dataLength	the length of the data (must be a variable)
 */
#define MHV_SHIFTOUT_ARRAY_CLOCKED_RISING(mhv_data, mhv_dataLength) \
do { \
	uint8_t dataOffClockOff = _mhv_out(MHV_SHIFT_WRITEDATA) & \
		~(_BV(_mhv_Bit(MHV_SHIFT_WRITEDATA) | _BV(_mhv_Bit(MHV_SHIFT_WRITECLOCK)))); \
	uint8_t dataOnClockOff = _mhv_out(MHV_SHIFT_WRITEDATA) & \
		~(_BV(_mhv_Bit(MHV_SHIFT_WRITEDATA) | _BV(_mhv_Bit(MHV_SHIFT_WRITECLOCK)))) | \
		_BV(_mhv_Bit(MHV_SHIFT_WRITEDATA)); \
	uint8_t dataOffClockOn = _mhv_out(MHV_SHIFT_WRITEDATA) & \
		~(_BV(_mhv_Bit(MHV_SHIFT_WRITEDATA) | _BV(_mhv_Bit(MHV_SHIFT_WRITECLOCK)))) | \
		_BV(_mhv_Bit(MHV_SHIFT_WRITECLOCK)); \
	uint8_t dataOnClockOn = _mhv_out(MHV_SHIFT_WRITEDATA) & \
		~(_BV(_mhv_Bit(MHV_SHIFT_WRITEDATA) | _BV(_mhv_Bit(MHV_SHIFT_WRITECLOCK)))) | \
		_BV(_mhv_Bit(MHV_SHIFT_WRITECLOCK)) | \
		_BV(_mhv_Bit(MHV_SHIFT_WRITEDATA)); \
	uint8_t mhv_dataCopy; \
\
	while (!dataLength--) { \
		mhv_dataCopy = *data++; \
		if ((mhv_dataCopy & MHV_BIT_1)) { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOff; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOn; \
		} else { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOff; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOn; \
		} \
		if ((mhv_dataCopy & MHV_BIT_2)) { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOff; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOn; \
		} else { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOff; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOn; \
		} \
		if ((mhv_dataCopy & MHV_BIT_3)) { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOff; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOn; \
		} else { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOff; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOn; \
		} \
		if ((mhv_dataCopy & MHV_BIT_4)) { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOff; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOn; \
		} else { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOff; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOn; \
		} \
		if ((mhv_dataCopy & MHV_BIT_5)) { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOff; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOn; \
		} else { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOff; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOn; \
		} \
		if ((mhv_dataCopy & MHV_BIT_6)) { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOff; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOn; \
		} else { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOff; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOn; \
		} \
		if ((mhv_dataCopy & MHV_BIT_7)) { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOff; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOn; \
		} else { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOff; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOn; \
		} \
		if ((mhv_dataCopy & MHV_BIT_8)) { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOff; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOn; \
		} else { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOff; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOn; \
		} \
	} \
} while (0)

/* Write an array of data as clocked serial, where the reading device
 * reads data when the clock falls
 * Clock and data must be on the same port.
 * Other pins on this port should not be manipulated by interrupts or
 * this macro will corrupt them.
 * data & dataLength are sacrificial, upon completion, data will point to the
 * end of the array, and dataLength will be 0
 * mhv_dataCopy is used to avoid pointer indirection on each bit test
 *
 * param	data		a pointer to the data to write
 * param	dataLength	the length of the data (must be a variable)
 *
 */
#define MHV_SHIFTOUT_ARRAY_CLOCKED_FALLING(mhv_data, mhv_dataLength) \
do { \
	uint8_t dataOffClockOff = _mhv_out(MHV_SHIFT_WRITEDATA) & \
		~(_BV(_mhv_Bit(MHV_SHIFT_WRITEDATA) | _BV(_mhv_Bit(MHV_SHIFT_WRITECLOCK)))); \
	uint8_t dataOnClockOff = _mhv_out(MHV_SHIFT_WRITEDATA) & \
		~(_BV(_mhv_Bit(MHV_SHIFT_WRITEDATA) | _BV(_mhv_Bit(MHV_SHIFT_WRITECLOCK)))) | \
		_BV(_mhv_Bit(MHV_SHIFT_WRITEDATA)); \
	uint8_t dataOffClockOn = _mhv_out(MHV_SHIFT_WRITEDATA) & \
		~(_BV(_mhv_Bit(MHV_SHIFT_WRITEDATA) | _BV(_mhv_Bit(MHV_SHIFT_WRITECLOCK)))) | \
		_BV(_mhv_Bit(MHV_SHIFT_WRITECLOCK)); \
	uint8_t dataOnClockOn = _mhv_out(MHV_SHIFT_WRITEDATA) & \
		~(_BV(_mhv_Bit(MHV_SHIFT_WRITEDATA) | _BV(_mhv_Bit(MHV_SHIFT_WRITECLOCK)))) | \
		_BV(_mhv_Bit(MHV_SHIFT_WRITECLOCK)) | \
		_BV(_mhv_Bit(MHV_SHIFT_WRITEDATA)); \
	uint8_t mhv_dataCopy; \
\
	while (!dataLength--) { \
		mhv_dataCopy = *data++; \
		if ((mhv_dataCopy & MHV_BIT_1)) { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOn; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOff; \
		} else { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOn; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOff; \
		} \
		if ((mhv_dataCopy & MHV_BIT_2)) { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOn; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOff; \
		} else { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOn; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOff; \
		} \
		if ((mhv_dataCopy & MHV_BIT_3)) { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOn; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOff; \
		} else { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOn; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOff; \
		} \
		if ((mhv_dataCopy & MHV_BIT_4)) { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOn; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOff; \
		} else { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOn; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOff; \
		} \
		if ((mhv_dataCopy & MHV_BIT_5)) { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOn; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOff; \
		} else { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOn; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOff; \
		} \
		if ((mhv_dataCopy & MHV_BIT_6)) { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOn; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOff; \
		} else { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOn; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOff; \
		} \
		if ((mhv_dataCopy & MHV_BIT_7)) { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOn; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOff; \
		} else { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOn; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOff; \
		} \
		if ((mhv_dataCopy & MHV_BIT_8)) { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOn; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOnClockOff; \
		} else { \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOn; \
			_mhv_out(MHV_SHIFT_WRITEDATA) = dataOffClockOff; \
		} \
	} \
} while (0)


/* Function variants of the above macros
 */
void mhv_shiftout_byte_lsb(MHV_PIN *data, MHV_PIN *clock, uint8_t byte);
void mhv_shiftout_byte_msb(MHV_PIN *data, MHV_PIN *clock, uint8_t byte);

#endif /* MHV_SHIFTER_H_ */
