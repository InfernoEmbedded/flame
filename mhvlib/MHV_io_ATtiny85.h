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


#ifndef MHV_IO_ATTINY85_H_
#define MHV_IO_ATTINY85_H_

#include <avr/io.h>

// 8 bit timers
//					 type,                        ctrlRegA,ctrlRegB,overflow1,overflow2,counter,interrupt,intEnable
#define MHV_TIMER8_0 MHV_TIMER_TYPE_5_PRESCALERS, &TCCR0A, &TCCR0B, &OCR0A,   &OCR0B,   &TCNT0, &TIMSK,   OCIE0A
#define MHV_TIMER8_1 MHV_TIMER_TYPE_5_PRESCALERS, &TCCR1A, &TCCR1B, &OCR1A,   &OCR1B,   &TCNT1, &TIMSK,   OCIE1A

#define MHV_TIMER0_INTERRUPTS SIG_OUTPUT_COMPARE0A, SIG_OUTPUT_COMPARE0B
#define MHV_TIMER1_INTERRUPTS SIG_OUTPUT_COMPARE1A, SIG_OUTPUT_COMPARE1B

#define MHV_AD_REFERENCE_VCC	0x00 << 4
#define MHV_AD_REFERENCE_AREF	0x04 << 4
#define MHV_AD_REFERENCE_1V1	0x08 << 6
#define MHV_AD_REFERENCE_2V56	0x09 << 6
#define MHV_AD_REFERENCE_2V56_AREF	0x0d << 6

#define MHV_AD_CHANNEL_0			0x00
#define MHV_AD_CHANNEL_1			0x01
#define MHV_AD_CHANNEL_2			0x02
#define MHV_AD_CHANNEL_3			0x03
#define MHV_AD_CHANNEL_2_X1_2		0x04
#define MHV_AD_CHANNEL_2_X20_2		0x05
#define MHV_AD_CHANNEL_2_X1_3		0x06
#define MHV_AD_CHANNEL_2_X20_3		0x07
#define MHV_AD_CHANNEL_0_X1_0		0x08
#define MHV_AD_CHANNEL_0_X20_0		0x09
#define MHV_AD_CHANNEL_0_X1_1		0x0a
#define MHV_AD_CHANNEL_0_X20_1		0x0b
#define MHV_AD_V_BANDGAP			0x0c
#define MHV_AD_0V					0x0d
#define MHV_AD_TEMPERATURE			0x0f

// Power reduction register for ADC
#define MHV_AD_PRR	PRR


//					Dir,	Output,	Input,	Bit,PCINT
#define MHV_PIN_B0	&DDRB,	&PORTB, &PINB,	0,	0
#define MHV_PIN_B1	&DDRB,	&PORTB, &PINB,	1,	1
#define MHV_PIN_B2	&DDRB,	&PORTB, &PINB,	2,	2
#define MHV_PIN_B3	&DDRB,	&PORTB, &PINB,	3,	3
#define MHV_PIN_B4	&DDRB,	&PORTB, &PINB,	4,	4
#define MHV_PIN_B5	&DDRB,	&PORTB, &PINB,	5,	5

#define MHV_PIN_TIMER_0_A	MHV_PIN_B0
#define MHV_PIN_TIMER_0_B	MHV_PIN_B1
#define MHV_PIN_TIMER_1_A	MHV_PIN_B1
#define MHV_PIN_TIMER_1_B	MHV_PIN_B4

#define MHV_INTERRUPT_INT0 INT0_vect, &MCUCR, ISC00

#define MHV_PC_INT_COUNT		6

#endif /* MHV_IO_ATTINY85_H_ */
