/*
 * MHV_AD.cpp
 *
 *  Created on: 04/04/2010
 *      Author: deece
 */

#include "MHV_AD.h"

uint16_t mhv_ad_busyRead(uint8_t channel, uint8_t reference) {
	ADMUX = reference | (channel & 0x0F);

#ifdef __AVR_ATmega1280__
	if (channel & _BV(MUX5)) {
		ADCSRB |= _BV(MUX5);
	} else {
		ADCSRB &= ~_BV(MUX5);
	}
#endif

// Start the conversion
	ADCSRA |= _BV(ADSC);

	while (ADCSRA & _BV(ADSC)) {};

	return ADC;
}

void mhv_ad_asyncRead(uint8_t channel, uint8_t reference) {
	ADMUX = reference | (channel & 0x0F);

#ifdef __AVR_ATmega1280__
	if (channel & _BV(MUX5)) {
		ADCSRB |= _BV(MUX5);
	} else {
		ADCSRB &= ~_BV(MUX5);
	}
#endif

// Start the conversion
	ADCSRA |= _BV(ADSC);
}

void mhv_ad_setPrescaler(MHV_AD_PRESCALER prescaler) {
	ADCSRA = (ADCSRA & 0xf8) | (prescaler & 0x7);
}
