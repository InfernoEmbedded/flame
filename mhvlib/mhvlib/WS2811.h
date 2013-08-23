/* Copyright (c) 2013, Make, Hack, Void Inc
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
 *
 * Adapted from Lasse Rasinen work on using the timer hardware for WS2811 on Arduino
 * http://techblog.zenrobotics.com/2013/04/bit-banging-ws2811-led-strips-with-pwm.html
 * https://github.com/lrasinen/ws2811/blob/master/pwm_led.ino
 */

#ifndef MHV_WS2811_H_
#define MHV_WS2811_H_

#define MHV_RGB_ORDER 3
#include <mhvlib/RGBLEDStrip.h>
#include <mhvlib/Timer.h>
#include <util/delay.h>

namespace mhvlib {

/**
 * Create a new WS2811 object to control a string of LED drivers
 * @tparam	dataPin...		the data pin for the LEDs (This must be the same pin that Output2 (OCRnB) is on)
 * @tparam	length		the number of LEDs in the string
 */
template<MHV_DECLARE_PIN(dataPin), uint16_t length>
class WS2811: public RGBLEDStrip<length> {
private:
	Timer &_timer;
	uint8_t _lowClocks;
	uint8_t _highClocks;

	/**
	 * Get the number of clocks for the high->low transition for a given bit
	 * @param data	the byte we are currently transmitting
	 * @param bit	the bit offset
	 */
	INLINE uint8_t getClocks(uint8_t data, uint8_t bit) {
//		return (data & (1 << bit)) ? _highClocks : _lowClocks;
		return (data & (1 << bit)) ? 20 : 5;
	}

	/**
	 * Send a bit down the line
	 * @param data	the byte we are currently transmitting
	 * @param bit	the bit offset
	 */
	inline void bang_bit(uint8_t data, uint8_t bit) {
		_timer.setOutput2(getClocks(data, bit));
		_timer.clearOverflow();
	}

public:
	/**
	 * Create a new driver for a string of WS2811 LEDs
	 * *@param timer	a timer that will be used to implement the WS2811 protocol (should be in PWM_FAST mode)
	 */
	WS2811(Timer &timer) :
			_timer(timer) {
		setOutput(MHV_PIN_PARMS(dataPin));
//		_timer.setTimes(0.35f, 0.7f);
//		_lowClocks = _timer.getOutput1();
//		_highClocks = _timer.getOutput2();

//		_timer.setTimes(1.25f, 1.25f);
		_timer.setPrescaler(TimerPrescaler::PRESCALER_5_1);
		_timer.setTop(25);
		_timer.connectOutput2(TimerConnect::CLEAR);
	}

	/**
	 * Write the current buffer to the string of chips
	 */
	void flush() {
		uint8_t *myData = (uint8_t *)RGBLEDStrip<length>::_data;

		// Prepare first bit
		bool isFirstByte = true;
		uint8_t nextByte = myData[0];
		_timer.setOutput2(getClocks(nextByte, 7));
		_timer.setCurrent(0);

		uint8_t *last = myData + (length * sizeof(*RGBLEDStrip<length>::_data));

		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			pinOn(MHV_PIN_PARMS(dataPin));
			_timer.enable();

			while (true) {
				uint8_t b = nextByte;
				char finished;
				if (isFirstByte) {
					bang_bit(b, 6);
				} else {
					_timer.waitForOverflow();
					bang_bit(b, 7);
					_timer.waitForOverflow();
					bang_bit(b, 6);
				}

				_timer.waitForOverflow();
				bang_bit(b, 5);
				isFirstByte = false;

				_timer.waitForOverflow();
				bang_bit(b, 4);
				myData++;

				_timer.waitForOverflow();
				bang_bit(b, 3);
				nextByte = *myData;

				_timer.waitForOverflow();
				bang_bit(b, 2);
				// Why so complex, you ask? If this is a simple boolean, the compile
				// will optimize the check to be at the end.
				finished = (myData == last) ? 1 : 2;

				_timer.waitForOverflow();
				bang_bit(b, 1);

				_timer.waitForOverflow();
				bang_bit(b, 0);

				// Shave off one cycle: this uses sbrs instead of cp and breq
				if (finished & 1) {
					break;
				}
			}

			_timer.waitForOverflow();
			_timer.disable();
			pinOff(MHV_PIN_PARMS(dataPin));
		}
	}
}
;

}
#endif /* MHV_WS2811_H_ */
