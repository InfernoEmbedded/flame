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

/*
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
*/

#include <MHV_io.h>
#include <MHV_VoltageRegulator.h>
#include <MHV_AD.h>

#ifdef MHV_AD_RESOLUTION

/* Initialize the library
 * @param mode			the mode the converter runs in (VREG_BUCK, VREG_BOOST)
 * @param voltage		the voltage to regulate to
 * @param vrefVoltage	the reference voltage value
 * @param vref			the voltage reference
 * @param divider		the value of the feedback resistor divider used (eg: 1/10, or R2/(R1 + R2) )
 * @param timer			the timer for which the duty cycle will be adjusted
 * @param timerChannel	the time channel to vary the duty cycle for
 * @param adcChannel	the ADC channel
 */
MHV_VoltageRegulator::MHV_VoltageRegulator(MHV_VREG_MODES mode, float voltage, float vrefVoltage, uint8_t vref,
		float divider, MHV_Timer8 *timer, uint8_t channel) {
	_targetADC = (uint16_t)(voltage * divider * MHV_AD_RESOLUTION / vrefVoltage);
	_vref = vref;
	_timer = timer;
	_adcChannel = channel;
	_mode = mode;
	_pwm = 1;
	_lastADC = 0;
	_lastMoveUp = true;
	_invert = false;
}

/* Regulate as a boosting regulator
 */
inline void MHV_VoltageRegulator::regulateBoost() {
	uint16_t adc = mhv_ad_busyRead(_adcChannel, _vref);

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
	} else if (newPWM > _timer->getTop()) {
		newPWM = 200;
	}

	_pwm = newPWM;
	_timer->setOutput2(_pwm);
}

/* Regulate as a buck regulator
 */
inline void MHV_VoltageRegulator::regulateBuck() {
	uint16_t adc = mhv_ad_busyRead(_adcChannel, _vref);

	_lastADC = adc;

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
		} else if (newPWM > 255) {
			newPWM = 255;
		}

		_pwm = newPWM;
		_timer->setOutput2(_pwm);
	}
}

/* Adjust the duty cycle to bring the voltage closer to what we want
 * Call this function periodically to ensure regulation
 */
void MHV_VoltageRegulator::regulate() {
	switch (_mode) {
	case VREG_MODE_BOOST:
		regulateBoost();
		break;
	case VREG_MODE_BUCK:
		regulateBuck();
		break;
	}
}

#endif
