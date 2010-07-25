/*
 * MHV_SoftwareHBridge.cpp
 *
 *  Created on: 04/07/2010
 *      Author: Deece
 */

#include <MHV_SoftwareHBridge.h>

/* Create a new H bridge where all transistors are controlled by us
 * Bottom transistors are toggled between VCC & ground.
 *
 * A timer is required for magnitude control.
 * If this is not required, a NULL may be passed
 *
 * If the H-bridge voltage is greater than VCC of the microcontroller:
 *   * resistors R1 & R2 and diodes D1 & D2 must be installed
 *   * Set type to MHV_SOFTWAREHBRIDGE_TYPE_PULLUP
 *   * Top transistors are toggled between high impedance & ground
 *
 * If the H bridge voltage is less than or equal to the microcontroller voltage,
 *  * resistors R1 & R2 can be omitted, and D1 and D2 can be shorted.
 *   * Set type to MHV_SOFTWAREHBRIDGE_TYPE_DIRECT
 *   * Top transistors are toggled between high impedance & ground
 *
 * The initial state is coasting
 */
MHV_SoftwareHBridge::MHV_SoftwareHBridge(MHV_SOFTWAREHBRIDGE_TYPE type, MHV_Timer16 *timer, uint8_t timerChannel,
		volatile uint8_t *dir1Top, volatile uint8_t *out1Top, volatile uint8_t *in1Top, uint8_t pin1Top,
		volatile uint8_t *dir1Bottom, volatile uint8_t *out1Bottom, volatile uint8_t *in1Bottom, uint8_t pin1Bottom,
		volatile uint8_t *dir2Top, volatile uint8_t *out2Top, volatile uint8_t *in2Top, uint8_t pin2Top,
		volatile uint8_t *dir2Bottom, volatile uint8_t *out2Bottom, volatile uint8_t *in2Bottom, uint8_t pin2Bottom) {
	_type = type;
	_timer = timer;
	_timerChannel = timerChannel;

	_dir1Top = dir1Top;
	_out1Top = out1Top;
	_pin1Top = pin1Top;
	_out1Bottom = out1Bottom;
	_pin1Bottom = pin1Bottom;
	_dir2Top = dir2Top;
	_out2Top = out2Top;
	_pin2Top = pin2Top;
	_out2Bottom = out2Bottom;
	_pin2Bottom = pin2Bottom;

	_direction = MHV_SOFTWAREHBRIDGE_DIR_COAST;
}

/* Set up for a new timer pass
 * Care is taken in here to never allow the top and bottom of any side
 * to be on simultaneously
 */
void MHV_SoftwareHBridge::reset() {
	switch (_direction) {
	case MHV_SOFTWAREHBRIDGE_DIR_COAST:
		// Tops and bottoms are off
		switch (_type) {
		case MHV_SOFTWAREHBRIDGE_TYPE_PULLUP:
			_mhv_setInput(_dir1Top, _out1Top, NULL, _pin1Top);
			_mhv_setInput(_dir2Top, _out2Top, NULL, _pin2Top);
			break;
		case MHV_SOFTWAREHBRIDGE_TYPE_DIRECT:
			_mhv_pinOn(NULL, _out1Top, NULL, _pin1Top);
			_mhv_pinOn(NULL, _out2Top, NULL, _pin2Top);
			break;
		}
		_mhv_pinOff(NULL, _out1Bottom, NULL, _pin1Bottom);
		_mhv_pinOff(NULL, _out2Bottom, NULL, _pin2Bottom);
		break;
	case MHV_SOFTWAREHBRIDGE_DIR_FORWARD:
		// Top 1 is on, bottom 2 is on
		_mhv_pinOff(NULL, _out1Bottom, NULL, _pin1Bottom);
		switch (_type) {
		case MHV_SOFTWAREHBRIDGE_TYPE_PULLUP:
			_mhv_setInput(_dir2Top, _out2Top, NULL, _pin2Top);
			_mhv_setOutput(_dir1Top, _out1Top, NULL, _pin1Top);
			_mhv_pinOff(_dir1Top, _out1Top, NULL, _pin1Top);
			break;
		case MHV_SOFTWAREHBRIDGE_TYPE_DIRECT:
			_mhv_pinOn(NULL, _out2Top, NULL, _pin2Top);
			_mhv_pinOff(NULL, _out1Top, NULL, _pin1Top);
			break;
		}
		_mhv_pinOn(NULL, _out2Bottom, NULL, _pin2Bottom);
		break;
	case MHV_SOFTWAREHBRIDGE_DIR_BACKWARD:
		// Top 2 is on, bottom 1 is on
		_mhv_pinOff(NULL, _out2Bottom, NULL, _pin2Bottom);
		switch (_type) {
		case MHV_SOFTWAREHBRIDGE_TYPE_PULLUP:
			_mhv_setInput(_dir1Top, _out1Top, NULL, _pin1Top);
			_mhv_setOutput(_dir2Top, _out2Top, NULL, _pin2Top);
			_mhv_pinOff(_dir2Top, _out2Top, NULL, _pin2Top);
			break;
		case MHV_SOFTWAREHBRIDGE_TYPE_DIRECT:
			_mhv_pinOn(NULL, _out1Top, NULL, _pin1Top);
			_mhv_pinOff(NULL, _out2Top, NULL, _pin2Top);
			break;
		}
		_mhv_pinOn(NULL, _out1Bottom, NULL, _pin1Bottom);
		break;
	case MHV_SOFTWAREHBRIDGE_DIR_BRAKE:
		// Bottoms are on
		switch (_type) {
		case MHV_SOFTWAREHBRIDGE_TYPE_PULLUP:
			_mhv_setInput(_dir1Top, _out1Top, NULL, _pin1Top);
			_mhv_setInput(_dir2Top, _out2Top, NULL, _pin2Top);
			break;
		case MHV_SOFTWAREHBRIDGE_TYPE_DIRECT:
			_mhv_pinOn(NULL, _out1Top, NULL, _pin1Top);
			_mhv_pinOn(NULL, _out2Top, NULL, _pin2Top);
			break;
		}
		_mhv_pinOn(NULL, _out1Bottom, NULL, _pin1Bottom);
		_mhv_pinOn(NULL, _out2Bottom, NULL, _pin2Bottom);
		break;
	}
}

/* Set the H bridge to coasting after the end of a timer cycle for PWM
 * This should be called from the trigger of the associated timer
 */
void MHV_SoftwareHBridge::update() {
	// Set to coasting until the next reset
	switch (_type) {
	case MHV_SOFTWAREHBRIDGE_TYPE_PULLUP:
		_mhv_setInput(_dir1Top, _out1Top, NULL, _pin1Top);
		_mhv_setInput(_dir2Top, _out2Top, NULL, _pin2Top);
		break;
	case MHV_SOFTWAREHBRIDGE_TYPE_DIRECT:
		_mhv_pinOn(NULL, _out1Top, NULL, _pin1Top);
		_mhv_pinOn(NULL, _out2Top, NULL, _pin2Top);
		break;
	}
	_mhv_pinOff(NULL, _out1Bottom, NULL, _pin1Bottom);
	_mhv_pinOff(NULL, _out2Bottom, NULL, _pin2Bottom);
}

/* Set the direction and magnitude of the H bridge
 * Requires a timer to be specified
 * param:	direction	the direction of the bridge
 * param:	magnitude	the magnitude of the direction (from 0 to TOP of the timer)
 */
void MHV_SoftwareHBridge::set(MHV_SOFTWAREHBRIDGE_DIRECTION direction, uint16_t magnitude) {
	_direction = direction;

	if (MHV_SOFTWAREHBRIDGE_DIR_COAST == direction ||
			MHV_SOFTWAREHBRIDGE_DIR_BRAKE == direction) {
		reset();
	}

	_timer->setOutput(_timerChannel, magnitude);
}

/* Set the direction of the H bridge
 * Does no require a timer to be specified
 * param:	direction	the direction of the H bridge
 */
void MHV_SoftwareHBridge::set(MHV_SOFTWAREHBRIDGE_DIRECTION direction) {
	_direction = direction;
	reset();
}

