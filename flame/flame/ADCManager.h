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

#ifndef FLAME_ADCMANAGER_H_
#define FLAME_ADCMANAGER_H_

#include <flame/io.h>
#include <flame/AD.h>

#include <flame/Device_TX.h>

namespace flame {

#define FLAME_ADC_ASSIGN_INTERRUPT(__flameADCManager) \
ISR(ADC_vect) { \
	__flameADCManager.adc(); \
}

/**
 * Create a new ADC object
 * @param	_flameObjectName	the variable name of the object
 * @param	_flameMaxChannels	the maximum number of channels the ADC needs to support
 * @param	_prescaler		the prescaler to run the ADC at
 */
#define FLAME_ADC_CREATE(_flameObjectName, _flameMaxChannels, _prescaler) \
		ADCManagerImplementation<_flameMaxChannels> _flameObjectName(_prescaler); \
		FLAME_ADC_ASSIGN_INTERRUPT(_flameObjectName);

typedef float Voltage;

INLINE ADCChannel& operator+(ADCChannel &channel, int increment) {
	channel = static_cast<ADCChannel>(static_cast<int>(channel) + increment);
	return channel;
}

INLINE ADCChannel& operator-(ADCChannel &channel, int decrement) {
	channel = static_cast<ADCChannel>(static_cast<int>(channel) - decrement);
	return channel;
}

class ADCListener {
public:
	/**
	 * Does the listener want voltage?
	 * @param channel	the channel that was read from
	 * @param referenceVoltage	the reference Voltage the reading was taken against
	 * @return true for voltage, false for counts
	 */
	virtual bool wantVoltage(ADCChannel UNUSED(channel), Voltage UNUSED(referenceVoltage)) {
		return false;
	}

	/**
	 * Handle incoming ADC reads by count
	 * @param channel	the channel that was read from
	 * @param value		the value read from the channel
	 */
	virtual void adc(ADCChannel UNUSED(channel), int16_t UNUSED(value)) {};

	/**
	 * Handle incoming ADC reads by Voltage
	 * @pre ADCManager.read() must be provided with a voltage
	 * @param channel	the channel that was read from
	 * @param value		the value read from the channel
	 */
	virtual void adc(ADCChannel UNUSED(channel), Voltage UNUSED(value)) {};

};

struct eventADC {
	ADCChannel channel;
	ADCListener *listener;
	int8_t		offset;
};
typedef struct eventADC EVENT_ADC;


class ADCManager {
protected:
	volatile int16_t	_adcValue;
	volatile ADCChannel	_channel;
	Voltage 			_referenceVoltage = 0;
	EVENT_ADC 			*_adcs;
	uint8_t				_eventCount;
	bool				_noiseReduction;

public:
	ADCManager(EVENT_ADC *adcs, uint8_t eventCount, ADCPrescaler prescaler, bool noiseReduction = false);
	void adc();
	virtual int16_t measureDifferentialOffset(ADCChannel channel, ADCReference reference);
	void saveDifferentialOffset(ADCChannel channel, ADCReference reference);
	void registerListener(ADCChannel channel, ADCListener &listener);
	void registerListener(ADCChannel channel, ADCListener *listener);
	void deregisterListener(ADCChannel channel);
	int16_t busyRead(ADCChannel channel, ADCReference reference);
	void read(ADCChannel channel, ADCReference reference, Voltage voltage);
	void read(ADCChannel channel, ADCReference reference);
	void read();
	void setPrescaler(ADCPrescaler prescaler);
	void handleEvents();
	virtual float voltsPerCount(ADCChannel channel, Voltage referenceVoltage);
}; // class ADCManager

/**
 * An event manager for ADC events
 * @tparam	events	the number of ADC events we can handle
 */
template<uint8_t events>
class ADCManagerImplementation : public ADCManager {
protected:
	EVENT_ADC _myAdcs[events];

public:
	/**
	 * An event manager for ADC events
	 * @param prescaler	the prescaler to run the ADC at
	 */
	ADCManagerImplementation(ADCPrescaler prescaler) :
			ADCManager(_myAdcs, events, prescaler) {}
}; // class ADCManagerImplementation

class ADCManagerOversampling : ADCManager {
protected:
	uint8_t				_oversampleBits = 0;
	uint16_t			_oversampleTotal = 1;
	uint16_t			_oversampleCount = 0;

public:
	ADCManagerOversampling(EVENT_ADC *adcs, uint8_t eventCount, ADCPrescaler prescaler, uint8_t bits = 10, bool noiseReduction = false);
	int16_t measureDifferentialOffset(ADCChannel channel, ADCReference reference);
	void setOversample(uint8_t bits);
	virtual float voltsPerCount(ADCChannel channel, Voltage referenceVoltage);
	void adc();
	void handleEvents();

};

/**
 * An event manager for Oversampling ADC events
 * @tparam	events	the number of ADC events we can handle
 * @tparam	noiseReduction	true to enter ADC noise reduction sleep on read()
 */
template<uint8_t events>
class ADCManagerOversamplingImplementation : public ADCManagerOversampling {
protected:
	EVENT_ADC _myAdcs[events];

public:
	/**
	 * An event manager for ADC events
	 * @param prescaler	the prescaler to run the ADC at
	 */
	ADCManagerOversamplingImplementation(ADCPrescaler prescaler) :
			ADCManagerOversampling(_myAdcs, events, prescaler) {}
}; // class ADCManagerOversamplingImplementation


}// namespace flame

#endif /* FLAME_ADCMANAGER_H_ */
