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

#include <mhvlib/TripleAxisSensor.h>
#include <math.h>
#include <mhvlib/Device_TX.h>
#include <string.h>

namespace mhvlib {
/**
 * Push a sample from the driver into this instance
 * @param which		Which channel to push
 * @param value		the updated value
 */
void TripleAxisSensor::pushSample(TripleAxisSensorChannel which, int16_t value) {
	if (TripleAxisSensorChannel::MAGNITUDE == which) {
		return;
	}

	_raw.value[(uint8_t)which] = value;
	_valueTemp.value[(uint8_t)which] = (value - _offsets.value[(uint8_t)which]) / _scaling.value[(uint8_t)which];
}

/**
 * Get the raw values
 * @param value	the Int3Axis to populate
 */
void TripleAxisSensor::getRawValue(TRIPLEAXISSENSOR_RAW_READING *value) {
	value->axis.x = _raw.axis.x;
	value->axis.y = _raw.axis.y;
	value->axis.z = _raw.axis.z;
}

/**
 * Save the current calibration data to EEPROM
 * @param eeprom	the EEPROM access instance
 * @param address	the address to save to (18 bytes)
 */
void TripleAxisSensor::saveCalibration(EEPROM &eeprom, uint16_t address) {
	eeprom.write((void *)&_offsets, address, sizeof(_offsets), this);
	// continued in eepromDone
}

/**
 * Called when an EEPROM write is finished
 * @param eeprom	the EEPROM access instance
 * @param address	the EEPROM address that the data was written to
 * @param buffer	the pointer that the data was wriiten from
 */
void TripleAxisSensor::eepromDone(EEPROM &eeprom, uint16_t address, void *buffer) {
	if (buffer == &_offsets) {
		// now write the scaling factors
		eeprom.write((void *)&_scaling, address + sizeof(_offsets),
				sizeof(_scaling), this);
	} else if (buffer == &_scaling) {
		// write the checksum
		uint16_t origAddress = address - sizeof(_offsets);
		uint16_t crc = eeprom.crc(origAddress, sizeof(_offsets) + sizeof(_scaling));
		eeprom.write((void *)&crc, address + sizeof(_scaling), sizeof(crc), NULL);
	}
}

/**
 * Load calibration data
 * @param eeprom	the EEPROM access instance
 * @param address	the EEPROM address that the data was written to
 * @return false on success, true if the device needs to be calibrated
 */
bool TripleAxisSensor::loadCalibration(EEPROM &eeprom, uint16_t address) {
	uint16_t crc = eeprom.crc(address, sizeof(_offsets) + sizeof(_scaling));
	uint16_t storedCRC;

	eeprom.busyRead(&storedCRC, address + sizeof(_offsets) + sizeof(_scaling),
			sizeof(storedCRC));

	if (crc != storedCRC) {
		return true;
	}

	eeprom.busyRead(&_offsets, address, sizeof(_offsets));
	eeprom.busyRead(&_scaling, address + sizeof(_offsets), sizeof(_scaling));

	return false;
}

/**
 * Set the offsets (pre-scaling) for the accelerometer
 * @param offsets	the new values for the offsets
 */
void TripleAxisSensor::setOffsets(const TRIPLEAXISSENSOR_OFFSETS &offsets) {
	memcpy(&_offsets, &offsets, sizeof(offsets));
}

/**
 * Set the offsets (pre-scaling) for the accelerometer
 * @param x		the X offset
 * @param y		the Y offset
 * @param z		the Z offset
 */
void TripleAxisSensor::setOffsets(int16_t x, int16_t y, int16_t z) {
	_offsets.axis.x = x;
	_offsets.axis.y = y;
	_offsets.axis.z = z;
}

/**
 * Set the scaling for the accelerometer
 * @param scales	the new values for scaling
 */
void TripleAxisSensor::setScale(const TRIPLEAXISSENSOR_SCALING &scales) {
	memcpy(&_scaling, &scales, sizeof(scales));
}

/**
 * Set the scaling for the accelerometer
 * @param x		the X scale
 * @param y		the Y scale
 * @param z		the Z scale
 */
void TripleAxisSensor::setScale(float x, float y, float z) {
	_scaling.axis.x = x;
	_scaling.axis.y = y;
	_scaling.axis.z = z;
}

/**
 * Get the function parameters for this sensor
 * @param offsets	the offsets for each axis
 * @param scales	the scale for each axis
 */
void TripleAxisSensor::getParameters(TRIPLEAXISSENSOR_OFFSETS *offsets, TRIPLEAXISSENSOR_SCALING *scales) {
	offsets->axis.x = _offsets.axis.x;
	offsets->axis.y = _offsets.axis.y;
	offsets->axis.z = _offsets.axis.z;

	scales->axis.x = _scaling.axis.x;
	scales->axis.y = _scaling.axis.y;
	scales->axis.z = _scaling.axis.z;
}

/**
 * Register a listener
 * @param	listener	the listener to notify
 */
void TripleAxisSensor::registerListener(TripleAxisSensorListener &listener) {
	_listener = &listener;
}

/**
 * Get the current listener
 * @return the listener
 */
TripleAxisSensorListener *TripleAxisSensor::getListener() {
	return _listener;
}

/**
 * Deregister the listener
 */
void TripleAxisSensor::deregisterListener() {
	_listener = NULL;
}

/**
 * Get the current values
 */
void TripleAxisSensor::getValue(float *x, float *y, float *z) {
		*x = _valueOut.axis.x;
		*y = _valueOut.axis.y;
		*z = _valueOut.axis.z;
}

/**
 * Get the current values
 */
void TripleAxisSensor::getValue(TRIPLEAXISSENSOR_READING *value) {
		memcpy((void *)value, (void *)&_valueOut, sizeof(*value));
}

/**
 * Get the magnitude (squared)
 */
float PURE TripleAxisSensor::magnitudeSquared() {
	float ret = 0.0f;

	for (uint8_t i = 0; i < 3; i++) {
		ret += _valueOut.value[i] *_valueOut.value[i];
	}
	return ret;
}

/**
 * Get the magnitude
 */
float PURE TripleAxisSensor::magnitude() {
	return sqrt(magnitudeSquared());
}

/**
 * Handle any pending events for the accelerometer
 */
void TripleAxisSensor::handleEvents() {
	if (isSampleReady()) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			memcpy((void *)&_valueOut, (void *)&_valueTemp, sizeof(_valueOut));
		}

		if (_limitMagnitudeSquared) {
			int32_t magSquared = magnitudeSquared();

			if (magSquared >= _limitMagnitudeSquared) {
				_listener->limitReached(*this, TripleAxisSensorChannel::MAGNITUDE);
			}
		}

		if (_listener) {
		TRIPLEAXISSENSOR_READING value;
		getValue(&value);

		for (uint8_t i = 0; i < 3; i++) {
			if (_limit.value[i] && (value.value[i] * value.value[i]) >= _limit.value[i]) {
				_listener->limitReached(*this, (TripleAxisSensorChannel)i);
			}
		}

		_listener->sampleIsReady(*this);
		}
	}
}

/**
 * Set a limit to trigger the listener
 * @param which	the channel limit to set
 * @param limit	the value of the limit in Gs
 */
void TripleAxisSensor::setLimit(TripleAxisSensorChannel which, float limit) {
	if (TripleAxisSensorChannel::MAGNITUDE == which) {
		_limitMagnitudeSquared = limit * limit;
		return;
	}

	_limit.value[(uint8_t)which] = limit * limit;
}

/**
 * Get the current limits (unscaled)
 */
void TripleAxisSensor::getLimits(TRIPLEAXISSENSOR_READING *limits) {
	memcpy((void *)limits, (void *)&_limit, sizeof(*limits));
}

/**
 * Called when a timer alarm goes off
 */
void TripleAxisSensor::alarm(UNUSED AlarmSource source) {
	sample();
}

/**
 * Set whether the sensor is calibrated calibrated
 * @param calibrated	true if the sensor is now calibrated
 */
void TripleAxisSensor::setCalibrated(bool calibrated) {
	_calibrated = calibrated;
}

/**
 * Check if the sensor is calibrated
 * @return true if the sensor is calibrated
 */
bool TripleAxisSensor::isCalibrated() {
	return _calibrated;
}

}
