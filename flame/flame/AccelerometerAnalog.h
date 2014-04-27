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
 *  * Neither the name of the Inferno Embedded nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL INFERNO EMBEDDED BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FLAME_ACCELEROMETER_ANALOG_H_
#define FLAME_ACCELEROMETER_ANALOG_H_

#include <flame/Accelerometer.h>
#include <flame/ADCManager.h>
#include <math.h>

namespace flame {

/**
 * An accelerometer that presents its data as analog values to be read by
 * the AVR's ADC channels
 * @tparam	ADCChannelX		the ADC channel to use for the X axis
 * @tparam	ADCChannelY		the ADC channel to use for the Y axis
 * @tparam	ADCChannelZ		the ADC channel to use for the Z axis
 * @tparam	ADCReference	the voltage reference to use
 *
 */
template<ADCChannel ADCChannelX, ADCChannel ADCChannelY, ADCChannel ADCChannelZ, ADCReference ADCReference>
class AccelerometerAnalog : public Accelerometer, public ADCListener {
protected:
	ADCManager		*_adc;
	bool			_sampleReady;
	bool			_dummyRead;

public:
	AccelerometerAnalog(ADCManager &adc) :
			_adc(&adc),
			_sampleReady(false),
			_dummyRead(false) {
		_adc->registerListener(ADCChannelX, this);
		_adc->registerListener(ADCChannelY, this);
		_adc->registerListener(ADCChannelZ, this);
	}

	/**
	 * Initiate a sampling of the accelerometer
	 */
	void sample() {
		if (FLAME_AD_REFERENCE != (uint8_t)ADCReference) {
// Do a dummy read since we are changing ADC references and the reading may not be accurate
			_dummyRead = true;
		}

		_adc->read(ADCChannelX, ADCReference);
	}

	/**
	 * Handle incoming ADC reads
	 * @param channel	the channel that was read from
	 * @param adcValue	the value read from the channel
	 */
	void adc(ADCChannel channel, uint16_t adcValue) {
		if (_dummyRead) {
// Redo the read since we the ADC reference was changed and the reading may not be accurate
			_dummyRead = false;
			_adc->read(ADCChannelX, ADCReference);
			return;
		}

		switch (channel) {
		case ADCChannelX:
			_sampleReady = false;
			pushSample(TripleAxisSensorChannel::X, adcValue);
			_adc->read(ADCChannelY, ADCReference);
			break;
		case ADCChannelY:
			pushSample(TripleAxisSensorChannel::Y, adcValue);
			_adc->read(ADCChannelZ, ADCReference);
			break;
		case ADCChannelZ:
			pushSample(TripleAxisSensorChannel::Z, adcValue);
			_sampleReady = true;
			break;
		default:
			break;
		}
	}

	/**
	 * Check if a sample is ready
	 * @return true if the sample is ready
	 */
	bool isSampleReady() {
		if (_sampleReady) {
			_sampleReady = false;
			return true;
		}
		return false;
	}
}; // class AccelerometerAnalog

} // namespace flame

#endif /* FLAME_ACCELEROMETER_ANALOG_H_ */
