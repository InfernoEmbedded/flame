/*
 * Inspired by the Arduino PID Library - Version 1
 *		by Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
 * 		licensed under a Creative Commons Attribution-ShareAlike 3.0 Unported License.
 *
 * MHV version Copyright (c) 2011, Make, Hack, Void Inc
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

#ifndef MHV_PID_H_
#define MHV_PID_H_

#include <MHV_io.h>

class MHV_PID {
protected:
	float	_kP;
	float	_kI;
	float	_kD;

	bool	_reverse;
	bool	_enabled;

	float	_setpoint;
	float	_integral;
	float	_lastInput;
	float	_lastOutput;

	float	_outMin;
	float	_outMax;

	inline void clampIntegral();

public:
	MHV_PID(float setpoint, float kP, float kI, float kD, uint16_t period,
			bool reverse, uint16_t min, uint16_t max);
	void setDirection(bool reverse);
	void enable(bool enable);
	float compute(float input);
	void setTuning(float kP, float kI, float kD, uint16_t period);
	void setOutputLimits(float, float);
};
#endif

