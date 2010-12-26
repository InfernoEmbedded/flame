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

#ifndef MHV_SOFTWAREHBRIDGE_H_
#define MHV_SOFTWAREHBRIDGE_H_

#include <inttypes.h>
#include <MHV_io.h>
#include <MHV_Timer16.h>

enum mhv_SoftwareHBridge_direction {
	MHV_SOFTWAREHBRIDGE_DIR_COAST,
	MHV_SOFTWAREHBRIDGE_DIR_FORWARD,
	MHV_SOFTWAREHBRIDGE_DIR_BACKWARD,
	MHV_SOFTWAREHBRIDGE_DIR_BRAKE
};
typedef enum mhv_SoftwareHBridge_direction MHV_SOFTWAREHBRIDGE_DIRECTION;

enum mhv_SoftwareHBridge_type {
	MHV_SOFTWAREHBRIDGE_TYPE_PULLUP,
	MHV_SOFTWAREHBRIDGE_TYPE_DIRECT
};
typedef enum mhv_SoftwareHBridge_type MHV_SOFTWAREHBRIDGE_TYPE;

class MHV_SoftwareHBridge {
protected:
	MHV_SOFTWAREHBRIDGE_TYPE	_type;
	MHV_Timer16					*_timer;
	uint8_t						_timerChannel;

	volatile uint8_t			*_dir1Top;
	volatile uint8_t			*_out1Top;
	uint8_t 					_pin1Top;
	volatile uint8_t			*_out1Bottom;
	uint8_t						_pin1Bottom;
	volatile uint8_t			*_dir2Top;
	volatile uint8_t			*_out2Top;
	uint8_t						_pin2Top;
	volatile uint8_t			*_out2Bottom;
	uint8_t						_pin2Bottom;

	MHV_SOFTWAREHBRIDGE_DIRECTION _direction;

public:
	MHV_SoftwareHBridge(MHV_SOFTWAREHBRIDGE_TYPE type, MHV_Timer16 *timer, uint8_t timerChannel,
			volatile uint8_t *dir1Top, volatile uint8_t *out1Top, volatile uint8_t *in1Top, uint8_t pin1Top, int8_t int1Top,
			volatile uint8_t *dir1Bottom, volatile uint8_t *out1Bottom, volatile uint8_t *in1Bottom, uint8_t pin1Bottom, int8_t int1Bottom,
			volatile uint8_t *dir2Top, volatile uint8_t *out2Top, volatile uint8_t *in2Top, uint8_t pin2Top, int8_t int2Top,
			volatile uint8_t *dir2Bottom, volatile uint8_t *out2Bottom, volatile uint8_t *in2Bottom, uint8_t pin2Bottom, int8_t int2Bottom);
	void reset();
	void update();
	void set(MHV_SOFTWAREHBRIDGE_DIRECTION direction, uint16_t magnitude);
	void set(MHV_SOFTWAREHBRIDGE_DIRECTION direction);
};

#endif /* MHV_SOFTWAREHBRIDGE_H_ */
