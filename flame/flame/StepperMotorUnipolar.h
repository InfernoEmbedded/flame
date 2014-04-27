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

#ifndef FLAME_STEPPERMOTORUNIPOLAR_H_
#define FLAME_STEPPERMOTORUNIPOLAR_H_

#include <flame/StepperMotor.h>
#include <avr/pgmspace.h>

namespace flame {

// coil patterns for wave drive
#ifdef __FLASH
const uint8_t __flash wavedrive[] = {
#else
const uint8_t wavedrive[] PROGMEM = {
#endif
//	Phase  xxxx DCBA
		0x01, 0x02, 0x04, 0x08
};

// coil patterns for full step drive
#ifdef __FLASH
const uint8_t __flash fulldrive[] = {
#else
const uint8_t fulldrive[] PROGMEM = {
#endif
//	Phase  xxxx DCBA
		0x03, 0x06, 0x0c, 0x09
};

// coil patterns for half step drive
#ifdef __FLASH
const uint8_t __flash halfdrive[] = {
#else
const uint8_t halfdrive[] PROGMEM = {
#endif
//	Phase  xxxx DCBA
		0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09
};


enum class StepperMode : uint8_t {
	WAVE,
	FULL,
	HALF
};

#define PHASE_MASK (0x0f << phaseAPin)

/**
 * A unipolar stepper motor driver
 *
 * Phase output is active high, so if you are using transistors to buffer the motor,
 * you will need to pull the common line high and use the transistors to pull the phases
 * to ground.
 *
 * Phases A & C share a common ground, as does B & D
 *
 * Phases B, C and D use consecutive pins on the same port as A
 *
 * @tparam	mode	the mode of the stepper driver
 * @tparam	phaseA	the pin for the A phase
 */
template <StepperMode mode, FLAME_DECLARE_PIN(phaseA)>
class StepperMotorUnipolar: public StepperMotor {
private:
	int8_t				_state;
	uint8_t				_steps;

	/**
	 * Set the outputs based on the current state & mode
	 */
	void setPins() {
		uint8_t coilPattern = 0;

#ifdef __FLASH
		switch (mode) {
		case StepperMode::WAVE:
			coilPattern = wavedrive[_state];
			break;
		case StepperMode::FULL:
			coilPattern = fulldrive[_state];
			break;
		case StepperMode::HALF:
			coilPattern = halfdrive[_state];
			break;
		}
#else
		switch (mode) {
		case StepperMode::WAVE:
			coilPattern = pgm_read_byte(wavedrive + _state);
			break;
		case StepperMode::FULL:
			coilPattern = pgm_read_byte(fulldrive + _state);
			break;
		case StepperMode::HALF:
			coilPattern = pgm_read_byte(halfdrive + _state);
			break;
		}
#endif

		coilPattern <<= phaseAPin;
		_MMIO_BYTE(phaseAOut) = (_MMIO_BYTE(phaseAOut) & ~PHASE_MASK) | coilPattern;
	}

public:
	/**
	 * Create a driver for a unipolar stepper motor
	 *
	 * @param	rtc		the RTC used for stepper movements
	 */
	StepperMotorUnipolar(RTC &rtc) :
			StepperMotor(rtc),
			_state(0),
			_steps((StepperMode::HALF == mode) ? 8 : 4) {
		setOutput(phaseADir, phaseAOut, phaseAIn, phaseAPin + 0, phaseAPinchangeInterrupt);
		setOutput(phaseADir, phaseAOut, phaseAIn, phaseAPin + 1, phaseAPinchangeInterrupt);
		setOutput(phaseADir, phaseAOut, phaseAIn, phaseAPin + 2, phaseAPinchangeInterrupt);
		setOutput(phaseADir, phaseAOut, phaseAIn, phaseAPin + 3, phaseAPinchangeInterrupt);

		setPins();
	}

	void step(bool forwards) {
		if (forwards) {
			if (++_state >= _steps) {
				_state = 0;
			}
		} else {
			if (--_state < 0) {
				_state = _steps - 1;
			}
		}

		setPins();
	}
};

}
#endif /* FLAME_STEPPERMOTORUNIPOLAR_H_ */
