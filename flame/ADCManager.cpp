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

#include <flame/ADCManager.h>
#include <avr/sleep.h>

#define OFFSET_OVERSAMPLING		64

namespace flame {

/**
 * An event manager for ADC events
 * @param eventsBuffer	storage for channel event data
 * @param eventCount	the maximum number of events to handle
 * @param prescaler		the prescaler to run the ADC at
 * @param noiseReduction	true to enable ADC noise reduction sleep on read()
 */
ADCManager::ADCManager(EVENT_ADC *eventsBuffer, uint8_t eventCount, ADCPrescaler prescaler, bool noiseReduction) :
		_channel(ADCChannel::UNDEFINED),
		_adcs(eventsBuffer),
		_eventCount(eventCount),
		_noiseReduction(noiseReduction) {

	for (uint8_t i = 0; i < _eventCount; i++) {
		_adcs[i].channel = ADCChannel::UNDEFINED;
		_adcs[i].listener = NULL;
		_adcs[i].offset = 0;
	}

	setPrescaler(prescaler);
	FLAME_AD_ENABLE;
}

/**
 * Interrupt handler to read the ADC
 */
void ADCManager::adc() {
	_adcValue = ADC;
	_channel = (ADCChannel)FLAME_AD_CHANNEL;

	FLAME_AD_DISABLE_INTERRUPT;
}

/**
 * Measure the offset for a differential channel
 * @param channel	the channel to measure the offset for (must be a differential channel)
 * @param reference	the reference to use
 * @fixme restructure to use ADC noise reduction sleep mode
 */
int16_t ADCManager::measureDifferentialOffset(ADCChannel channel, ADCReference reference) {
	ADCChannel offsetChannel = channel - 1;

	// ignore the first read as the values may not be accurate
	busyRead(offsetChannel, reference);

	/* Worst case accuracy of the ADC is 7 bits in 200x mode, so we will oversample to bring the effective accuracy
	 * to 10 bits
	 */
	int16_t accumulator = 0;
	for (uint8_t sample = 0; sample < OFFSET_OVERSAMPLING; sample++) {
		accumulator = busyRead (offsetChannel, reference);
	}

	accumulator /= OFFSET_OVERSAMPLING;

	return accumulator;
}

/**
 * Measure and save the offset for a differential channel
 * @pre a listener for the channel must be registered
 * @param channel	the channel to measure the offset for (must be a differential channel)
 * @param reference	the reference to use
 */
void ADCManager::saveDifferentialOffset(ADCChannel channel, ADCReference reference) {
	for (uint8_t i = 0; i < _eventCount; i++) {
		if (_adcs[i].channel == channel) {
			_adcs[i].offset = measureDifferentialOffset(channel, reference);
			break;
		}
	}
}

/**
 * Register interest for an ADC channel
 * @param	channel		the ADC channel
 * @param	listener	an ADCListener to notify when an ADC reading has been completed
 */
void ADCManager::registerListener(ADCChannel channel, ADCListener &listener) {
	for (uint8_t i = 0; i < _eventCount; i++) {
		if (_adcs[i].channel == ADCChannel::UNDEFINED) {
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
void ADCManager::registerListener(ADCChannel channel, ADCListener *listener) {
	for (uint8_t i = 0; i < _eventCount; i++) {
		if (_adcs[i].channel == ADCChannel::UNDEFINED) {
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
void ADCManager::deregisterListener(ADCChannel channel) {
	for (uint8_t i = 0; i < _eventCount; i++) {
		if (_adcs[i].channel == channel) {
			_adcs[i].channel = ADCChannel::UNDEFINED;
			break;
		}
	}
}


/**
 * Read an ADC channel
 * @param	channel		the channel to read
 * @param	reference	the voltage reference to use
 */
int16_t ADCManager::busyRead(ADCChannel channel, ADCReference reference) {
	ADMUX = reference | (channel & 0x0F);

#ifdef MUX5
	ADCSRB = (ADCSRB & ~_BV(MUX5)) | ((channel & _BV(5) >> (5-MUX5)));
#endif

	// Start the conversion
	ADCSRA |= _BV(ADSC);

	FLAME_AD_ENABLE_INTERRUPT;

	while (ADCSRA & _BV(ADSC)) {};

	return ADC;
}

/**
 * Trigger an ADC channel event
 * @param	channel		the channel to read
 * @param	reference	the voltage reference to use
 */
void ADCManager::read(ADCChannel channel, ADCReference reference) {
	ADMUX = (uint8_t)reference | ((uint8_t)channel & 0x0F);

#ifdef MUX5
	ADCSRB = (ADCSRB & ~_BV(MUX5)) | ((channel & _BV(5) >> (5-MUX5)));
#endif

	read();
}

/**
 * Trigger an ADC channel event, saving the reference voltage for Voltage based listeners
 * @param	channel		the channel to read
 * @param	reference	the voltage reference to use
 */
void ADCManager::read(ADCChannel channel, ADCReference reference, Voltage referenceVoltage) {
	_referenceVoltage = referenceVoltage;
	read(channel, reference);
}


/**
 * Retrigger an ADC channel event on the channel/reference previously selected by read()
 */
void ADCManager::read() {
	FLAME_AD_ENABLE_INTERRUPT;

	_adcValue = 0;

	// Start the conversion
	if (_noiseReduction) {
		set_sleep_mode(SLEEP_MODE_ADC);
		sleep_mode();
	} else {
		ADCSRA |= _BV(ADSC);
	}
}

/**
 * Set the ADC clock prescaler
 * @param	prescaler	the prescaler to use
 */
void ADCManager::setPrescaler(ADCPrescaler prescaler) {
	ad_setPrescaler(prescaler);
}

/**
 * Determine the Voltage per ADC count
 * @param channel	the ADC channel in question
 * @param reference	the ADC reference voltage
 * @return the Voltage per ADC count
 */
float CONST ADCManager::voltsPerCount(ADCChannel channel, Voltage referenceVoltage) {
	uint8_t multiplier;

	switch (channel) {
	case ADCChannel::CHANNEL_0_X10_0:
	case ADCChannel::CHANNEL_1_X10_0:
	case ADCChannel::CHANNEL_2_X10_2:
	case ADCChannel::CHANNEL_3_X10_2:
	case ADCChannel::CHANNEL_8_X10_8:
	case ADCChannel::CHANNEL_9_X10_8:
	case ADCChannel::CHANNEL_10_X10_10:
	case ADCChannel::CHANNEL_11_X10_10:
		multiplier = 10;
		break;
	case ADCChannel::CHANNEL_0_X200_0:
	case ADCChannel::CHANNEL_1_X200_0:
	case ADCChannel::CHANNEL_2_X200_2:
	case ADCChannel::CHANNEL_3_X200_2:
	case ADCChannel::CHANNEL_8_X200_8:
	case ADCChannel::CHANNEL_9_X200_8:
	case ADCChannel::CHANNEL_10_X200_10:
	case ADCChannel::CHANNEL_11_X200_10:
		multiplier = 200;
		break;
	default:
		multiplier = 1;
		break;
	}

#define COUNTS	1024
	return referenceVoltage / (COUNTS * multiplier);
}

/**
 * Call from the main loop to handle any events
 */
void ADCManager::handleEvents() {
	uint8_t i;

	if (_channel != ADCChannel::UNDEFINED) {
		for (i = 0; i < _eventCount; i++) {
			if (_adcs[i].channel == _channel) {
				_channel = ADCChannel::UNDEFINED;
				if (_adcs[i].listener->wantVoltage(_adcs[i].channel, _referenceVoltage)) {
					Voltage voltage = (_adcValue - _adcs[i].offset) * voltsPerCount(_adcs[i].channel, _referenceVoltage);
					_adcs[i].listener->adc(_adcs[i].channel, voltage);
				} else {
					_adcs[i].listener->adc(_adcs[i].channel, _adcValue - _adcs[i].offset);
				}
				break;
			}
		}
	}
}

/**
 * An event manager for oversampling ADC events
 * @param eventsBuffer	storage for channel event data
 * @param eventCount	the maximum number of events to handle
 * @param prescaler		the prescaler to run the ADC at
 * @param bits			the number of additional bits of resolution desired
 * @see setOversample
 */
ADCManagerOversampling::ADCManagerOversampling(EVENT_ADC *eventsBuffer, uint8_t eventCount, ADCPrescaler prescaler,
		uint8_t bits, bool noiseReduction) :
		ADCManager(eventsBuffer, eventCount, prescaler, noiseReduction) {
	setOversample(bits);
}

/**
 * Measure the offset for a differential channel
 * @param channel	the channel to measure the offset for (must be a differential channel)
 * @param reference	the reference to use
 * @fixme restructure to use ADC noise reduction sleep mode
 */
int16_t ADCManagerOversampling::measureDifferentialOffset(ADCChannel channel, ADCReference reference) {
	ADCChannel offsetChannel = channel - 1;

	// ignore the first read as the values may not be accurate
	busyRead(offsetChannel, reference);

	int16_t accumulator = 0;
	for (uint8_t sample = 0; sample < _oversampleTotal; sample++) {
		accumulator = busyRead(offsetChannel, reference);
	}

	return accumulator;
}


/**
 * Set the amount of oversampling
 * Oversampling can increase the effective resolution as per the following table
 *  Bits		Samples
 *  0			1
 *  1			4
 *  2			16
 *  3			64
 *  4			256
 *  5			1024
 *  6			4096
 *  7			8192
 * See http://en.wikipedia.org/wiki/Oversampling for more information
 * @param bits	the number of additional bits of resolution desired
 */
void ADCManagerOversampling::setOversample(uint8_t bits) {
	switch (bits) {
	case 0:
		_oversampleTotal = 1;
		_oversampleBits = 0;
		break;
	case 1:
		_oversampleTotal = 4;
		_oversampleBits = 1;
		break;
	case 2:
		_oversampleTotal = 16;
		_oversampleBits = 1;
		break;
	case 3:
		_oversampleTotal = 64;
		_oversampleBits = 2;
		break;
	case 4:
		_oversampleTotal = 256;
		_oversampleBits = 4;
		break;
	case 5:
		_oversampleTotal = 1024;
		_oversampleBits = 5;
		break;
	case 6:
		_oversampleTotal = 4096;
		_oversampleBits = 6;
		break;
	default:
		_oversampleTotal = 8192;
		_oversampleBits = 7;
		break;
	}
}

/**
 * Determine the Voltage per ADC count
 * @param channel	the ADC channel in question
 * @param reference	the ADC reference voltage
 * @return the Voltage per ADC count
 */
float PURE ADCManagerOversampling::voltsPerCount(ADCChannel channel, Voltage referenceVoltage) {
	return ADCManager::voltsPerCount(channel, referenceVoltage) / _oversampleTotal;
}


/**
 * Interrupt handler to read the ADC
 */
void ADCManagerOversampling::adc() {
	ADCManager::_adcValue += ADC;
	ADCManager::_channel = (ADCChannel)FLAME_AD_CHANNEL;

	if (_oversampleCount++ >= _oversampleTotal) {
		FLAME_AD_DISABLE_INTERRUPT;
	} else {
		ADCManager::read();
	}
}

/**
 * Call from the main loop to handle any events
 */
void ADCManagerOversampling::handleEvents() {
	if (_oversampleCount++ >= _oversampleTotal) {
		ADCManager::handleEvents();
		_oversampleCount = 0;
	}
}


}
