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
#include <mhvlib/EEPROM.h>
#include <inttypes.h>
#include <math.h>

namespace mhvlib {

// Forward declaration of Accelerometer
class Accelerometer;

enum AccelerometerChannel {
	X,
	Y,
	Z,
	MAGNITUDE
};

typedef struct Float3Axis ACCELEROMETER_READING;
typedef struct Int3Axis ACCELEROMETER_OFFSETS;
typedef struct Int3Axis ACCELEROMETER_LIMITS;
typedef struct Float3Axis ACCELEROMETER_SCALING;

class AccelerometerListener {
	/**
	 * Called when a sample is ready
	 * @param accelerometer	the accelerometer whose sample is ready
	 */
	virtual void sampleIsReady(Accelerometer *accelerometer) {};

	/**
	 * Called when an acceleration limit is reached
	 * @param accelerometer	the accelerometer whose limit was reached
	 * @param which			which limit was reached
	 */
	virtual void limitReached(Accelerometer *accelerometer, AccelerometerChannel which) {};
};

/**
 * Accelerometer Interface
 */
class Accelerometer : EEPROMListener {
protected:
	AccelerometerListener			*_listener = NULL;
	ACCELEROMETER_READING			_valueTemp = (0.0f, 0.0f, 0.0f);
	ACCELEROMETER_READING			_valueOut = (0.0f, 0.0f, 0.0f);
	ACCELEROMETER_LIMITS			_limit = (0, 0, 0);
	int32_t							_limitMagnitudeSquared = 0;
	ACCELEROMETER_OFFSETS			_offsets = (0, 0, 0);
	ACCELEROMETER_SCALING			_scaling = (0.0f, 0.0f, 0.0f);

	/**
	 * Push a sample from the driver into this class
	 * @param which		Which channel to push
	 * @param value		the updated value
	 */
	void pushSample(AccelerometerChannel which, int16_t value);

public:
	Accelerometer();

	/**
	 * Initiate a sampling of the accelerometer
	 */
	virtual void sample() =0;

	/**
	 * Check if a sample is ready
	 * @return true if the sample is ready
	 */
	virtual bool isSampleReady() =0;

	void saveCalibration(EEPROM &eeprom, uint16_t address);
	void eepromDone(EEPROM *eeprom, uint16_t address, void *buffer);
	bool loadCalibration(EEPROM &eeprom, uint16_t address);
	void setOffsets(ACCELEROMETER_OFFSETS *offsets);
	void setOffsets(int16_t x, int16_t y, int16_t z);
	void setScale(ACCELEROMETER_SCALING *scales);
	void setScale(float x, float y, float z);
	void registerListener(AccelerometerListener &listener);
	void deregisterListener();
	void getValues(float *x, float *y, float *z);
	void getValues(ACCELEROMETER_READING *value);
	float magnitudeSquared();
	float magnitude();
	void handleEvents();
	void setLimit(AccelerometerChannel which, float limit);
	void getLimits(ACCELEROMETER_READING *limits);
}; // class Accelerometer

} // namespace mhvlib

#endif /* MHV_ACCELEROMETER_H_ */
