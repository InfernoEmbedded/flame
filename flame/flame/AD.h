/* Copyright (c) 2014, Inferno Embedded
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of the Inferno Embedded nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL INFERNO EMBEDDED BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FLAME_AD_H_
#define FLAME_AD_H_

#include <flame/io.h>

#ifdef ADC

// Maximum value of the ADC
#define FLAME_AD_MAX			1023
#define FLAME_AD_RESOLUTION	1024


// helper macros for adc trigger functions
#define FLAME_AD_REFERENCE (ADMUX & 0xF0)
#ifdef MUX5
#define FLAME_AD_CHANNEL ((ADMUX & 0x0F) | (ADCSRB & _BV(MUX5)))
#else
#define FLAME_AD_CHANNEL (ADMUX & 0x0F)
#endif

// trigger func takes uint16_t parm value
#define FLAME_AD_ASSIGN_INTERRUPT(adcTrigger) \
ISR(ADC_vect) { \
	adcTrigger(ADC); \
}

// Enable/Disable interrupts for busyRead
#define FLAME_AD_ENABLE_INTERRUPT		ADCSRA |= _BV(ADIE)
#define FLAME_AD_DISABLE_INTERRUPT	ADCSRA &= ~_BV(ADIE)

// Enable/Disable the ADC
#ifdef PRR
#define FLAME_AD_ENABLE \
	do { \
		PRR &= ~_BV(PRADC); \
		ADCSRA |= _BV(ADEN); \
	} while (0)

#define FLAME_AD_DISABLE \
	do { \
		PRR |= _BV(PRADC); \
		ADCSRA |= _BV(ADEN); \
	} while (0)
#elif defined PRR0
#define FLAME_AD_ENABLE \
	do { \
		PRR0 &= ~_BV(PRADC); \
		ADCSRA |= _BV(ADEN); \
	} while (0)

#define FLAME_AD_DISABLE \
	do { \
		PRR0 |= _BV(PRADC); \
		ADCSRA |= _BV(ADEN); \
	} while (0)
#endif

namespace flame {
enum class ADCPrescaler : uint8_t {
	DIVIDE_BY_2   = 1,
	DIVIDE_BY_4   = 2,
	DIVIDE_BY_8   = 3,
	DIVIDE_BY_16  = 4,
	DIVIDE_BY_32  = 5,
	DIVIDE_BY_64  = 6,
	DIVIDE_BY_128 = 7
};
INLINE uint8_t operator& (ADCPrescaler prescaler, uint8_t andedWith) {
	const uint8_t myPrescaler = static_cast<uint8_t>(prescaler);
	return (myPrescaler & andedWith);
}

INLINE ADCChannel operator+ (ADCChannel channel, uint8_t increment) {
	const uint8_t myChannel = static_cast<uint8_t>(channel);
	return static_cast<ADCChannel>(myChannel + increment);
}

INLINE uint8_t operator& (ADCChannel channel, uint8_t andedWith) {
	const uint8_t myChannel = static_cast<uint8_t>(channel);
	return (myChannel & andedWith);
}

INLINE uint8_t operator& (ADCReference ref, uint8_t andedWith) {
	const uint8_t myRef = static_cast<uint8_t>(ref);
	return (myRef & andedWith);
}

INLINE uint8_t operator| (ADCReference ref, uint8_t oredWith) {
	const uint8_t myRef = static_cast<uint8_t>(ref);
	return (myRef | oredWith);
}


uint16_t ad_busyRead(ADCChannel channel, ADCReference reference);
void ad_startRead(ADCChannel channel, ADCReference reference);
void ad_asyncRead(ADCChannel channel, ADCReference reference);
void ad_setPrescaler(ADCPrescaler prescaler);
} // End of namespace flame

#endif // ADC
#endif /* FLAME_AD_H_ */
