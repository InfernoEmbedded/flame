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

#include <mhvlib/StepperMotor.h>

/**
 * Create a new StepperMotor
 * @param	rtc		an RTC we will use to trigger events
 */
MHV_StepperMotor::MHV_StepperMotor(MHV_RTC &rtc) :
		_position(0),
		_rtc(rtc),
		_moving(false),
		_stepperListener(NULL) {}

/**
 * Mark the current position of the motor.
 * This simply changes where we think we are, it does not move the motor
 *
 * @param	position	the new position
 */
void MHV_StepperMotor::setPosition(int32_t	position) {
		_position = position;
}

/**
 * Get the current position of the motor
 * @return the current position
 */
int32_t PURE MHV_StepperMotor::getPosition() {
	return _position;
}

/**
 * Is the motor current moving?
 * @return true if the motor is moving
 */
bool PURE MHV_StepperMotor::isMoving() {
	return _moving;
}

/**
 * Move the motor to a new position (this will override a current move if there is one)
 * @param	forward		true to move forward
 * @param	speed		the speed to move in steps/second
 * @param	until		the position to stop at
 */
void MHV_StepperMotor::rotate(bool forward, float speed, int32_t until) {
	_forward = forward;
	_speed = speed;
	_until = until;

// Remove any current alarms
	_rtc.removeAlarm(this);

// Register the alarm with the RTC
	_rtc.current(_started);

	_startPosition = _position;

	_moving = true;

	_rtc.addAlarm(this, 0, 0, 0, 1);
}

/**
 * Called periodically to move the motor
 */
void MHV_StepperMotor::alarm() {
	if (!_moving) {
		return;
	}

	MHV_TIMESTAMP elapsed;
	_rtc.elapsed(_started, elapsed);
// Reuse elapsed.timestamp to represent milliseconds elapsed
	elapsed.timestamp *= 1000;
	elapsed.timestamp += elapsed.milliseconds;

	int32_t expectedPosition = _startPosition + elapsed.timestamp * _speed / ((_forward) ? 1000 : -1000);

	if (expectedPosition != _position) {
		step(_forward);
		_position += (_forward) ? 1 : -1;
	}

	if (_until == _position) {
// We are done
		_speed = 0;
		_moving = false;
		_rtc.removeAlarm(this);
		if (_stepperListener) {
			_stepperListener->moveComplete(_position);
		}
	}
}

/**
 * Register a listener to be notified when moves are complete
 * @param	listener	the listener to notify
 */
void MHV_StepperMotor::registerListener(MHV_StepperListener &listener) {
	_stepperListener = &listener;
}

/**
 * Deregister the listener
 */
void MHV_StepperMotor::deregisterListener() {
	_stepperListener = NULL;
}
