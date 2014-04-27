/*
 * Copyright (c) 2014, Inferno Embedded
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of the Inferno Embedded nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL INFERNO EMBEDDED BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FLAME_SOFTWAREHBRIDGE_H_
#define FLAME_SOFTWAREHBRIDGE_H_

#ifdef FLAME_TIMER16_1

#include <inttypes.h>
#include <flame/io.h>
#include <flame/Timer16.h>

namespace flame {

enum class SoftwareHBridgeDirection : uint8_t {
	COAST,
	FORWARD,
	BACKWARD,
	BRAKE
};

enum class SoftwareHBridgeType : bool {
	PULLUP,
	DIRECT
};

class SoftwareHBridge : public TimerListener {
protected:
	SoftwareHBridgeType		_type;
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

	SoftwareHBridgeDirection	_direction;

public:
	SoftwareHBridge(SoftwareHBridgeType type, FLAME_Timer &timer, uint8_t timerChannel,
			FLAME_DECLARE_PIN(dir1Top),
			FLAME_DECLARE_PIN(dir1Bottom),
			FLAME_DECLARE_PIN(dir2Top),
			FLAME_DECLARE_PIN(dir2Bottom));
	void reset();
	void alarm();
	void set(SoftwareHBridgeDirection direction, uint16_t magnitude);
	void set(SoftwareHBridgeDirection direction);
};

}

#endif // FLAME_TIMER16_1

#endif /* FLAME_SOFTWAREHBRIDGE_H_ */
