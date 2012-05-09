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


#ifndef MHV_SERVOCONTROL_H_
#define MHV_SERVOCONTROL_H_

#include <MHV_Timer16.h>

#ifdef MHV_TIMER16_1

// 1ms in timer ticks
#define MHV_SERVO_MIN	(F_CPU / (1000L))
// 2ms in timer ticks
#define MHV_SERVO_MAX	((F_CPU * 2) / (1000L))

#define MHV_MAX_SERVO_COUNT	254

/**
 * Create a new servo controller
 * @param	_mhvObjName		the name of the object to create
 * @param	_mhvServoTimer	the 16 bit timer to use for servo control
 * @param	_mhvServoCount	the number of servos that need to be controlled
 */
#define MHV_SERVOCONTROL_CREATE(_mhvObjName, _mhvServoTimer, _mhvServoCount) \
	MHV_SERVOCONTROLBLOCK _mhvObjName ## ControlBlocks[_mhvServoCount]; \
	MHV_ServoControl _mhvObjName(_mhvServoTimer, _mhvObjName ## ControlBlocks, _mhvServoCount);



/* The servoOrder member is not really part of the control block, but instead is an easy way
 * to have an array of N things we can use to specify the order the servos should be serviced
 */
struct MHV_ServoControlBlock {
	mhv_register		port;
	uint8_t				pin;
	uint16_t			position;
	int16_t				clockMinOffset;
	int16_t				clockMaxOffset;
	uint8_t				servoOrder; // This isn't really a member of the control structure
};
typedef struct MHV_ServoControlBlock MHV_SERVOCONTROLBLOCK;

class MHV_ServoControl : public MHV_TimerListener {
private:
	MHV_Timer16 			&_timer;
	MHV_SERVOCONTROLBLOCK 	*_controlBlocks;
	uint8_t					_count;
	volatile uint8_t		_nextServoIndex;

	void sortServos();

public:
	MHV_ServoControl(MHV_Timer16 &timer, MHV_SERVOCONTROLBLOCK controlBlocks[], uint8_t count);
	void addServo(uint8_t servo, MHV_DECLARE_PIN(pin));
	void tweakServo(uint8_t servo, int8_t minOffset, int8_t maxOffset);
	void positionServo(uint8_t servo, uint16_t position);
	bool canPosition();
	void positionServoBusyWait(uint8_t servo, uint16_t position);
	void enable();
	void disable();
	void alarm();
};

#endif

#endif /* MHV_SERVOCONTROL_H_ */
