/*
 * Copyright (c) 2012, Make, Hack, Void Inc
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

#ifdef MHV_TIMER16_1

#include <inttypes.h>
#include <mhvlib/io.h>
#include <mhvlib/Timer16.h>

namespace mhvlib {

enum class softwarehbridge_direction : uint8_t {
	COAST,
	FORWARD,
	BACKWARD,
	BRAKE
};
typedef enum softwarehbridge_direction SOFTWAREHBRIDGE_DIRECTION;

enum class softwarehbridge_type : bool {
	PULLUP,
	DIRECT
};
typedef enum softwarehbridge_type SOFTWAREHBRIDGE_TYPE;

class SoftwareHBridge : public TimerListener {
protected:
	SOFTWAREHBRIDGE_TYPE		_type;
	Timer						&_timer;

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

	SOFTWAREHBRIDGE_DIRECTION	_direction;

public:
	SoftwareHBridge(SOFTWAREHBRIDGE_TYPE type, MHV_Timer &timer, uint8_t timerChannel,
			MHV_DECLARE_PIN(dir1Top),
			MHV_DECLARE_PIN(dir1Bottom),
			MHV_DECLARE_PIN(dir2Top),
			MHV_DECLARE_PIN(dir2Bottom));
	void reset();
	void alarm();
	void set(SOFTWAREHBRIDGE_DIRECTION direction, uint16_t magnitude);
	void set(SOFTWAREHBRIDGE_DIRECTION direction);
};

}

#endif // MHV_TIMER16_1

#endif /* MHV_SOFTWAREHBRIDGE_H_ */
