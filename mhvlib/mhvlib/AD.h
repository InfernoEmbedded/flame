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

#include <mhvlib/io.h>

#ifdef ADC

// Maximum value of the ADC
#define MHV_AD_MAX			1023
#define MHV_AD_RESOLUTION	1024


// helper macros for adc trigger functions
#define MHV_AD_REFERENCE (ADMUX & 0xF0)
#ifdef MUX5
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
#ifdef PRR
#define MHV_AD_ENABLE \
	do { \
		PRR &= ~_BV(PRADC); \
		ADCSRA |= _BV(ADEN); \
	} while (0)

#define MHV_AD_DISABLE \
	do { \
		PRR |= _BV(PRADC); \
		ADCSRA |= _BV(ADEN); \
	} while (0)
#elif defined PRR0
#define MHV_AD_ENABLE \
	do { \
		PRR0 &= ~_BV(PRADC); \
		ADCSRA |= _BV(ADEN); \
	} while (0)

#define MHV_AD_DISABLE \
	do { \
		PRR0 |= _BV(PRADC); \
		ADCSRA |= _BV(ADEN); \
	} while (0)
#endif

namespace mhvlib {
enum class ad_prescaler : uint8_t {
	DIVIDE_BY_2   = 1,
	DIVIDE_BY_4   = 2,
	DIVIDE_BY_8   = 3,
	DIVIDE_BY_16  = 4,
	DIVIDE_BY_32  = 5,
	DIVIDE_BY_64  = 6,
	DIVIDE_BY_128 = 7
};
typedef enum ad_prescaler AD_PRESCALER;
INLINE uint8_t operator& (AD_PRESCALER prescaler, uint8_t andedWith) {
	const uint8_t myPrescaler = static_cast<uint8_t>(prescaler);
	return(myPrescaler & andedWith);
}


uint16_t ad_busyRead(uint8_t channel, uint8_t reference);
void ad_asyncRead(uint8_t channel, uint8_t reference);
void ad_setPrescaler(AD_PRESCALER prescaler);
}

#endif // ADC
#endif /* MHV_AD_H_ */
