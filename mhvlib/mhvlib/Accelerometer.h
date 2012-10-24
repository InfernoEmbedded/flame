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

#ifndef MHV_ACCELEROMETER_H_
#define MHV_ACCELEROMETER_H_

#include <mhvlib/io.h>
#include <mhvlib/ADC.h>

namespace mhvlib {

// Forward declaration of Accelerometer
class Accelerometer;

enum AccelerometerChannel {
	X,
	Y,
	Z,
	MAGNITUDE
};

class AccelerometerListener {
	/**
	 * Called when a sample is ready
	 * @param accelerometer	the accelerometer whose sample is ready
	 */
	virtual void sampleIsReady(Accelerometer &accelerometer) {};

	/**
	 * Called when an acceleration limit is reached
	 * @param accelerometer	the accelerometer whose limit was reached
	 * @param which			which limit was reached
	 */
	virtual void limitReached(Accelerometer &accelerometer, AccelerometerChannel which) {};
};

/**
 * Accelerometer Interface
 */
class Accelerometer {
protected:
	AccelerometerListener	*_listener;
	int16_t					_x;
	int16_t					_y;
	int16_t					_z;
	int16_t					_limitX;
	int16_t					_limitY;
	int16_t					_limitZ;
	int32_t					_limitMagnitudeSquared;

public:
	Accelerometer() :
		_listener(NULL) {}

	/**
	 * Initiate a sampling of the accelerometer
	 */
	virtual void sample() =0;

	/**
	 * Check if a sample is ready
	 * @return true if the sample is ready
	 */
	virtual void isSampleReady() =0;

	/**
	 * Register a listener
	 * @param	listener	the listener to notify
	 */
	void registerListener(AccelerometerListener &listener) {
		_listener = &listener;
	}

	/**
	 * Deregister the listener
	 */
	void deregisterListener() {
		_listener = NULL;
	}

	/**
	 * Handle any pending events for the accelerometer
	 */
	void handleEvents() {
		if (isSampleReady()) {
			if (_limitMagnitudeSquared) {
				int32_t magnitudeSquared =
						(int32_t)_x * (int32_t)_x +
						(int32_t)_y * (int32_t)_y +
						(int32_t)_z * (int32_t)_z;

				if (magnitudeSquared >= _limitMagnitudeSquared) {
					_listener->limitReached(this, AccelerometerChannel::MAGNITUDE);
				}
			}

			if (_limitX && abs(X) >= _limitX) {
				_listener->limitReached(this, AccelerometerChannel::X);
			}
			if (_limitX && abs(Y) >= _limitY) {
				_listener->limitReached(this, AccelerometerChannel::Y);
			}
			if (_limitX && abs(Z) >= _limitZ) {
				_listener->limitReached(this, AccelerometerChannel::Z);
			}
		}
	}

}; // class Accelerometer

} // namespace mhvlib

#endif /* MHV_ACCELEROMETER_H_ */
