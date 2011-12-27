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

#ifndef MHV_VOLTAGEREGULATOR_H_
#define MHV_VOLTAGEREGULATOR_H_

#ifdef MHV_AD_RESOLUTION
#ifdef MHV_TIMER16_1
#include <MHV_Timer16.h>

enum mhv_vreg_modes {
	MHV_VREG_MODE_BUCK,
	MHV_VREG_MODE_BOOST
};
typedef enum mhv_vreg_modes MHV_VREG_MODES;


class MHV_VoltageRegulator {
private:
	MHV_VREG_MODES	_mode;
	uint8_t			_vref;
	float			_vrefVoltage;
	uint16_t		_targetADC;
	MHV_Timer16		&_timer;
	uint8_t			_adcChannel;
	uint16_t		_pwm;	// The current PWM rate
	bool			_lastMoveUp;
	bool			_invert;
	uint16_t		_lastADC;
	float			_divider;

	void regulateBoost();
	void regulateBuck();

public:
	MHV_VoltageRegulator(MHV_VREG_MODES mode, float voltage, float vrefVoltage, uint8_t vref,
			float divider, MHV_Timer16 &timer, uint8_t channel);
	void enable();
	void disable();
	float getVoltage();
	void regulate();
};

#endif // MHV_TIMER16_1
#endif // MHV_AD_RESOLUTION
#endif // MHV_VOLTAGEREGULATOR_H_
