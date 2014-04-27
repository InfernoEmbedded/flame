/* Copyright (c) 2014, Inferno Embedded
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

#ifndef FLAME_WS2801_H_
#define FLAME_WS2801_H_

#define FLAME_RGB_ORDER 5
#include <flame/RGBLEDStrip.h>
#include <flame/Shifter.h>

namespace flame {

/**
 * Create a new WS2801 object to control a string of LED drivers
 * @tparam	clock...	the clock pin for the LEDs
 * @tparam	data...		the data pin for the LEDs, must be on the same port as the clock
 * @tparam	length		the number of LEDs in the string
 */
template <FLAME_DECLARE_PIN(clock), FLAME_DECLARE_PIN(data), uint16_t length>
class WS2801 : public RGBLEDStrip<length> {
private:
	ShifterImplementation<FLAME_PIN_PARMS(clock), FLAME_PIN_PARMS(data), true, true, 2>
				_shifter;

public:
	/**
	 * Create a new driver for a string of WS2801 LEDs
	 */
	WS2801() {
		setOutput(FLAME_PIN_PARMS(clock));
		setOutput(FLAME_PIN_PARMS(data));
	}

	/**
	 * Write the current buffer to the string of chips
	 */
	void flush() {
		_shifter.shiftOut((uint8_t *)RGBLEDStrip<length>::_data,
				FLAME_BYTESIZEOF(*RGBLEDStrip<length>::_data), length);
	}
};

}
#endif /* FLAME_WS2801_H_ */
