/*
 * MHV_AD.h
 *
 *  Created on: 04/04/2010
 *      Author: deece
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

// trigger func takes uint8_t parm value
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
