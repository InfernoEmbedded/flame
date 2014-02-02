/*
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

#ifndef FLAME_STEPPERMOTOR_H_
#define FLAME_STEPPERMOTOR_H_

#include <flame/io.h>
#include <flame/RTC.h>

namespace flame{

class StepperListener {
public:
	virtual void moveComplete(int32_t position) =0;
};

class StepperMotor : public TimerListener {
private:
	int32_t			_position;
	RTC				&_rtc;
	bool			_moving;
	bool			_forward;
	float			_speed;
	int32_t			_until;
	TIMESTAMP		_started;
	int32_t			_startPosition;
	StepperListener	*_stepperListener;

public:
	StepperMotor(RTC &rtc);
	virtual void step(bool forward) =0;
	void setPosition(int32_t position);
	bool isMoving() PURE;
	int32_t	getPosition() PURE;
	void rotate(bool forward, float speed, int32_t until);
	void alarm(AlarmSource source);
	void registerListener(StepperListener &listener);
	void deregisterListener();
};

}

#endif /* FLAME_STEPPERMOTOR_H_ */
