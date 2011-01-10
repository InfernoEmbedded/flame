/*
 * Copyright (c) 2011, Make, Hack, Void Inc
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


/*
 *      Fixme:  need to measure how many clocks it takes to get to the point in the handler to toggle the pin,
 *      then subtract that from the trigger time
 *      Fixme: We lose accuracy if multiple servos are positioned on the same clock
 *      	one possible solution is to cluster the servos into groups such that there are no closely positioned
 *      		servos in any group, then stagger the triggering of the groups
 *
 */


#include <MHV_ServoControl.h>
#include <MHV_io.h>

#define SERVO_ORDER(mhvServoIndex) \
	(_controlBlocks[mhvServoIndex].servoOrder)

void MHV_ServoControl::refreshServos(void *data) {
	if (255 == _nextServoIndex) {
		// Start of the servo pulse
		for (int i = 0; i < _count; i++) {
			if (_controlBlocks[i].port) {
				mhv_pinOn(0, _controlBlocks[SERVO_ORDER(i)].port,
						0, _controlBlocks[SERVO_ORDER(i)].pin, -1);
			}
		}
		_nextServoIndex = 0;
		// Set up the timer to end the first servo pulse
		// Note that the timer is reset to 0 when the interrupt is triggered, so the current value is how long to reach this point
		_timer->setPeriods(MHV_TIMER_PRESCALER_5_1,
				_controlBlocks[SERVO_ORDER(_nextServoIndex)].position - _timer->current(), 0, 0);
		_timer->enable();
		return;
	}

	uint16_t startPosition = _controlBlocks[SERVO_ORDER(_nextServoIndex)].position;

	for (;;) {
		mhv_pinOff(0, _controlBlocks[SERVO_ORDER(_nextServoIndex)].port,
				0, _controlBlocks[SERVO_ORDER(_nextServoIndex)].pin, -1);
		_nextServoIndex++;
		if (_nextServoIndex >= _count || 255 == SERVO_ORDER(_nextServoIndex)) {
			_nextServoIndex = 255;
			// sleep for 20ms to the next servo pass
			_timer->setPeriods(20000UL, 0UL, 0UL); // sleep for 20ms
			_timer->enable();
			return;
		}

		// Set up the next servo if it is sufficiently far away from the current one
		if ((uint32_t)_controlBlocks[SERVO_ORDER(_nextServoIndex)].position >
				(uint32_t)_controlBlocks[SERVO_ORDER(_nextServoIndex-1)].position + _timer->current()) {

			_timer->setPeriods(MHV_TIMER_PRESCALER_5_1,
					_controlBlocks[SERVO_ORDER(_nextServoIndex)].position - startPosition - _timer->current(), 0, 0);
			_timer->enable();
			return;
		}
	}
}

MHV_ServoControl::MHV_ServoControl(MHV_Timer16 *timer, MHV_SERVOCONTROLBLOCK *controlBlocks, uint8_t count) {
	_timer = timer;
	_controlBlocks = controlBlocks;
	_count = count;
	_nextServoIndex = 255;

	uint8_t i;
	for (i = 0; i < count; i++) {
		controlBlocks[i].clockMaxOffset = 0;
		controlBlocks[i].clockMinOffset = 0;
		controlBlocks[i].pin = 0;
		controlBlocks[i].port = 0;
		controlBlocks[i].position = 0;
		controlBlocks[i].servoOrder = 255;
	}

	_timer->setMode(MHV_TIMER_ONE_SHOT);
}

void MHV_ServoControl::addServo(uint8_t servo, volatile uint8_t *dir, volatile uint8_t *out, volatile uint8_t *in, uint8_t pin, int8_t pinchangeInterrupt) {
	_controlBlocks[servo].pin = pin;
	_controlBlocks[servo].port = out;
	_controlBlocks[servo].position = (MHV_SERVO_MIN + MHV_SERVO_MAX) / 2;

	mhv_setOutput(dir, out, in, pin, -1);

	if (_timer->enabled()) {
		sortServos();
	}
}

void MHV_ServoControl::tweakServo(uint8_t servo, int8_t minOffset, int8_t maxOffset) {
	_controlBlocks[servo].clockMinOffset = minOffset * F_CPU / (2 * 1000000); // timer ticks per us
	_controlBlocks[servo].clockMinOffset = maxOffset * F_CPU / (2 * 1000000);
}

void MHV_ServoControl::positionServo(uint8_t servo, uint16_t newPosition) {
	_controlBlocks[servo].position = ((uint32_t)newPosition) * (MHV_SERVO_MAX + _controlBlocks[servo].clockMaxOffset -
			(MHV_SERVO_MIN + _controlBlocks[servo].clockMinOffset) ) / 65536 + MHV_SERVO_MIN + _controlBlocks[servo].clockMinOffset;

	if (_timer->enabled()) {
		sortServos();
	}
}

bool MHV_ServoControl::canPosition() {
	return 255 == _nextServoIndex;
}

// Position the servo, waiting until the next
void MHV_ServoControl::positionServoBusyWait(uint8_t servo, uint16_t newPosition) {
	while (255 != _nextServoIndex) {}
	positionServo(servo, newPosition);
}

void MHV_ServoControl::enable() {
	uint8_t i;
	for (i = 0; i < _count; ++i) {
		if (_controlBlocks[i].port) {
			_controlBlocks[i].servoOrder = i;
		} else {
			_controlBlocks[i].servoOrder = 255;
		}
	}

	_nextServoIndex = 0;
	sortServos();
	refreshServos(0);
}

void MHV_ServoControl::disable() {
	_timer->disable();
}

void MHV_ServoControl::sortServos() {
	uint8_t i;
	uint16_t curPosition;

	// We'll use a bubble sort here since the general case is that the list is sorted, with only 1 servo out of position
	for (i = 0; i < _count - 1; ++i) {
		if ((curPosition = _controlBlocks[_controlBlocks[i].servoOrder].position) > _controlBlocks[_controlBlocks[i+1].servoOrder].position) {
			_controlBlocks[_controlBlocks[i].servoOrder].position = _controlBlocks[_controlBlocks[i+1].servoOrder].position;
			_controlBlocks[_controlBlocks[i+1].servoOrder].position = curPosition;
			if (i > 1) {
				i -= 2;
			} else {
				// We want i to wrap around to 0 on the next iteration
				i = -1;
			}
		}
	}
}
