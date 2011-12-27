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


#include <MHV_WS2801.h>

/**
 * Create a new driver for a string of WS2801 chips
 * See the datasheet at: http://www.adafruit.com/datasheets/WS2801.pdf
 *
 * @param	buf			memory to buffer the data into
 * @param	length		the length of the string of controllers
 * @param	shifter		the shifter to use to speak to the string
 */
MHV_WS2801::MHV_WS2801(MHV_RGB buf[], uint16_t length, MHV_Shifter &shifter) :
		_length(length),
		_data(buf),
		_shifter(shifter) {}

/**
 * Set a pixel to a particular value
 * @param	pixel	the pixel to set
 * @param	red		the red value
 * @param	blue	the blue value
 * @param	green	the green value
 */
void MHV_WS2801::setPixel(uint16_t pixel, uint8_t red, uint8_t green, uint8_t blue) {
	MHV_RGB *chip = _data + pixel;

	chip->red = red;
	chip->green = green;
	chip->blue = blue;
}

/**
 * Set a pixel to a particular value
 * @param	pixel	the pixel to set
 * @param	value	the value to set
 */
void MHV_WS2801::setPixel(uint16_t pixel, MHV_RGB *value) {
	mhv_memcpy(_data + pixel, value, MHV_BYTESIZEOF(*value));
}

/**
 * Set the strip to a particular value
 * @param	red		the red value
 * @param	blue	the blue value
 * @param	green	the green value
 */
void MHV_WS2801::setAll(uint8_t red, uint8_t green, uint8_t blue) {
	MHV_RGB *chip;

	for (uint16_t i = 0; i < _length; i++) {
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
void MHV_WS2801::setAll(MHV_RGB *value) {
	for (uint16_t i = 0; i < _length; i++) {
		mhv_memcpy(_data + i, value, MHV_BYTESIZEOF(*value));
	}
}

/**
 * Set a pixel to a gamma corrected value
 * @param	pixel	the pixel to set
 * @param	red		the red value
 * @param	blue	the blue value
 * @param	green	the green value
 */
void MHV_WS2801::setPixelGamma(uint16_t pixel, uint8_t red, uint8_t green, uint8_t blue) {
	MHV_RGB *chip = _data + pixel;

	chip->red = mhv_precalculatedGammaCorrect(red);
	chip->green = mhv_precalculatedGammaCorrect(green);
	chip->blue = mhv_precalculatedGammaCorrect(blue);
}

/**
 * Set a pixel to a gamma corrected value
 * @param	pixel	the pixel to set
 * @param	value	the value to set
 */
void MHV_WS2801::setPixelGamma(uint16_t pixel, MHV_RGB *value) {
	MHV_RGB *chip = _data + pixel;

	chip->red = mhv_precalculatedGammaCorrect(value->red);
	chip->green = mhv_precalculatedGammaCorrect(value->green);
	chip->blue = mhv_precalculatedGammaCorrect(value->blue);
}

/**
 * Set the strip to a gamma corrected value
 * @param	red		the red value
 * @param	blue	the blue value
 * @param	green	the green value
 */
void MHV_WS2801::setAllGamma(uint8_t red, uint8_t green, uint8_t blue) {
	MHV_RGB *chip;

	for (uint16_t i = 0; i < _length; i++) {
		chip = _data + i;

		chip->red = mhv_precalculatedGammaCorrect(red);
		chip->green = mhv_precalculatedGammaCorrect(green);
		chip->blue = mhv_precalculatedGammaCorrect(blue);
	}
}

/**
 * Set the strip to a gamma corrected value
 * @param	value	the value to set
 */
void MHV_WS2801::setAllGamma(MHV_RGB *value) {
	MHV_RGB newValue = {mhv_precalculatedGammaCorrect(value->red),
			mhv_precalculatedGammaCorrect(value->green),
			mhv_precalculatedGammaCorrect(value->blue)};

	for (uint16_t i = 0; i < _length; i++) {
		mhv_memcpy(_data + i, &newValue, MHV_BYTESIZEOF(newValue));
	}
}


/**
 * Write the current buffer to the string of chips
 */
void MHV_WS2801::flush() {
	_shifter.shiftOut((uint8_t *)_data, MHV_BYTESIZEOF(*_data) * _length);
}

/**
 * Rotate the string by 1 pixel
 * @param	forwards	true for forwards, false for backwards
 */
void MHV_WS2801::rotate(bool forwards) {
	MHV_RGB temp;

	if (forwards) {
		mhv_memcpy(&temp, _data + _length - 1, MHV_BYTESIZEOF(temp));
		mhv_memcpyTailFirst(_data + 1, _data, MHV_BYTESIZEOF(*_data), _length - 1);
		mhv_memcpy(_data, &temp, MHV_BYTESIZEOF(temp));
	} else {
		mhv_memcpy(&temp, _data, MHV_BYTESIZEOF(temp));
		mhv_memcpy(_data, _data + 1,  MHV_BYTESIZEOF(*_data), _length - 1);
		mhv_memcpy(_data, &temp, MHV_BYTESIZEOF(temp));
	}
}
