/* Copyright (c) 2012, Make, Hack, Void Inc
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


#ifndef MHV_RGB_H_
#define MHV_RGB_H_

#include <mhvlib/io.h>
#include <mhvlib/GammaCorrect.h>

namespace mhvlib {

/**
 * Set the colour order of the MHV_RGB struct
 * 	RGB	1
 *	RBG	2
 *	GRB	3
 *	GBR	4
 *	BGR	5
 *	BRG	6
 */

#define MHV_RGB_ORDER_RGB	1
#define MHV_RGB_ORDER_RBG	2
#define MHV_RGB_ORDER_GRB	3
#define MHV_RGB_ORDER_GBR	4
#define MHV_RGB_ORDER_BGR	5
#define MHV_RGB_ORDER_BRG	6

#ifndef MHV_RGB_ORDER
#define MHV_RGB_ORDER	MHV_RGB_ORDER_RGB
#endif

union rgb {
	int8_t			value[3];
#if MHV_RGB_ORDER == MHV_RGB_ORDER_RGB
	struct {
		uint8_t	red;
		uint8_t	green;
		uint8_t	blue;
	} channel;
};

enum rgbChannel {
	RED	= 0,
	GREEN = 1,
	BLUE = 2
};
#elif MHV_RGB_ORDER == MHV_RGB_ORDER_RBG
	struct {
		uint8_t	red;
		uint8_t	blue;
		uint8_t	green;
	} channel;
};

enum rgbChannel {
	RED	= 0,
	GREEN = 2,
	BLUE = 1
};
#elif MHV_RGB_ORDER == MHV_RGB_ORDER_GRB
	struct {
		uint8_t	green;
		uint8_t	red;
		uint8_t	blue;
	} channel;
};

enum rgbChannel {
	RED	= 1,
	GREEN = 0,
	BLUE = 2
};
#elif MHV_RGB_ORDER == MHV_RGB_ORDER_GBR
	struct {
		uint8_t	green;
		uint8_t	blue;
		uint8_t	red;
	} channel;
};

enum rgbChannel {
	RED	= 2,
	GREEN = 0,
	BLUE = 1
};
#elif MHV_RGB_ORDER == MHV_RGB_ORDER_BGR
	struct {
		uint8_t	blue;
		uint8_t	green;
		uint8_t	red;
	} channel;
};

enum rgbChannel {
	RED	= 2,
	GREEN = 1,
	BLUE = 0
};
#elif MHV_RGB_ORDER == MHV_RGB_ORDER_BRG
	struct {
		uint8_t	blue;
		uint8_t	red;
		uint8_t	green;
	} channel;
};

enum rgbChannel {
	RED	= 1,
	GREEN = 2,
	BLUE = 0
};
#endif

typedef enum rgbChannel RGB_CHANNEL;

class RGB {
protected:
	union rgb	_data;

	/**
	 * Fade from an initial value to a final value
	 * @param initial	value to fade from
	 * @param final 	value to fade to
	 * @param iteration	The current iteration
	 * @param total		total number of iterations to fade across
	 * @return the faded value
	 */
	uint8_t fade(uint8_t initial, uint8_t final, int16_t iteration, int16_t total) {
		if (0 == total || iteration >= total) {
			return final;
		}

		if (iteration <= 0) {
			return initial;
		}

		int32_t di = iteration;
		di *= final - initial;
		di /= total;

		return initial + di;
	}

public:
	/**
	 * Constructor
	 * @param red	the red value
	 * @param green	the green value
	 * @param blue	the blue value
	 */
	RGB(uint8_t red, uint8_t green, uint8_t blue) {
		_data.channel.red = red;
		_data.channel.green = green;
		_data.channel.blue = blue;
	}

	/**
	 * Constructor
	 * @param value	an RGB to take the value of
	 */
	RGB(const RGB &value) {
		_data.channel.red = value._data.channel.red;
		_data.channel.green = value._data.channel.green;
		_data.channel.blue = value._data.channel.blue;
	}

	/**
	 * Constructor
	 * @param value	an RGB to take the value of
	 */
	RGB(const RGB *value) {
		_data.channel.red = value->_data.channel.red;
		_data.channel.green = value->_data.channel.green;
		_data.channel.blue = value->_data.channel.blue;
	}

	/**
	 * Constructor
	 */
	RGB() {
		_data.channel.red = 0;
		_data.channel.green = 0;
		_data.channel.blue = 0;
	}

	/**
	 * Fade from an initial value to a final value (should be called repeatedly with decreasing iterations)
	 * @param final 	value to fade to
	 * @param out 		the output value
	 * @param currentIteration	current iteration
	 * @param iterationsLeft	total number of iterations remaining to fade across
	 */
	void fadeTo(const RGB &final, uint16_t currentIteration, uint16_t iterations) {
		_data.channel.red = fade(_data.channel.red, final._data.channel.red,
				currentIteration, iterations);
		_data.channel.green = fade(_data.channel.green, final._data.channel.green,
				currentIteration, iterations);
		_data.channel.blue = fade(_data.channel.blue, final._data.channel.blue,
				currentIteration, iterations);
	}

	/**
	 * Set a value
	 * @param red	the red value
	 * @param green	the green value
	 * @param blue	the blue value
	 */
	void set (uint8_t red, uint8_t green, uint8_t blue) {
		_data.channel.red = red;
		_data.channel.green = green;
		_data.channel.blue = blue;
	}

	/**
	 * Override the = operator
	 * @param rhs	the value to assign to the left hand side
	 */
	RGB &operator=(const RGB &rhs) {
		set(rhs);
		return *this;
	}

	/**
	 * Set a value and gamma correct
	 * @param red	the red value
	 * @param green	the green value
	 * @param blue	the blue value
	 */
	void setGamma (uint8_t red, uint8_t green, uint8_t blue) {
		_data.channel.red = precalculatedGammaCorrect(red);
		_data.channel.green = precalculatedGammaCorrect(green);
		_data.channel.blue = precalculatedGammaCorrect(blue);
	}

	/**
	 * Set a value
	 * @param value		the new value
	 */
	void set (const RGB &value) {
		_data.channel.red = value._data.channel.red;
		_data.channel.green = value._data.channel.green;
		_data.channel.blue = value._data.channel.blue;
	}

	/**
	 * Get the value for a channel
	 * @param channel	the channel to get
	 */
	uint8_t get (RGB_CHANNEL channel) {
		return _data.value[channel];
	}


	/**
	 * Set a value and gamma correct
	 * @param value		the new value
	 */
	void setGamma (const RGB &value) {
		_data.channel.red = precalculatedGammaCorrect(value._data.channel.red);
		_data.channel.green = precalculatedGammaCorrect(value._data.channel.green);
		_data.channel.blue = precalculatedGammaCorrect(value._data.channel.blue);
	}

	/**
	 * Set a value
	 * @param value		the new value
	 */
	void set (const RGB *value) {
		_data.channel.red = value->_data.channel.red;
		_data.channel.green = value->_data.channel.green;
		_data.channel.blue = value->_data.channel.blue;
	}

	/**
	 * Set a value for a channel
	 * @param channel	the channel
	 * @param value		the new value
	 */
	void set (RGB_CHANNEL channel, uint8_t value) {
		_data.value[channel] = value;
	}

	/**
	 * Set a value and gamma correct
	 * @param value		the new value
	 */
	void setGamma (const RGB *value) {
		_data.channel.red = precalculatedGammaCorrect(value->_data.channel.red);
		_data.channel.green = precalculatedGammaCorrect(value->_data.channel.green);
		_data.channel.blue = precalculatedGammaCorrect(value->_data.channel.blue);
	}

	/**
	 * Gamma correct the current value
	 */
	void gammaCorrect() {
		_data.channel.red = precalculatedGammaCorrect(_data.channel.red);
		_data.channel.green = precalculatedGammaCorrect(_data.channel.green);
		_data.channel.blue = precalculatedGammaCorrect(_data.channel.blue);

	}

	/**
	 * Inverse gamma correct the current value
	 */
	void inverseGammaCorrect() {
		_data.channel.red = precalculatedInverseGammaCorrect(_data.channel.red);
		_data.channel.green = precalculatedInverseGammaCorrect(_data.channel.green);
		_data.channel.blue = precalculatedInverseGammaCorrect(_data.channel.blue);
	}
};

}

#endif /* MHV_RGB_H_ */
