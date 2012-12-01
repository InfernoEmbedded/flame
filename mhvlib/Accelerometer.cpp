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

#include <mhvlib/Accelerometer.h>

namespace mhvlib {

/**
 * Push a sample from the driver into this class
 * @param which		Which channel to push
 * @param value		the updated value
 */
void Accelerometer::pushSample(AccelerometerChannel which, int16_t value) {
	switch (which) {
	case X:
		_valueTemp.x = (value - _offsets.x) / _scaling.x;
		break;
	case Y:
		_valueTemp.y = (value - _offsets.y) / _scaling.y;
		break;
	case Z:
		_valueTemp.z = (value - _offsets.z) / _scaling.z;
		break;
	}
}

/**
 * Constructor
 */
Accelerometer::Accelerometer() {}

/**
 * Save the current calibration data to EEPROM
 * @param eeprom	the EEPROM access instance
 * @param address	the address to save to (18 bytes)
 */
void Accelerometer::saveCalibration(EEPROM &eeprom, uint16_t address) {
	eeprom.write((void *)&_offsets, address, sizeof(_offsets), this);
	// continued in eepromDone
}

/**
 * Called when an EEPROM write is finished
 * @param eeprom	the EEPROM access instance
 * @param address	the EEPROM address that the data was written to
 * @param buffer	the pointer that the data was wriiten from
 */
void Accelerometer::eepromDone(EEPROM *eeprom, uint16_t address, void *buffer) {
	if (buffer == &_offsets) {
		// now write the scaling factors
		eeprom->write((void *)&_scaling, address + sizeof(_offsets),
				sizeof(_scaling), NULL);
	}
}

/**
 * Load calibration data
 * @param eeprom	the EEPROM access instance
 * @param address	the EEPROM address that the data was written to
 * @return false on success, true if the device needs to be calibrated
 */
bool Accelerometer::loadCalibration(EEPROM &eeprom, uint16_t address) {
	uint16_t crc = eeprom.crc(address, sizeof(_offsets) + sizeof(_scaling));
	uint16_t storedCRC;

	eeprom.busyRead(&storedCRC, address + sizeof(_offsets) + sizeof(_scaling),
			sizeof(storedCRC));

	if (crc != storedCRC) {
		return true;
	}

	eeprom.busyRead(&_offsets, address, sizeof(_offsets));
	eeprom.busyRead(&_scaling, address + sizeof(_offsets), sizeof(_scaling));
}

/**
 * Set the offsets (pre-scaling) for the accelerometer
 * @param offsets	the new values for the offsets
 */
void Accelerometer::setOffsets(ACCELEROMETER_OFFSETS *offsets) {
	memCopy(&_offsets, offsets, sizeof(*offsets));
}

/**
 * Set the offsets (pre-scaling) for the accelerometer
 * @param x		the X offset
 * @param y		the Y offset
 * @param z		the Z offset
 */
void Accelerometer::setOffsets(int16_t x, int16_t y, int16_t z) {
	_offsets.x = x;
	_offsets.y = y;
	_offsets.z = z;
}

/**
 * Set the scaling for the accelerometer
 * @param scales	the new values for scaling
 */
void Accelerometer::setScale(ACCELEROMETER_SCALING *scales) {
	memCopy(&_scaling, scales, sizeof(*scales));
}

/**
 * Set the scaling for the accelerometer
 * @param x		the X scale
 * @param y		the Y scale
 * @param z		the Z scale
 */
void Accelerometer::setScale(float x, float y, float z) {
	_scaling.x = x;
	_scaling.y = y;
	_scaling.z = z;
}

/**
 * Register a listener
 * @param	listener	the listener to notify
 */
void Accelerometer::registerListener(AccelerometerListener &listener) {
	_listener = &listener;
}

/**
 * Deregister the listener
 */
void Accelerometer::deregisterListener() {
	_listener = NULL;
}

/**
 * Get the current values
 */
void Accelerometer::getValues(float *x, float *y, float *z) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		*x = _valueOut.x;
		*y = _valueOut.y;
		*z = _valueOut.z;
	}
}

/**
 * Get the current values
 */
void Accelerometer::getValues(ACCELEROMETER_READING *value) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		memCopy((void *)value, (void *)&_valueOut, sizeof(*value));
	}
}

/**
 * Get the magnitude (squared)
 */
float Accelerometer::magnitudeSquared() {
	return	_valueOut.x * _valueOut.x +
			_valueOut.y * _valueOut.y +
			_valueOut.z * _valueOut.z;
}

/**
 * Get the magnitude
 */
float Accelerometer::magnitude() {
	return sqrt(magnitudeSquared());
}

/**
 * Handle any pending events for the accelerometer
 */
void Accelerometer::handleEvents() {
	if (isSampleReady()) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			memCopy((void *)&_valueOut, (void *)&_valueTemp, sizeof(_valueOut));
		}

		if (_limitMagnitudeSquared) {
			int32_t magnitudeSquared = magnitudeSquared();

			if (magnitudeSquared >= _limitMagnitudeSquared) {
				_listener->limitReached(this, AccelerometerChannel::MAGNITUDE);
			}
		}

		ACCELEROMETER_READING value;
		getValues(&value);

		if (_limit.x && abs(value.x) >= _limit.x) {
			_listener->limitReached(this, AccelerometerChannel::X);
		}
		if (_limit.y && abs(value.y) >= _limit.y) {
			_listener->limitReached(this, AccelerometerChannel::Y);
		}
		if (_limit.z && abs(value.z) >= _limit.z) {
			_listener->limitReached(this, AccelerometerChannel::Z);
		}
	}
}

/**
 * Set a limit to trigger the listener
 * @param which	the channel limit to set
 * @param limit	the value of the limit in Gs
 */
void Accelerometer::setLimit(AccelerometerChannel which, float limit) {
	switch (which) {
	case X:
		_limit.x = limit * _scaling.x + _offsets.x;
		break;
	case Y:
		_limit.y = limit * _scaling.y + _offsets.y;
		break;
	case Z:
		_limit.z = limit * _scaling.z + _offsets.z;
		break;
	case MAGNITUDE:
		_limitMagnitudeSquared = limit * limit;
		break;
	}
}

/**
 * Get the current limits (unscaled)
 */
void Accelerometer::getLimits(ACCELEROMETER_READING *limits) {
	memCopy((void *)limits, (void *)_limit, sizeof(*limits));
}


}
