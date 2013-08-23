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

/*
 *      Fixme:  need to measure how many clocks it takes to get to the point in the handler to toggle the pin,
 *      then subtract that from the trigger time
 *      Fixme: We lose accuracy if multiple servos are positioned on the same clock
 *      	one possible solution is to cluster the servos into groups such that there are no closely positioned
 *      		servos in any group, then stagger the triggering of the groups
 *
 */

#ifndef MHV_SERVOCONTROL_H_
#define MHV_SERVOCONTROL_H_

#include <mhvlib/Timer.h>

#ifdef MHV_TIMER16_1

// 1ms in timer ticks
#define MHV_SERVO_MIN	(F_CPU / (1000L))
// 2ms in timer ticks
#define MHV_SERVO_MAX	((F_CPU * 2) / (1000L))

#define MHV_MAX_SERVO_COUNT	254

namespace mhvlib {


/* The servoOrder member is not really part of the control block, but instead is an easy way
 * to have an array of N things we can use to specify the order the servos should be serviced
 */
struct ServoControlBlock {
	mhv_register		port;
	uint8_t				pin;
	uint16_t			position;
	int16_t				clockMinOffset;
	int16_t				clockMaxOffset;
	uint8_t				servoOrder; // This isn't really a member of the control structure
};
typedef struct ServoControlBlock SERVOCONTROLBLOCK;

template <uint8_t servoCount>
class ServoControl : public TimerListener {
#define SERVO_ORDER(mhvServoIndex) \
	(_controlBlocks[mhvServoIndex].servoOrder)

private:
	Timer 					&_timer;
	SERVOCONTROLBLOCK 		_controlBlocks[servoCount];
	volatile uint8_t		_nextServoIndex;

	/**
	 * Sort the servos in order of ascending time (required to minimise the processing load)
	 */
	void sortServos() {
		if (servoCount < 2) {
			return;
		} else {
			uint8_t i;
			uint16_t curPosition;

			// We'll use a bubble sort here since the general case is that the list is sorted, with only 1 servo out of position
			for (i = 0; (i + 1) < servoCount; i++) {
				if ((curPosition = _controlBlocks[_controlBlocks[i].servoOrder].position)
						> _controlBlocks[_controlBlocks[i + 1].servoOrder].position) {
					_controlBlocks[_controlBlocks[i].servoOrder].position =
							_controlBlocks[_controlBlocks[i + 1].servoOrder].position;
					_controlBlocks[_controlBlocks[i + 1].servoOrder].position = curPosition;
					if (i > 1) {
						i -= 2;
					} else {
						// We want i to wrap around to 0 on the next iteration
						i = -1;
					}
				}
			}
		}
	}

public:
	/**
	 * Create a new ServoControl object
	 * @param	timer			the timer to use (should be set to MHV_MODE_ONE_SHOT)
	 */
	ServoControl(Timer &timer) :
			_timer(timer),
			_nextServoIndex(255) {
		uint8_t i;
		for (i = 0; i < servoCount; i++) {
			_controlBlocks[i].clockMaxOffset = 0;
			_controlBlocks[i].clockMinOffset = 0;
			_controlBlocks[i].pin = 0;
			_controlBlocks[i].port = 0;
			_controlBlocks[i].position = 0;
			_controlBlocks[i].servoOrder = 255;
		}

		_timer.setListener1(this);
	}

	/**
	 * Add a servo to be controlled
	 * @param	servo	an index to address the servo by
	 * @param	pin	the pin the servo is connected to
	 */
	#pragma GCC diagnostic ignored "-Wunused-parameter"
	void addServo(uint8_t servo, MHV_DECLARE_PIN(pin)) {
		_controlBlocks[servo].pin = pinPin;
		_controlBlocks[servo].port = pinOut;
		_controlBlocks[servo].position = (MHV_SERVO_MIN + MHV_SERVO_MAX) / 2;

		setOutput(pinDir, pinOut, pinIn, pinPin, -1);

		if (_timer.enabled()) {
			sortServos();
		}
	}
	#pragma GCC diagnostic warning "-Wunused-parameter"

	/**
	 * Adjust the minimum & maximum positions of a servo
	 * @param	minOffset	the new minimum position
	 * @param	maxOffset	the new maximum position
	 */
	void tweakServo(uint8_t servo, int8_t minOffset, int8_t maxOffset) {
		_controlBlocks[servo].clockMinOffset = minOffset * F_CPU / (2 * 1000000); // timer ticks per us
		_controlBlocks[servo].clockMaxOffset = maxOffset * F_CPU / (2 * 1000000);
	}

	/**
	 * Set a servo to a new position
	 * @param	servo		the servo to position
	 * @param	newPosition	the new position of the servo (0 - 65535)
	 */
	void positionServo(uint8_t servo, uint16_t newPosition) {
		_controlBlocks[servo].position = ((uint32_t)newPosition) * (MHV_SERVO_MAX + _controlBlocks[servo].clockMaxOffset -
				(MHV_SERVO_MIN + _controlBlocks[servo].clockMinOffset) ) / 65535 + MHV_SERVO_MIN + _controlBlocks[servo].clockMinOffset;

		if (_timer.enabled()) {
			sortServos();
		}
	}

	/**
	 * Determine if a servo can safely be positioned (avoids altering data while the class is processing it)
	 * @return true if positions can be set
	 */
	bool canPosition() {
		return 255 == _nextServoIndex;
	}

	/**
	 * Set a servo to a new position, waiting until it can be safely positioned
	 * @param	servo		the servo to position
	 * @param	newPosition	the new position of the servo (0 - 65535)
	 */
	void positionServoBusyWait(uint8_t servo, uint16_t newPosition) {
		while (255 != _nextServoIndex) {}
		positionServo(servo, newPosition);
	}

	/**
	 * Enable the controller
	 */
	void enable() {
		uint8_t i;
		for (i = 0; i < servoCount; ++i) {
			if (_controlBlocks[i].port) {
				_controlBlocks[i].servoOrder = i;
			} else {
				_controlBlocks[i].servoOrder = 255;
			}
		}

		_nextServoIndex = 0;
		sortServos();
		alarm(AlarmSource.UNKNOWN);
	}

	/**
	 * Disable the controller
	 */
	void disable() {
		_timer.disable();
	}

	/**
	 * Start a servo cycle
	 * @param	timer	the timer that triggered us (unused)
	 */
	void alarm(UNUSED AlarmSource source) {
		if (255 == _nextServoIndex) {
			// Start of the servo pulse
			for (uint8_t i = 0; i < servoCount; i++) {
				if (_controlBlocks[i].port) {
					pinOn(0, _controlBlocks[SERVO_ORDER(i)].port,
							0, _controlBlocks[SERVO_ORDER(i)].pin, -1);
				}
			}
			_nextServoIndex = 0;
			// Set up the timer to end the first servo pulse
			// Note that the timer is reset to 0 when the interrupt is triggered, so the current value is how long to reach this point
			_timer.setPeriods(TimerPrescaler::PRESCALER_5_1,
					_controlBlocks[SERVO_ORDER(_nextServoIndex)].position - _timer.current(), 0, 0);
			_timer.enable();
			return;
		}

		uint16_t startPosition = _controlBlocks[SERVO_ORDER(_nextServoIndex)].position;

		for (;;) {
			pinOff(0, _controlBlocks[SERVO_ORDER(_nextServoIndex)].port,
					0, _controlBlocks[SERVO_ORDER(_nextServoIndex)].pin, -1);
			_nextServoIndex++;
			if (_nextServoIndex >= servoCount || 255 == SERVO_ORDER(_nextServoIndex)) {
				_nextServoIndex = 255;
				// sleep for 20ms to the next servo pass
				_timer.setTimes(20000UL, 0UL, 0UL); // sleep for 20ms
				_timer.enable();
				return;
			}

			// Set up the next servo if it is sufficiently far away from the current one
			if ((uint32_t)_controlBlocks[SERVO_ORDER(_nextServoIndex)].position >
					(uint32_t)_controlBlocks[SERVO_ORDER(_nextServoIndex-1)].position + _timer.current()) {

				_timer.setPeriods(TimerPrescaler::PRESCALER_5_1,
						_controlBlocks[SERVO_ORDER(_nextServoIndex)].position - startPosition - _timer.current(), 0, 0);
				_timer.enable();
				return;
			}
		}
	}
};

}

#endif

#endif /* MHV_SERVOCONTROL_H_ */
