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

#include <mhvlib/Pin.h>
#include <mhvlib/RGBLEDStrip.h>
#include <mhvlib/Timer.h>
#include <util/delay.h>

namespace mhvlib {

#define MHV_WS2811_TIMER_PARMS MHV_TIMER_PARMS, \
		(8 == bits) ? TimerMode::PWM_FAST_VAR_FREQ_8 : TimerMode::PWM_FAST_16

/**
 * Create a new WS2811 object to control a string of LED drivers
 * @tparam	dataPin...		the data pin for the LEDs (This must be the same pin that Output2 (OCRnB) is on)
 * @tparam	length		the number of LEDs in the string
 * @tparam  timer		the timer parameters
 */
template<MHV_DECLARE_PIN(dataPin), uint16_t length, MHV_DECLARE_TIMER>
class WS2811 : public RGBLEDStrip<length>, public PinImplementation<MHV_PIN_PARMS(dataPin)>,
		public TimerImplementation<MHV_WS2811_TIMER_PARMS> {
private:
	uint8_t _lowClocks;
	uint8_t _highClocks;

	/**
	 * Get the number of clocks for the high->low transition for a given bit
	 * @param data	the byte we are currently transmitting
	 * @param bit	the bit offset
	 */
	INLINE uint8_t getClocks(uint8_t data, uint8_t bit) {
//		return (data & (1 << bit)) ? _highClocks : _lowClocks;
//		return (data & (1 << bit)) ? 20 : 5;
		bool high = false;

		switch (bit) {
		case 7:
			high = data & (_BV(7));
			break;
		case 6:
			high = data & (_BV(6));
			break;
		case 5:
			high = data & (_BV(5));
			break;
		case 4:
			high = data & (_BV(4));
			break;
		case 3:
			high = data & (_BV(3));
			break;
		case 2:
			high = data & (_BV(2));
			break;
		case 1:
			high = data & (_BV(1));
			break;
		case 0:
			high = data & (_BV(0));
			break;
		}
		return (high) ? 14 : 7;
	}

	/**
	 * Send a bit down the line
	 * @param data	the byte we are currently transmitting
	 * @param bit	the bit offset
	 */
	INLINE void bangBit(uint8_t data, uint8_t bit) {
		uint8_t clocks = getClocks(data, bit);
		TimerImplementation<MHV_WS2811_TIMER_PARMS>::waitForOverflow();
		TimerImplementation<MHV_WS2811_TIMER_PARMS>::setOutput2(clocks);
		TimerImplementation<MHV_WS2811_TIMER_PARMS>::clearOverflow();
	}

public:
	/**
	 * Create a new driver for a string of WS2811 LEDs
	 */
	WS2811() {
		PinImplementation<MHV_PIN_PARMS(dataPin)>::setOutput();

//		_timer.setTimes(0.35f, 0.7f);
//		_lowClocks = _timer.getOutput1();
//		_highClocks = _timer.getOutput2();

//		_timer.setTimes(1.25f, 1.25f);
		TimerImplementation<MHV_WS2811_TIMER_PARMS>::setPrescaler(TimerPrescaler::PRESCALER_5_1);
		TimerImplementation<MHV_WS2811_TIMER_PARMS>::setTop(25);
		TimerImplementation<MHV_WS2811_TIMER_PARMS>::connectOutput2(TimerConnect::CLEAR);
	}

	/**
	 * Write the current buffer to the string of chips
	 */
	void flush() {
		uint8_t *myData = (uint8_t *)RGBLEDStrip<length>::_data;

		// Prepare first bit
		uint8_t nextByte = myData[0];
		uint8_t b = nextByte;
		uint8_t *last = myData + (length * sizeof(*RGBLEDStrip<length>::_data));

		TimerImplementation<MHV_WS2811_TIMER_PARMS>::setCurrent(0);
		TimerImplementation<MHV_WS2811_TIMER_PARMS>::setOutput2(getClocks(nextByte, 7));
		TimerImplementation<MHV_WS2811_TIMER_PARMS>::enable();

		myData++;
		bangBit(b, 6);
		nextByte = *myData;
		bangBit(b, 5);
		bangBit(b, 4);
		bangBit(b, 3);
		bangBit(b, 2);
		bangBit(b, 1);
		bangBit(b, 0);

		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
//			PinImplementation<MHV_PIN_PARMS(dataPin)>::on();

			while (true) {
				b = nextByte;
				bangBit(b, 7);
				myData++;
				bangBit(b, 6);
				nextByte = *myData;
				bangBit(b, 5);
				uint8_t finished = (myData == last) ? 1 : 2; // Why so complex, you ask? If this is a simple boolean, the compiler will optimize the check to be at the end.
				bangBit(b, 4);
				bangBit(b, 3);
				bangBit(b, 2);
				bangBit(b, 1);
				bangBit(b, 0);

				// Shave off one cycle: this uses sbrs instead of cp and breq
				if (finished & 1) {
					break;
				}
			}

			TimerImplementation<MHV_WS2811_TIMER_PARMS>::waitForOverflow();
			TimerImplementation<MHV_WS2811_TIMER_PARMS>::disable();
			PinImplementation<MHV_PIN_PARMS(dataPin)>::off();
		}
	}
}
;

}
#endif /* MHV_WS2811_H_ */
