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

#ifndef MHV_ADC_H_
#define MHV_ADC_H_

#include <mhvlib/io.h>
#include <mhvlib/AD.h>

#define MHV_ADC_ASSIGN_INTERRUPT(__mhvADCManager) \
ISR(ADC_vect) { \
	__mhvADCManager.adc(); \
}

/**
 * Allocate a buffer to contain event mappings
 */
#define MHV_ADC_BUFFER_CREATE(_mhvADCBufferName, _mhvADCBufferCount) \
MHV_EVENT_ADC _mhvADCBufferName[_mhvADCBufferCount];

namespace mhvlib_bsd {

class ADCListener {
public:
	virtual void adc(uint8_t adcChannel, uint16_t adcValue) =0;
};

struct eventADC {
	int8_t channel;
	ADCListener *listener;
};
typedef struct eventADC EVENT_ADC;

/**
 * An event manager for ADC events
 * @tparam	events	the number of ADC events we can handle
 */
template<uint8_t events>
class MHV_ADC {
protected:
	uint16_t _adcValue;
	int8_t _adcChannel;
	EVENT_ADC _adcs[events];

public:
	/**
	 * An event manager for ADC events
	 */

	MHV_ADC::MHV_ADC() {
		_adcChannel = -1;
		memClear(_adcs, sizeof(*_adcs), events);
	}

	/**
	 * Interrupt handler to read the ADC
	 */
	void MHV_ADC::adc() {
		_adcValue = ADC;
		_adcChannel = MHV_AD_CHANNEL;

		MHV_AD_DISABLE_INTERRUPT;
	}

	/**
	 * Register interest for an ADC channel
	 * @param	channel		the ADC channel
	 * @param	listener	an MHV_ADCListener to notify when an ADC reading has been completed
	 */
#pragma GCC diagnostic ignored "-Wsuggest-attribute=const"
	void MHV_ADC::registerListener(int8_t channel, ADCListener &listener) {
		for (uint8_t i = 0; i < events; i++) {
			if (_adcs[i].channel == -1) {
				_adcs[i].channel = channel;
				_adcs[i].listener = &listener;
				break;
			}
		}
	}
#pragma GCC diagnostic warning "-Wsuggest-attribute=const"

	/**
	 * Deregister interest for an ADC channel
	 * @param	channel		the ADC channel
	 */
	void MHV_ADC::deregisterListener(int8_t channel) {
		for (uint8_t i = 0; i < events; i++) {
			if (_adcs[i].channel == channel) {
				_adcs[i].channel = -1;
				break;
			}
		}
	}

	/**
	 * Read an ADC channel
	 * @param	channel		the channel to read
	 * @param	reference	the voltage reference to use
	 */
	uint16_t MHV_ADC::busyRead(int8_t channel, uint8_t reference) {
		ADMUX = reference | (channel & 0x0F);

#ifdef MUX5
		ADCSRB = (ADCSRB & ~_BV(MUX5)) | ((channel & _BV(5) >> (5 - MUX5)));
#endif

		// Start the conversion
		ADCSRA |= _BV(ADSC);

		while (ADCSRA & _BV(ADSC)) {
		};

		return ADC;
	}

	/**
	 * Trigger an ADC channel event
	 * @param	channel		the channel to read
	 * @param	reference	the voltage reference to use
	 */
	void MHV_ADC::asyncRead(int8_t channel, uint8_t reference) {
		ADMUX = reference | (channel & 0x0F);

#ifdef MUX5
		ADCSRB = (ADCSRB & ~_BV(MUX5)) | ((channel & _BV(5) >> (5 - MUX5)));
#endif

		// Start the conversion
		ADCSRA |= _BV(ADSC);

		MHV_AD_ENABLE_INTERRUPT;
	}

	/**
	 * Set the ADC clock prescaler
	 * @param	prescaler	the prescaler to use
	 */
	void MHV_ADC::setPrescaler(AD_PRESCALER prescaler) {
		ADCSRA = (ADCSRA & 0xf8) | (prescaler & 0x7);
	}

	/**
	 * Call from the main loop to handle any events
	 */
	void MHV_ADC::handleEvents() {
		uint8_t i;

		if (_adcChannel != -1) {
			for (i = 0; i < events; i++) {
				if (_adcs[i].channel == _adcChannel) {
					_adcChannel = -1;
					_adcs[i].listener->adc(_adcs[i].channel, _adcValue);
					break;
				}
			}
		}
	}
};

}

#endif /* MHV_ADC_H_ */
