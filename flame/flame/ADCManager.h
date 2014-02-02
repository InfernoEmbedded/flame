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

namespace flame{

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



class ADCListener {
public:
	/**
	 * Handle incoming ADC reads
	 * @param channel	the channel that was read from
	 * @param value		the value read from the channel
	 */
	virtual void adc(ADCChannel channel, uint16_t value) =0;
};

struct eventADC {
	ADCChannel channel;
	ADCListener *listener;
};
typedef struct eventADC EVENT_ADC;


class ADCManager {
protected:
	volatile uint16_t	_adcValue;
	volatile ADCChannel	_channel;
	EVENT_ADC 			*_adcs;
	uint8_t				_eventCount;

public:
	ADCManager(EVENT_ADC *adcs, uint8_t adcCount, ADCPrescaler prescaler);
	void adc();
	void registerListener(ADCChannel channel, ADCListener &listener);
	void registerListener(ADCChannel channel, ADCListener *listener);
	void deregisterListener(ADCChannel channel);
	int16_t busyRead(ADCChannel channel, ADCReference reference);
	void read(ADCChannel channel, ADCReference reference);
	void setPrescaler(ADCPrescaler prescaler);
	void handleEvents();
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
}; // class ADCManager

}// namespace flame

#endif /* FLAME_ADCMANAGER_H_ */
