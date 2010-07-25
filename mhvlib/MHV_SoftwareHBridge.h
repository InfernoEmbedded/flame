/*
 * MHV_SoftwareHBridge.h
 *
 *  Created on: 04/07/2010
 *      Author: Deece
 */

#ifndef MHV_SOFTWAREHBRIDGE_H_
#define MHV_SOFTWAREHBRIDGE_H_

#include <inttypes.h>
#include <MHV_io.h>
#include <MHV_Timer16.h>

enum mhv_SoftwareHBridge_direction {
	MHV_SOFTWAREHBRIDGE_DIR_COAST,
	MHV_SOFTWAREHBRIDGE_DIR_FORWARD,
	MHV_SOFTWAREHBRIDGE_DIR_BACKWARD,
	MHV_SOFTWAREHBRIDGE_DIR_BRAKE
};
typedef enum mhv_SoftwareHBridge_direction MHV_SOFTWAREHBRIDGE_DIRECTION;

enum mhv_SoftwareHBridge_type {
	MHV_SOFTWAREHBRIDGE_TYPE_PULLUP,
	MHV_SOFTWAREHBRIDGE_TYPE_DIRECT
};
typedef enum mhv_SoftwareHBridge_type MHV_SOFTWAREHBRIDGE_TYPE;

class MHV_SoftwareHBridge {
protected:
	MHV_SOFTWAREHBRIDGE_TYPE	_type;
	MHV_Timer16					*_timer;
	uint8_t						_timerChannel;

	volatile uint8_t			*_dir1Top;
	volatile uint8_t			*_out1Top;
	uint8_t 					_pin1Top;
	volatile uint8_t			*_out1Bottom;
	uint8_t						_pin1Bottom;
	volatile uint8_t			*_dir2Top;
	volatile uint8_t			*_out2Top;
	uint8_t						_pin2Top;
	volatile uint8_t			*_out2Bottom;
	uint8_t						_pin2Bottom;

	MHV_SOFTWAREHBRIDGE_DIRECTION _direction;

public:
	MHV_SoftwareHBridge(MHV_SOFTWAREHBRIDGE_TYPE type, MHV_Timer16 *timer, uint8_t timerChannel,
			volatile uint8_t *dir1Top, volatile uint8_t *out1Top, volatile uint8_t *in1Top, uint8_t pin1Top,
			volatile uint8_t *dir1Bottom, volatile uint8_t *out1Bottom, volatile uint8_t *in1Bottom, uint8_t pin1Bottom,
			volatile uint8_t *dir2Top, volatile uint8_t *out2Top, volatile uint8_t *in2Top, uint8_t pin2Top,
			volatile uint8_t *dir2Bottom, volatile uint8_t *out2Bottom, volatile uint8_t *in2Bottom, uint8_t pin2Bottom);
	void reset();
	void update();
	void set(MHV_SOFTWAREHBRIDGE_DIRECTION direction, uint16_t magnitude);
	void set(MHV_SOFTWAREHBRIDGE_DIRECTION direction);
};

#endif /* MHV_SOFTWAREHBRIDGE_H_ */
