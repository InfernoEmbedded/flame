/*
 * Copyright (c) 2012, Make, Hack, Void Inc
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

#ifndef MHV_ACCELEROMETER_ANALOG_H_
#define MHV_ACCELEROMETER_ANALOG_H_

#include <mhvlib/ADCManager.h>
#include <math.h>

namespace mhvlib {

/**
 * Accelerometer Interface
 */
template<uint8_t adcChannelX, uint8_t adcChannelY, uint8_t adcChannelZ, uint8_t adcReference>
class AccelerometerAnalog : public Accelerometer, ADCListener {
protected:
	ADCManager		*_adc;
	bool			_sampleReady;
	bool			_dummyRead;

public:
	AccelerometerAnalog(ADCManager &adc) :
		_adc(&adc),
		_sampleReady(false),
		_dummyRead(false) {

		_adc->registerListener(adcChannelX, this);
		_adc->registerListener(adcChannelY, this);
		_adc->registerListener(adcChannelZ, this);
	}

	/**
	 * Initiate a sampling of the accelerometer
	 */
	void sample() {
		if (MHV_AD_REFERENCE != adcReference) {
// Do a dummy read since we are changing ADC references and the reading may not be accurate
			_dummyRead = true;
		}

		_adc->read(adcChannelX, adcReference);
	}

	/**
	 * Handle incoming ADC reads
	 * @param adcChannel	the channel that was read from
	 * @param adcValue		the value read from the channel
	 */
	void adc(uint8_t adcChannel, uint16_t adcValue) {
		if (_dummyRead) {
// Redo the read since we the ADC reference was changed and the reading may not be accurate
			_dummyRead = false;
			_adc->read(adcChannelX, adcReference);
			return;
		}

		switch (adcChannel) {
		case adcChannelX:
			pushSample(X, adcValue);
			_adc->read(adcChannelY, adcReference);
			break;
		case adcChannelY:
			pushSample(Y, adcValue);
			_adc->read(adcChannelZ, adcReference);
			break;
		case adcChannelZ:
			pushSample(Z, adcValue);
			_sampleReady = true;
			break;
		}
	}

	/**
	 * Check if a sample is ready
	 * @return true if the sample is ready
	 */
	bool isSampleReady() {
		return _sampleReady;
	}

}; // class AccelerometerAnalog

} // namespace mhvlib

#endif /* MHV_ACCELEROMETER_ANALOG_H_ */
