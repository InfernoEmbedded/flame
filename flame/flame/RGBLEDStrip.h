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

#ifndef FLAME_RGBLEDSTRIP_H_
#define FLAME_RGBLEDSTRIP_H_

#include <flame/RGB.h>
#include <flame/GammaCorrect.h>
#include <string.h>

namespace flame {

/**
 * Create a new RGBLEDSTRIP object to control a string of LED drivers
 * @tparam	clock...	the clock pin for the LEDs
 * @tparam	data...		the data pin for the LEDs, mult be on the same port as the clock
 * @tparam	length		the number of LEDs in the string
 */
template <uint16_t length>
class RGBLEDStrip {
protected:
	RGB		_data[length];

public:
	/**
	 * Get a pixel
	 * @param	pixel	the pikel to get
	 */
	RGB &getPixel(uint16_t pixel) {
		return _data[pixel];
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

		chip->set(red, green, blue);
	}

	/**
	 * Set a pixel to a particular value
	 * @param	pixel	the pixel to set
	 * @param	value	the value to set
	 */
	void setPixel(uint16_t pixel, const RGB *value) {
		memcpy(_data + pixel, value, FLAME_BYTESIZEOF(*value));
	}

	/**
	 * Set a pixel to a particular value
	 * @param	pixel	the pixel to set
	 * @param	value	the value to set
	 */
	void setPixel(uint16_t pixel, const RGB &value) {
		memcpy(_data + pixel, &value, FLAME_BYTESIZEOF(value));
	}

	/**
	 * Set the strip to a particular value
	 * @param	red		the red value
	 * @param	blue	the blue value
	 * @param	green	the green value
	 */
	void setAll(uint8_t red, uint8_t green, uint8_t blue) {
		RGB newValue(red, green, blue);
		setAll(newValue);
	}

	/**
	 * Set the strip to a particular value
	 * @param	value	the value to set
	 */
	void setAll(const RGB &value) {
		for (uint16_t i = 0; i < length; i++) {
			memcpy(_data + i, &value, FLAME_BYTESIZEOF(value));
		}
	}

	/**
	 * Set a pixel to a gamma corrected value
	 * @param	pixel	the pixel to set
	 * @param	red		the red value
	 * @param	green	the green value
	 * @param	blue	the blue value
	 */
	void setPixelGamma(uint16_t pixel, uint8_t red, uint8_t green, uint8_t blue) {
		RGB *chip = _data + pixel;

		chip->setGamma(red, green, blue);
	}

	/**
	 * Set a pixel to a gamma corrected value
	 * @param	pixel	the pixel to set
	 * @param	value	the value to set
	 */
	void setPixelGamma(uint16_t pixel, const RGB *value) {
		RGB *chip = _data + pixel;

		chip->setGamma(value);
	}

	/**
	 * Set a pixel to a gamma corrected value
	 * @param	pixel	the pixel to set
	 * @param	value	the value to set
	 */
	void setPixelGamma(uint16_t pixel, const RGB &value) {
		RGB *chip = _data + pixel;

		chip->setGamma(value);
	}

	/**
	 * Set the strip to a gamma corrected value
	 * @param	red		the red value
	 * @param	blue	the blue value
	 * @param	green	the green value
	 */
	void setAllGamma(uint8_t red, uint8_t green, uint8_t blue) {
		RGB newValue;
		newValue.setGamma(red, green, blue);

		for (uint16_t i = 0; i < length; i++) {
			memcpy(_data + i, &newValue, FLAME_BYTESIZEOF(newValue));
		}
	}

	/**
	 * Set the strip to a gamma corrected value
	 * @param	value	the value to set
	 */
	void setAllGamma(const RGB &value) {
		RGB newValue;
		newValue.setGamma(value);

		for (uint16_t i = 0; i < length; i++) {
			memcpy(_data + i, &newValue, FLAME_BYTESIZEOF(newValue));
		}
	}

	/**
	 * Write the current buffer to the string of LEDs
	 */
	virtual void flush() {}

	/**
	 * Rotate the string by 1 pixel
	 * @param	forwards	true for forwards, false for backwards
	 */
	void rotate(bool forwards) {
		RGB temp;

		if (forwards) {
			memcpy(&temp, _data + length - 1, FLAME_BYTESIZEOF(temp));
			memmove(_data + 1, _data, FLAME_BYTESIZEOF(*_data) * (length - 1));
			memcpy(_data, &temp, FLAME_BYTESIZEOF(temp));
		} else {
			memcpy(&temp, _data, FLAME_BYTESIZEOF(temp));
			memcpy(_data, _data + 1,  FLAME_BYTESIZEOF(*_data) * (length - 1));
			memcpy(_data, &temp, FLAME_BYTESIZEOF(temp));
		}
	}
};

}
#endif /* FLAME_RGBLEDSTRIP_H_ */
