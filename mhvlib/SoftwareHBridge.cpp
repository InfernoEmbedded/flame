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

#ifdef MHV_TIMER16_1

#include <mhvlib/SoftwareHBridge.h>

namespace mhvlib {

/* Create a new H bridge where all transistors are controlled by us
 * Bottom transistors are toggled between VCC & ground.
 *
 * *******************************
 * UNTESTED - USE AT YOUR OWN RISK
 * *******************************
 *
 * A timer is required for magnitude control.
 *
 * If the H-bridge voltage is greater than VCC of the microcontroller:
 *   * resistors R1 & R2 and diodes D1 & D2 must be installed
 *   * Set type to SOFTWAREHBRIDGE_TYPE::PULLUP
 *   * Top transistors are toggled between high impedance & ground
 *
 * If the H bridge voltage is less than or equal to the microcontroller voltage,
 *  * resistors R1 & R2 can be omitted, and D1 and D2 can be shorted.
 *   * Set type to SOFTWAREHBRIDGE_TYPE::DIRECT
 *   * Top transistors are toggled between high impedance & ground
 *
 * The initial state is coasting
 */
SoftwareHBridge::SoftwareHBridge(MHV_SOFTWAREHBRIDGE_TYPE type, MHV_Timer &timer,
		MHV_DECLARE_PIN(dir1Top),
		MHV_DECLARE_PIN(dir1Bottom),
		MHV_DECLARE_PIN(dir2Top),
		MHV_DECLARE_PIN(dir2Bottom)) :
		_type(type),
		_timer(timer),
		_dir1Top(dir1Top),
		_out1Top(out1Top),
		_pin1Top(pin1Top),
		_out1Bottom(out1Bottom),
		_pin1Bottom(pin1Bottom),
		_dir2Top(dir2Top),
		_out2Top(out2Top),
		_pin2Top(pin2Top),
		_out2Bottom(out2Bottom),
		_pin2Bottom(pin2Bottom),
		_direction(MHV_SOFTWAREHBRIDGE_DIR_COAST) {
	_timer.setListener1(this);
}

/* Set up for a new timer pass
 * Care is taken in here to never allow the top and bottom of any side
 * to be on simultaneously
 */
void SoftwareHBridge::reset() {
	switch (_direction) {
	case SOFTWAREHBRIDGE_DIRECTION::COAST:
		// Tops and bottoms are off
		switch (_type) {
		case SOFTWAREHBRIDGE_TYPE::PULLUP:
			setInput(_dir1Top, _out1Top, NULL, _pin1Top, -1);
			setInput(_dir2Top, _out2Top, NULL, _pin2Top, -1);
			break;
		case SOFTWAREHBRIDGE_TYPE::DIRECT:
			pinOn(NULL, _out1Top, NULL, _pin1Top, -1);
			pinOn(NULL, _out2Top, NULL, _pin2Top, -1);
			break;
		}
		pinOff(NULL, _out1Bottom, NULL, _pin1Bottom, -1);
		pinOff(NULL, _out2Bottom, NULL, _pin2Bottom, -1);
		break;
	case SOFTWAREHBRIDGE_DIRECTION::FORWARD:
		// Top 1 is on, bottom 2 is on
		pinOff(NULL, _out1Bottom, NULL, _pin1Bottom, -1);
		switch (_type) {
		case SOFTWAREHBRIDGE_TYPE::PULLUP:
			setInput(_dir2Top, _out2Top, NULL, _pin2Top, -1);
			setOutput(_dir1Top, _out1Top, NULL, _pin1Top, -1);
			pinOff(_dir1Top, _out1Top, NULL, _pin1Top, -1);
			break;
		case SOFTWAREHBRIDGE_TYPE::DIRECT:
			pinOn(NULL, _out2Top, NULL, _pin2Top, -1);
			pinOff(NULL, _out1Top, NULL, _pin1Top, -1);
			break;
		}
		pinOn(NULL, _out2Bottom, NULL, _pin2Bottom, -1);
		break;
	case SOFTWAREHBRIDGE_DIRECTION::BACKWARD:
		// Top 2 is on, bottom 1 is on
		pinOff(NULL, _out2Bottom, NULL, _pin2Bottom, -1);
		switch (_type) {
		case SOFTWAREHBRIDGE_TYPE::PULLUP:
			setInput(_dir1Top, _out1Top, NULL, _pin1Top, -1);
			setOutput(_dir2Top, _out2Top, NULL, _pin2Top, -1);
			pinOff(_dir2Top, _out2Top, NULL, _pin2Top, -1);
			break;
		case SOFTWAREHBRIDGE_TYPE::DIRECT:
			pinOn(NULL, _out1Top, NULL, _pin1Top, -1);
			pinOff(NULL, _out2Top, NULL, _pin2Top, -1);
			break;
		}
		pinOn(NULL, _out1Bottom, NULL, _pin1Bottom, -1);
		break;
	case SOFTWAREHBRIDGE_DIRECTION::BRAKE:
		// Bottoms are on
		switch (_type) {
		case SOFTWAREHBRIDGE_TYPE::PULLUP:
			setInput(_dir1Top, _out1Top, NULL, _pin1Top, -1);
			setInput(_dir2Top, _out2Top, NULL, _pin2Top, -1);
			break;
		case SOFTWAREHBRIDGE_TYPE::DIRECT:
			pinOn(NULL, _out1Top, NULL, _pin1Top, -1);
			pinOn(NULL, _out2Top, NULL, _pin2Top, -1);
			break;
		}
		pinOn(NULL, _out1Bottom, NULL, _pin1Bottom, -1);
		pinOn(NULL, _out2Bottom, NULL, _pin2Bottom, -1);
		break;
	}
}

/* Set the H bridge to coasting after the end of a timer cycle for PWM
 * This should be called from the trigger of the associated timer
 */
void SoftwareHBridge::alarm() {
	// Set to coasting until the next reset
	switch (_type) {
	case SOFTWAREHBRIDGE_TYPE::PULLUP:
		setInput(_dir1Top, _out1Top, NULL, _pin1Top, -1);
		setInput(_dir2Top, _out2Top, NULL, _pin2Top, -1);
		break;
	case SOFTWAREHBRIDGE_TYPE::DIRECT:
		pinOn(NULL, _out1Top, NULL, _pin1Top, -1);
		pinOn(NULL, _out2Top, NULL, _pin2Top, -1);
		break;
	}
	pinOff(NULL, _out1Bottom, NULL, _pin1Bottom, -1);
	pinOff(NULL, _out2Bottom, NULL, _pin2Bottom, -1);
}

/* Set the direction and magnitude of the H bridge
 * Requires a timer to be specified
 * param:	direction	the direction of the bridge
 * param:	magnitude	the magnitude of the direction (from 0 to TOP of the timer)
 */
void SoftwareHBridge::set(SOFTWAREHBRIDGE_DIRECTION direction, uint16_t magnitude) {
	_direction = direction;

	if (SOFTWAREHBRIDGE_DIRECTION::COAST == direction ||
			SOFTWAREHBRIDGE_DIRECTION::BRAKE == direction) {
		reset();
	}

	_timer->setOutput(_timerChannel, magnitude);
}

/* Set the direction of the H bridge
 * Does not require a timer to be specified
 * param:	direction	the direction of the H bridge
 */
void SoftwareHBridge::set(SOFTWAREHBRIDGE_DIRECTION direction) {
	_direction = direction;
	reset();
}

}
#endif // MHV_TIMER16_1
