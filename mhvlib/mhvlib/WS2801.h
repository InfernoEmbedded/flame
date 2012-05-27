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
#include <mhvlib/RGB.h>
#include <mhvlib/GammaCorrect.h>
#include <mhvlib/Shifter.h>

namespace mhvlib_bsd {

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

/**
 * Create a new WS2801 object to control a string of LED drivers
 * @tparam	clock...	the clock pin for the LEDs
 * @tparam	data...		the data pin for the LEDs, mult be on the same port as the clock
 * @tparam	length		the number of LEDs in the string
 */
template <MHV_DECLARE_PIN(clock), MHV_DECLARE_PIN(data), uint16_t length>
class WS2801 {
private:
	RGB		_data[length];

	ShifterImplementation<MHV_PIN_PARMS(clock), MHV_PIN_PARMS(data), true, true, 2>
				_shifter;

public:
	/**
	 * Create a new driver for a string of WS2801 LEDs
	 */
	WS2801() {
		setOutput(MHV_PIN_PARMS(clock));
		setOutput(MHV_PIN_PARMS(data));
	}

	/**
	 * Set a pixel to a particular value
	 * @param	pixel	the pixel to set
	 * @param	red		the red value
	 * @param	blue	the blue value
	 * @param	green	the green value
	 */
	void setPixel(uint16_t pixel, uint8_t red, uint8_t green, uint8_t blue) {
		RGB *chip = _data + pixel;

		chip->red = red;
		chip->green = green;
		chip->blue = blue;
	}

	/**
	 * Set a pixel to a particular value
	 * @param	pixel	the pixel to set
	 * @param	value	the value to set
	 */
	void setPixel(uint16_t pixel, RGB *value) {
		memcpy(_data + pixel, value, MHV_BYTESIZEOF(*value));
	}

	/**
	 * Set the strip to a particular value
	 * @param	red		the red value
	 * @param	blue	the blue value
	 * @param	green	the green value
	 */
	void setAll(uint8_t red, uint8_t green, uint8_t blue) {
		RGB *chip;

		for (uint16_t i = 0; i < length; i++) {
			chip = _data + i;

			chip->red = red;
			chip->green = green;
			chip->blue = blue;
		}
	}

	/**
	 * Set the strip to a particular value
	 * @param	value	the value to set
	 */
	void setAll(RGB *value) {
		for (uint16_t i = 0; i < length; i++) {
			memcpy(_data + i, value, MHV_BYTESIZEOF(*value));
		}
	}

	/**
	 * Set a pixel to a gamma corrected value
	 * @param	pixel	the pixel to set
	 * @param	red		the red value
	 * @param	blue	the blue value
	 * @param	green	the green value
	 */
	void setPixelGamma(uint16_t pixel, uint8_t red, uint8_t green, uint8_t blue) {
		RGB *chip = _data + pixel;

		chip->red = precalculatedGammaCorrect(red);
		chip->green = precalculatedGammaCorrect(green);
		chip->blue = precalculatedGammaCorrect(blue);
	}

	/**
	 * Set a pixel to a gamma corrected value
	 * @param	pixel	the pixel to set
	 * @param	value	the value to set
	 */
	void setPixelGamma(uint16_t pixel, RGB *value) {
		RGB *chip = _data + pixel;

		chip->red = precalculatedGammaCorrect(value->red);
		chip->green = precalculatedGammaCorrect(value->green);
		chip->blue = precalculatedGammaCorrect(value->blue);
	}

	/**
	 * Set the strip to a gamma corrected value
	 * @param	red		the red value
	 * @param	blue	the blue value
	 * @param	green	the green value
	 */
	void setAllGamma(uint8_t red, uint8_t green, uint8_t blue) {
		RGB *chip;

		for (uint16_t i = 0; i < length; i++) {
			chip = _data + i;

			chip->red = precalculatedGammaCorrect(red);
			chip->green = precalculatedGammaCorrect(green);
			chip->blue = precalculatedGammaCorrect(blue);
		}
	}

	/**
	 * Set the strip to a gamma corrected value
	 * @param	value	the value to set
	 */
	void setAllGamma(RGB *value) {
		RGB newValue = {precalculatedGammaCorrect(value->red),
				precalculatedGammaCorrect(value->green),
				precalculatedGammaCorrect(value->blue)};

		for (uint16_t i = 0; i < length; i++) {
			memcpy(_data + i, &newValue, MHV_BYTESIZEOF(newValue));
		}
	}


	/**
	 * Write the current buffer to the string of chips
	 */
	void flush() {
		_shifter.shiftOut((uint8_t *)_data, MHV_BYTESIZEOF(*_data), length);
	}

	/**
	 * Rotate the string by 1 pixel
	 * @param	forwards	true for forwards, false for backwards
	 */
	void rotate(bool forwards) {
		RGB temp;

		if (forwards) {
			memCopy(&temp, _data + length - 1, MHV_BYTESIZEOF(temp));
			memCopyTailFirst(_data + 1, _data, MHV_BYTESIZEOF(*_data), length - 1);
			memCopy(_data, &temp, MHV_BYTESIZEOF(temp));
		} else {
			memCopy(&temp, _data, MHV_BYTESIZEOF(temp));
			memCopy(_data, _data + 1,  MHV_BYTESIZEOF(*_data), length - 1);
			memCopy(_data, &temp, MHV_BYTESIZEOF(temp));
		}
	}

};

}
#endif /* MHV_WS2801_H_ */
