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

#ifdef MHV_TIMER16_1

#include <MHV_Timer16.h>

// 1ms in timer ticks
#define MHV_SERVO_MIN	(F_CPU / (1000L))
// 2ms in timer ticks
#define MHV_SERVO_MAX	((F_CPU * 2) / (1000L))

#define MHV_MAX_SERVO_COUNT	254

#define MHV_SERVOCONTROL_DECLARE_TRIGGER(mhvTimer16,mhvServo) \
void mhvServo##Trigger(void *data) { \
	mhvServo.refreshServos(data);\
}

#define MHV_SERVOCONTROL_ASSIGN_TRIGGER(mhvTimer16,mhvServo) \
		mhvTimer16.setTriggers(&(mhvServo##Trigger), 0, 0, 0, 0, 0)

/* The servoOrder member is not really part of the control block, but instead is an easy way
 * to have an array of N things we can use to specify the order the servos should be serviced
 */
struct MHV_ServoControlBlock {
	volatile uint8_t	*port;
	uint8_t				pin;
	uint16_t			position;
	int16_t				clockMinOffset;
	int16_t				clockMaxOffset;
	uint8_t				servoOrder; // This isn't really a member of the control structure
};
typedef struct MHV_ServoControlBlock MHV_SERVOCONTROLBLOCK;

class MHV_ServoControl {
private:
	MHV_Timer16 			*_timer;
	MHV_SERVOCONTROLBLOCK 	*_controlBlocks;
	uint8_t					_count;
	volatile uint8_t		_nextServoIndex;

	void sortServos();

public:
	MHV_ServoControl(MHV_Timer16 *timer, MHV_SERVOCONTROLBLOCK *controlBlocks, uint8_t count);
	void addServo(uint8_t servo, volatile uint8_t *dir, volatile uint8_t *out, volatile uint8_t *in, uint8_t pin, int8_t pinchangeInterrupt);
	void tweakServo(uint8_t servo, int8_t minOffset, int8_t maxOffset);
	void positionServo(uint8_t servo, uint16_t position);
	bool canPosition();
	void positionServoBusyWait(uint8_t servo, uint16_t position);
	void refreshServos(void *data);
	void enable();
	void disable();
};

#endif

#endif /* MHV_SERVOCONTROL_H_ */
