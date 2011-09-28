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

#include <MHV_io.h>
#include <MHV_AD.h>


#define MHV_ADC_ASSIGN_INTERRUPT(__mhvADCManager) \
ISR(ADC_vect) { \
	__mhvADCManager.adc(); \
}


class MHV_ADCListener {
public:
	virtual void adc(uint8_t adcChannel, uint16_t adcValue) =0;
};

struct mhv_eventADC {
	uint8_t					channel;
	MHV_ADCListener			*listener;
};
typedef struct mhv_eventADC	MHV_EVENT_ADC;

class MHV_ADC {
protected:
	uint16_t		_adcValue;
	int8_t			_adcChannel;
	MHV_EVENT_ADC	*_adcs;
	uint8_t			_adcCount;


public:
	MHV_ADC(MHV_EVENT_ADC *adcs, uint8_t adcCount);

// ADC
	void adc();
	void registerListener(uint8_t channel, MHV_ADCListener *listener);
	void deregisterListener(uint8_t channel);
	void enable();
	void disable();
	void busyRead(uint8_t channel, uint8_t reference);
	void asyncRead(uint8_t channel, uint8_t reference);
	void setPrescaler(MHV_AD_PRESCALER prescaler);

	void handleEvents();
};

#endif /* MHV_ADC_H_ */
