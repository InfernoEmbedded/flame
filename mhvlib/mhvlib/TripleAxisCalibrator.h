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

#ifndef TRIPLEAXISCALIBRATOR_H_
#define TRIPLEAXISCALIBRATOR_H_

#include <avr/pgmspace.h>
#include <mhvlib/io.h>
#include <mhvlib/Device_TX.h>
#include <mhvlib/TripleAxisSensor.h>

namespace mhvlib {

enum TripleAxisCalibratorState {
	SETUP,
	BOTTOM_DOWN,
	TOP_DOWN,
	LEFT_DOWN,
	RIGHT_DOWN,
	FRONT_DOWN,
	BACK_DOWN
};

class TripleAxisCalibrator : public TripleAxisSensorListener {
protected:
	TRIPLEAXISSENSOR_RAW_READING	_previousSample = {{0, 0, 0}};
	uint8_t							_goodSamples = 0;
	PGM_P							_lastMessage = NULL;
	uint8_t							_samplesToTake = 32; // the number of samples to take at each calibration point
	uint8_t							_currentSamples;
	enum TripleAxisCalibratorState	_state = SETUP;
	TripleAxisSensor				&_sensor;
	Device_TX 						&_output;
	TripleAxisSensorListener		*_savedListener = NULL;

	bool isSampleGood(const TRIPLEAXISSENSOR_RAW_READING &sample, PGM_P *feedback);

public:
	TripleAxisCalibrator(TripleAxisSensor &sensor, Device_TX &output);
	bool pushSample(const TRIPLEAXISSENSOR_RAW_READING &sample, PGM_P *feedback);
	void calibrate();

	// Implement TripleAxisSensorListener
	void sampleIsReady(TripleAxisSensor &sensor);
	void limitReached(TripleAxisSensor &sensor, TripleAxisSensorChannel which);

	/**
	 * Add a sample to the calibrator
	 * @param	sample		the sample to send
	 */
	virtual void addObservation(const TRIPLEAXISSENSOR_RAW_READING &sample) =0;

	/**
	 * Write the offsets and scales to the sensor
	 * @param	sensor		the sensor to write to (should be the same sensor that is pushing the sample
	 */
	virtual void calibrateSensor(TripleAxisSensor &sensor) =0;




}; // class TripleAxisCalibrator
} // namespace mhvlib

#endif /* _3AXISCALIBRATOR_H_ */
