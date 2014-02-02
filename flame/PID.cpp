/*
 * Inspired by the Arduino PID Library - Version 1
 *		by Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
 * 		licensed under a Creative Commons Attribution-ShareAlike 3.0 Unported License.
 *
 * Copyright (c) 2014, Inferno Embedded
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

#include <flame/PID.h>

namespace flame {

/**
 * Create a new PID
 * @param	setpoint	the target value
 * @param	kP			the proportional constant
 * @param	kI			the integral constant
 * @param	kD			the derivative constant
 * @param	period		the period that compute() is called, in ms
 * @param	reverse		true if there is an inverse relationship between the output and the input (eg. running a cooler to reduce temperature)
 * @param	min			the minimum value for output
 * @param	max			the maximum value for output
 */
PID::PID(float setpoint, float kP, float kI, float kD, uint16_t period,
		bool reverse, uint16_t min, uint16_t max) :
		_enabled(false),
		_setpoint(setpoint),
		_integral(0),
		_lastInput(0),
		_lastOutput(min),
		_outMin(min),
		_outMax(max) {
	_setpoint = setpoint;

	setDirection(reverse);
	setTuning(kP, kI, kD, period);
	clampIntegral();
}


inline void PID::clampIntegral() {
	if (_integral > _outMax) {
		_integral = _outMax;
	} else if (_integral < _outMin) {
		_integral = _outMin;
	}
}

/**
 * Calculate the next output
 * @param	input	the latest sample
 */
float PID::compute(float input) {
	float error = _setpoint - input;

	_integral += (_kI * error);

	clampIntegral();

	float output = _kP * error + _integral - _kD * (input - _lastInput);

	if (output > _outMax)
		output = _outMax;
	else if (output < _outMin)
		output = _outMin;

	_lastInput = input;
	_lastOutput = output;

	return output;
}


/**
 * Adjust PID parameters
 * @param	kP		the proportional constant
 * @param	kI		the integral constant
 * @param	kD		the derivative constant
 * @param	period	the period that compute() is called, in ms
 */
void PID::setTuning(float kP, float kI, float kD, uint16_t period) {
	float myPeriod = period / 1000;

	_kP = kP;
	_kI = kI * myPeriod;
	_kD = kD / myPeriod;

	if (_reverse) {
		_kP = 0 - _kP;
		_kI = 0 - _kI;
		_kD = 0 - _kD;
	}
}


/**
 * Alter the minimum & maximum values for the output
 * @param	min		the new minimum output value
 * @param	max		the new maximum output value
 */
void PID::setOutputLimits(float min, float max) {
	_outMin = min;
	_outMax = max;

	clampIntegral();
}


/**
 * Enable/Disable the PID
 * @param	enable	true to enable the pid
 */
void PID::enable(bool enable) {
	if (enable && !_enabled) {
		_integral = _lastOutput;

		clampIntegral();
	}
	_enabled = enable;
}


/**
 * Set the direction for the PID.
 *
 * Setting reverse means that increasing output reduces the input (eg. increasing power to a cooler,
 * with the input being temperature)
 */
void PID::setDirection(bool reverse) {
if (_enabled && reverse != _reverse) {
	_kP = 0 - _kP;
	_kI = 0 - _kI;
	_kD = 0 - _kD;
}

_reverse = reverse;
}

}
