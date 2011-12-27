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

#ifndef MHV_STEPPERMOTORUNIPOLAR_H_
#define MHV_STEPPERMOTORUNIPOLAR_H_

#include <MHV_StepperMotor.h>

enum MHV_StepperMode {
	MHV_STEPPER_MODE_WAVE,
	MHV_STEPPER_MODE_FULL,
	MHV_STEPPER_MODE_HALF
};
typedef enum MHV_StepperMode MHV_STEPPER_MODE;

class MHV_StepperMotorUnipolar: public MHV_StepperMotor {
private:
	volatile uint8_t	*_phaseAOut;
	uint8_t				_phaseAPin;
	uint8_t				_phaseMask;

	MHV_STEPPER_MODE	_mode;
	int8_t				_state;
	uint8_t				_steps;

	void setOutput();

public:
	MHV_StepperMotorUnipolar(MHV_RTC &rtc, MHV_STEPPER_MODE mode, MHV_DECLARE_PIN(phaseA));
	virtual void step(bool forwards);
};

#endif /* MHV_STEPPERMOTORUNIPOLAR_H_ */
