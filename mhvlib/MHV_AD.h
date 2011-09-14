/* Copyright (c) 2011, Make, Hack, Void Inc
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

#ifndef MHV_AD_H_
#define MHV_AD_H_

#include <MHV_io.h>

// Maximum value of the ADC
#define MHV_AD_MAX	1024

enum mhv_ad_prescaler {
	MHV_AD_PRESCALER_2   = 1,
	MHV_AD_PRESCALER_4   = 2,
	MHV_AD_PRESCALER_8   = 3,
	MHV_AD_PRESCALER_16  = 4,
	MHV_AD_PRESCALER_32  = 5,
	MHV_AD_PRESCALER_64  = 6,
	MHV_AD_PRESCALER_128 = 7
};
typedef enum mhv_ad_prescaler MHV_AD_PRESCALER;

// helper macros for adc trigger functions
#define MHV_AD_REFERENCE (ADMUX & 0xF0)
#ifdef __AVR_ATmega1280__
#define MHV_AD_CHANNEL ((ADMUX & 0x0F) | (ADCSRB & _BV(MUX5)))
#else
#define MHV_AD_CHANNEL (ADMUX & 0x0F)
#endif

// trigger func takes uint16_t parm value
#define MHV_AD_ASSIGN_INTERRUPT(adcTrigger) \
ISR(ADC_vect) { \
	adcTrigger(ADC); \
}

// Enable/Disable interrupts for busyRead
#define MHV_AD_ENABLE_INTERRUPT		ADCSRA |= _BV(ADIE)
#define MHV_AD_DISABLE_INTERRUPT	ADCSRA &= ~_BV(ADIE)

// Enable/Disable the ADC
#define MHV_AD_ENABLE \
	do { \
		MHV_AD_PRR &= ~_BV(PRADC); \
		ADCSRA |= _BV(ADEN); \
	} while (0)

#define MHV_AD_DISABLE \
	do { \
		MHV_AD_PRR |= _BV(PRADC); \
		ADCSRA |= _BV(ADEN); \
	} while (0)

uint16_t mhv_ad_busyRead(uint8_t channel, uint8_t reference);
void mhv_ad_asyncRead(uint8_t channel, uint8_t reference);
void mhv_ad_setPrescaler(MHV_AD_PRESCALER prescaler);

#endif /* MHV_AD_H_ */
