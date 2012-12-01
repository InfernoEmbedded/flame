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

#ifndef MHV_ADCMANAGER_H_
#define MHV_ADCMANAGER_H_

#include <mhvlib/io.h>
#include <mhvlib/AD.h>

#include <mhvlib/Device_TX.h>

namespace mhvlib {

#define MHV_ADC_ASSIGN_INTERRUPT(__mhvADCManager) \
ISR(ADC_vect) { \
	__mhvADCManager.adc(); \
}

/**
 * Create a new ADC object
 * @param	_mhvObjectName	the variable name of the object
 * @param	_mhvMaxChannels	the maximum number of channels the ADC needs to support
 * @param	_prescaler		the prescaler to run the ADC at
 */
#define MHV_ADC_CREATE(_mhvObjectName, _mhvMaxChannels, _prescaler) \
		ADCManagerImplementation<_mhvMaxChannels> _mhvObjectName(_prescaler); \
		MHV_ADC_ASSIGN_INTERRUPT(_mhvObjectName);

class ADCListener {
public:
	/**
	 * Handle incoming ADC reads
	 * @param adcChannel	the channel that was read from
	 * @param adcValue		the value read from the channel
	 */
	virtual void adc(uint8_t adcChannel, uint16_t adcValue) =0;
};

struct eventADC {
	int8_t channel;
	ADCListener *listener;
};
typedef struct eventADC EVENT_ADC;


class ADCManager {
protected:
	volatile uint16_t	_adcValue;
	volatile int8_t		_adcChannel;
	EVENT_ADC 			*_adcs;
	uint8_t				_eventCount;

public:
	ADCManager(EVENT_ADC *adcs, uint8_t adcCount, AD_PRESCALER prescaler);
	void adc();
	void registerListener(int8_t channel, ADCListener &listener);
	void registerListener(int8_t channel, ADCListener *listener);
	void deregisterListener(int8_t channel);
	int16_t busyRead(int8_t channel, uint8_t reference);
	void read(int8_t channel, uint8_t reference);
	void setPrescaler(AD_PRESCALER prescaler);
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

	ADCManagerImplementation(AD_PRESCALER prescaler) :
			ADCManager(_myAdcs, events, prescaler) {}
}; // class ADCManager

}// namespace mhvlib

#endif /* MHV_ADCMANAGER_H_ */
