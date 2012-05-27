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

#include <mhvlib/io.h>
#include <mhvlib/AD.h>

#ifdef ADC
#include <mhvlib/Timer.h>
#include <mhvlib/RTC.h>

enum mhv_vreg_modes {
	MHV_VREG_MODE_BUCK,
	MHV_VREG_MODE_BOOST
};
typedef enum mhv_vreg_modes MHV_VREG_MODES;

#define MHV_VREG_DIVIDER(__mhv_divider_value) ((uint16_t)((__mhv_divider_value) * 16384))

/**
 * Allows the AVR to act as a voltage regulator
 * Inspired by: http://spritesmods.com/?art=ucboost
 *
 * Boost Mode:
 * 	Refer to http://www.ladyada.net/library/diyboostcalc.html for the schematic & calculator
 *
 * Buck mode:
 *  Refer to http://www.daycounter.com/Calculators/Switching-Converter-Calculator.phtml for the schematic & calculator
 *
 * The ADC input is capped at 1.1V or 2.56V - you must use a resistor divider (total impedance should be <10kOhm)
 * to bring the feedback voltage down into this range. Use the divider parameter to tell the library the value
 * of this divider network
 *
 * @param mode			the mode the converter runs in (VREG_BUCK, VREG_BOOST)
 * @param voltage		the voltage to regulate to
 * @param vrefVoltage	the reference voltage value
 * @param vref			the voltage reference
 * @param divider		the value of the feedback resistor divider used, multiplied by 16384 (eg: (16384 * 1/10, or (16384 * R2/(R1 + R2) )
 * @param adcChannel	the ADC channel for feedback
 */

template <MHV_VREG_MODES mode, uint32_t millivolts, uint32_t vrefMillivolts, uint8_t vref, uint16_t divider,
	uint8_t adcChannel>
class MHV_VoltageRegulator : public MHV_TimerListener {
private:
	uint16_t		_targetADC;
	MHV_Timer		&_timer;
	uint16_t		_pwm;	// The current PWM rate
	bool			_lastMoveUp;
	bool			_invert;
	uint16_t		_lastADC;

	/* Regulate as a boosting regulator
	 */
	inline void regulateBoost() {
		uint16_t adc = mhv_ad_busyRead(adcChannel, vref);
		uint16_t newPWM = (uint16_t)(_pwm * (float)_targetADC / (float)adc);

		if (newPWM == _pwm) {
			if (_targetADC < adc) {
				newPWM = _pwm - 1;
			} else if (_targetADC > adc) {
				newPWM = _pwm + 1;
			}
		}

		// set a minimum on and off time - boost regulators are useless if the transistor is always on or off
		if (newPWM < 1) {
			newPWM = 1;
		} else if (newPWM + 1 > _timer.getTop()) {
			newPWM = _timer.getTop() - 1;
		}

		_pwm = newPWM;
		_timer.setOutput2(_pwm);

		_lastADC = adc;
	}

	/**
	 * Regulate as a buck regulator
	 */
	inline void regulateBuck() {
		uint16_t adc = mhv_ad_busyRead(adcChannel, vref);

		if (adc < _targetADC) {
			_lastMoveUp = true;
		} else {
			_lastMoveUp = false;
		}

	// _lastMoveUp now contains the move we want to make
		uint16_t newPWM;
		if (adc != _targetADC) {
			if ((!_invert && !_lastMoveUp) || (_invert && _lastMoveUp)) {
				newPWM = _pwm - 1;
			} else {
				newPWM = _pwm + 1;
			}

	// set a minimum on and off time
			if (newPWM < 1) {
				newPWM = 1;
			} else if (newPWM > uint16_t(_timer.getTop() - 1)) {
				newPWM = _timer.getTop() - 1;
			}

			_pwm = newPWM;
			_timer.setOutput2(_pwm);
		}

		_lastADC = adc;
	}

	/**
	 * Adjust the duty cycle to bring the voltage closer to what we want
	 * Call this function periodically to ensure regulation
	 */
	inline void regulate() {
		switch (mode) {
		case MHV_VREG_MODE_BOOST:
			regulateBoost();
			break;
		case MHV_VREG_MODE_BUCK:
			regulateBuck();
			break;
		}
	}

public:
	/**
	 * Initialise the library
	 * @param timer			the timer for which the duty cycle will be adjusted
	 */
	MHV_VoltageRegulator(MHV_Timer &timer) :
				_timer(timer) {
		_targetADC = (uint16_t)(millivolts * divider * MHV_AD_RESOLUTION / (vrefMillivolts * 16384));
		_pwm = 1;
		_lastADC = 0;
		_lastMoveUp = true;
		_invert = false;
	}

	/**
	 * Initiate regulation as a MHV_TimerListener
	 */
	void alarm() {
		regulate();
	}

	/**
	 * Start regulating
	 */
	void enable() {
		_timer.enable();
		_timer.setOutput2(1);

		MHV_AD_ENABLE;
	}

	/**
	 * Stop regulating
	 */
	void disable() {
		_timer.setOutput2(0);
		_timer.disable();
	}

	/**
	 * Get the voltage as of when regulate was last called
	 */
	float PURE getVoltage() {
		return (float)(_lastADC * vrefMillivolts * 16384) / (divider * MHV_AD_RESOLUTION * 1000);
	}
};

#endif // ADC
#endif // MHV_VOLTAGEREGULATOR_H_
