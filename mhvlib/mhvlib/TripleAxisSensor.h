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

#ifndef MHV_TRIPLEAXISSENSOR_H_
#define MHV_TRIPLEAXISSENSOR_H_

#include <mhvlib/io.h>
#include <mhvlib/EEPROM.h>
#include <mhvlib/Timer.h>
#include <inttypes.h>

#include <mhvlib/Device_TX.h>


namespace mhvlib {

// Forward declaration of Sensor
class TripleAxisSensor;

enum TripleAxisSensorChannel {
	X = 0,
	Y = 1,
	Z = 2,
	MAGNITUDE
};

typedef union Float3Axis TRIPLEAXISSENSOR_READING;
typedef union Int3Axis TRIPLEAXISSENSOR_RAW_READING;
typedef union Int3Axis TRIPLEAXISSENSOR_OFFSETS;
typedef union Float3Axis TRIPLEAXISSENSOR_LIMITS;
typedef union Float3Axis TRIPLEAXISSENSOR_SCALING;

class TripleAxisSensorListener {
public:
	/**
	 * Called when a sample is ready
	 * @param sensor	the sensor whose sample is ready
	 */
	virtual void sampleIsReady(TripleAxisSensor &sensor) =0;

	/**
	 * Called when an acceleration limit is reached
	 * @param sensor	the sensor whose limit was reached
	 * @param which			which limit was reached
	 */
	virtual void limitReached(TripleAxisSensor &sensor, TripleAxisSensorChannel which) =0;
};

/**
 * 3 Axis Sensor Interface
 */
class TripleAxisSensor : public EEPROMListener, public TimerListener {
protected:
	TripleAxisSensorListener			*_listener = NULL;
	TRIPLEAXISSENSOR_READING			_valueTemp = {{0.0f, 0.0f, 0.0f}};
	TRIPLEAXISSENSOR_READING			_valueOut = {{0.0f, 0.0f, 0.0f}};
	TRIPLEAXISSENSOR_LIMITS				_limit = {{0.0f, 0.0f, 0.0f}};
	float								_limitMagnitudeSquared = 0.0f;
	TRIPLEAXISSENSOR_OFFSETS			_offsets = {{0, 0, 0}};
	TRIPLEAXISSENSOR_SCALING			_scaling = {{1.0f, 1.0f, 1.0f}};
	TRIPLEAXISSENSOR_RAW_READING		_raw = {{0, 0, 0}};
	bool								_calibrated = false;

	/**
	 * Push a sample from the driver into this class
	 * @param which		Which channel to push
	 * @param value		the updated value
	 */
	void pushSample(TripleAxisSensorChannel which, int16_t value);

public:
	/**
	 * Initiate a sampling of the sensor
	 */
	virtual void sample() =0;

	/**
	 * Check if a sample is ready
	 * @return true if the sample is ready
	 */
	virtual bool isSampleReady() =0;

	void saveCalibration(EEPROM &eeprom, uint16_t address);
	void eepromDone(EEPROM &eeprom, uint16_t address, void *buffer);
	bool loadCalibration(EEPROM &eeprom, uint16_t address);
	void setOffsets(const TRIPLEAXISSENSOR_OFFSETS &offsets);
	void setOffsets(int16_t x, int16_t y, int16_t z);
	void setScale(const TRIPLEAXISSENSOR_SCALING &scales);
	void setScale(float x, float y, float z);
	void getParameters(TRIPLEAXISSENSOR_OFFSETS *offsets, TRIPLEAXISSENSOR_SCALING *scales);
	void registerListener(TripleAxisSensorListener &listener);
	void deregisterListener();
	TripleAxisSensorListener *getListener();
	void getValue(float *x, float *y, float *z);
	void getValue(TRIPLEAXISSENSOR_READING *value);
	void getRawValue(TRIPLEAXISSENSOR_RAW_READING *value);
	float magnitudeSquared();
	float magnitude();
	void handleEvents();
	void setLimit(TripleAxisSensorChannel which, float limit);
	void getLimits(TRIPLEAXISSENSOR_READING *limits);
	void alarm();
	bool isCalibrated();
	void setCalibrated(bool calibrated);
}; // class TripleAxisSensor

} // namespace mhvlib

#endif /* MHV_TRIPLEAXISSENSOR_H_ */
