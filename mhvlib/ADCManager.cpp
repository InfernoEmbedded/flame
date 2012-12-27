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

#include <mhvlib/ADCManager.h>

namespace mhvlib {

/**
 * An event manager for ADC events
 * @param eventsBuffer	storage for channel event data
 * @param eventCount	the maximum number of events to handle
 * @param prescaler		the prescaler to run the ADC at
 */

ADCManager::ADCManager(EVENT_ADC *eventsBuffer, uint8_t eventCount, AD_PRESCALER prescaler) :
		_adcChannel(-1) {
	_adcs = eventsBuffer;
	_eventCount = eventCount;

	for (uint8_t i = 0; i < _eventCount; i++) {
		_adcs[i].channel = -1;
		_adcs[i].listener = NULL;
	}

	setPrescaler(prescaler);
	MHV_AD_ENABLE;
}

/**
 * Interrupt handler to read the ADC
 */
void ADCManager::adc() {
	_adcValue = ADC;
	_adcChannel = MHV_AD_CHANNEL;

	MHV_AD_DISABLE_INTERRUPT;
}


/**
 * Register interest for an ADC channel
 * @param	channel		the ADC channel
 * @param	listener	an ADCListener to notify when an ADC reading has been completed
 */
void ADCManager::registerListener(int8_t channel, ADCListener &listener) {
	for (uint8_t i = 0; i < _eventCount; i++) {
		if (_adcs[i].channel == -1) {
			_adcs[i].channel = channel;
			_adcs[i].listener = &listener;
			break;
		}
	}
}


/**
 * Register interest for an ADC channel
 * @param	channel		the ADC channel
 * @param	listener	an ADCListener to notify when an ADC reading has been completed
 */
void ADCManager::registerListener(int8_t channel, ADCListener *listener) {
	for (uint8_t i = 0; i < _eventCount; i++) {
		if (_adcs[i].channel == -1) {
			_adcs[i].channel = channel;
			_adcs[i].listener = listener;
			break;
		}
	}
}


/**
 * Deregister interest for an ADC channel
 * @param	channel		the ADC channel
 */
void ADCManager::deregisterListener(int8_t channel) {
	for (uint8_t i = 0; i < _eventCount; i++) {
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
int16_t ADCManager::busyRead(int8_t channel, uint8_t reference) {
	ADMUX = reference | (channel & 0x0F);

#ifdef MUX5
	ADCSRB = (ADCSRB & ~_BV(MUX5)) | ((channel & _BV(5) >> (5-MUX5)));
#endif

	// Start the conversion
	ADCSRA |= _BV(ADSC);

	MHV_AD_ENABLE_INTERRUPT;

	while (ADCSRA & _BV(ADSC)) {};

	return ADC;
}


/**
 * Trigger an ADC channel event
 * @param	channel		the channel to read
 * @param	reference	the voltage reference to use
 */
void ADCManager::read(int8_t channel, uint8_t reference) {
	ADMUX = reference | (channel & 0x0F);

#ifdef MUX5
	ADCSRB = (ADCSRB & ~_BV(MUX5)) | ((channel & _BV(5) >> (5-MUX5)));
#endif

	// Start the conversion
	ADCSRA |= _BV(ADSC);

	MHV_AD_ENABLE_INTERRUPT;
}


/**
 * Set the ADC clock prescaler
 * @param	prescaler	the prescaler to use
 */
void ADCManager::setPrescaler(AD_PRESCALER prescaler) {
	ad_setPrescaler(prescaler);
}


/**
 * Call from the main loop to handle any events
 */
void ADCManager::handleEvents() {
	uint8_t i;

	if (_adcChannel != -1) {
		for (i = 0; i < _eventCount; i++) {
			if (_adcs[i].channel == _adcChannel) {
				_adcChannel = -1;
				_adcs[i].listener->adc(_adcs[i].channel, _adcValue);
				break;
			}
		}
	}
}

}