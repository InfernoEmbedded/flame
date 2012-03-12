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


#ifndef MHV_IO_ATMEGA168_H_
#define MHV_IO_ATMEGA168_H_

#include <avr/io.h>

// 8 bit timers
//					 type,                        ctrlRegA,ctrlRegB,overflow1,overflow2,counter,interrupt,intEnable
#define MHV_TIMER8_0 MHV_TIMER_TYPE_5_PRESCALERS, &TCCR0A, &TCCR0B, &OCR0A,   &OCR0B,   &TCNT0, &TIMSK0,  OCIE0A
#define MHV_TIMER8_2 MHV_TIMER_TYPE_7_PRESCALERS, &TCCR2A, &TCCR2B, &OCR2A,   &OCR2B,   &TCNT2, &TIMSK2,  OCIE2A

#define MHV_TIMER0_INTERRUPTS TIMER0_COMPA_vect, TIMER0_COMPB_vect, 0
#define MHV_TIMER2_INTERRUPTS TIMER2_COMPA_vect, TIMER2_COMPB_vect, 0

// 16 bit timers
//					  ctrlRegA,ctrlRegB,ctrlRegC,overflow1,overflow2,overflow3,counter,interrupt,inputCapture1
#define MHV_TIMER16_1 &TCCR1A, &TCCR1B, 0,       &OCR1A,   &OCR1B,   0,        &TCNT1, &TIMSK1,  &ICR1

#define MHV_TIMER1_INTERRUPTS TIMER1_COMPA_vect, TIMER1_COMPB_vect, 0


// USART
//				   ubrr,   ucsra,   ucsrb,   udr,   rxen,  txen,  rxcie,  txcie,  udre,  u2x
#define MHV_USART0 &UBRR0, &UCSR0A, &UCSR0B, &UDR0, RXEN0, TXEN0, RXCIE0, TXCIE0, UDRE0, U2X0
#define MHV_USART0_INTERRUPTS USART_RX_vect, USART_TX_vect

#define MHV_AD_RESOLUTION		1024

#define MHV_AD_REFERENCE_AREF	(uint8_t)(0x00 << 6)
#define MHV_AD_REFERENCE_AVCC	(uint8_t)(0x01 << 6)
#define MHV_AD_REFERENCE_1V1	(uint8_t)(0x03 << 6)

#define MHV_AD_CHANNEL_0			0x00
#define MHV_AD_CHANNEL_1			0x01
#define MHV_AD_CHANNEL_2			0x02
#define MHV_AD_CHANNEL_3			0x03
#define MHV_AD_CHANNEL_4			0x04
#define MHV_AD_CHANNEL_5			0x05
#define MHV_AD_CHANNEL_6			0x06
#define MHV_AD_CHANNEL_7			0x07
#define MHV_AD_CHANNEL_8			0x08
#define MHV_AD_TEMPERATURE			0x08
#define MHV_AD_CHANNEL_1V1			0xfe
#define MHV_AD_CHANNEL_0V			0xff

// Power reduction register for ADC
#define MHV_AD_PRR	PRR

//							Dir,	Output,	Input,	Bit,PCINT
#define MHV_PIN_B0			&DDRB,	&PORTB,	&PINB,	0,	0
#define MHV_PIN_B1			&DDRB,	&PORTB,	&PINB,	1,	1
#define MHV_PIN_B2			&DDRB,	&PORTB,	&PINB,	2,	2
#define MHV_PIN_B3			&DDRB,	&PORTB,	&PINB,	3,	3
#define MHV_PIN_B4			&DDRB,	&PORTB,	&PINB,	4,	4
#define MHV_PIN_B5			&DDRB,	&PORTB,	&PINB,	5,	5
#define MHV_PIN_B6			&DDRB,	&PORTB,	&PINB,	6,	6
#define MHV_PIN_B7			&DDRB,	&PORTB,	&PINB,	7,	7
#define MHV_PIN_C0			&DDRC,	&PORTC,	&PINC,	0,	8
#define MHV_PIN_C1			&DDRC,	&PORTC,	&PINC,	1,	9
#define MHV_PIN_C2			&DDRC,	&PORTC,	&PINC,	2,	10
#define MHV_PIN_C3			&DDRC,	&PORTC,	&PINC,	3,	11
#define MHV_PIN_C4			&DDRC,	&PORTC,	&PINC,	4,	12
#define MHV_PIN_C5			&DDRC,	&PORTC,	&PINC,	5,	13
#define MHV_PIN_C6			&DDRC,	&PORTC,	&PINC,	6,	14
#define MHV_PIN_D0			&DDRD,	&PORTD,	&PIND,	0,	16
#define MHV_PIN_D1			&DDRD,	&PORTD,	&PIND,	1,	17
#define MHV_PIN_D2			&DDRD,	&PORTD,	&PIND,	2,	18
#define MHV_PIN_D3			&DDRD,	&PORTD,	&PIND,	3,	19
#define MHV_PIN_D4			&DDRD,	&PORTD,	&PIND,	4,	20
#define MHV_PIN_D5			&DDRD,	&PORTD,	&PIND,	5,	21
#define MHV_PIN_D6			&DDRD,	&PORTD,	&PIND,	6,	22
#define MHV_PIN_D7			&DDRD,	&PORTD,	&PIND,	7,	23

#define MHV_TEMPLATE_PIN_B0	0x04,	0x05,	0x03,	0,	0
#define MHV_TEMPLATE_PIN_B1	0x04,	0x05,	0x03,	1,	1
#define MHV_TEMPLATE_PIN_B2	0x04,	0x05,	0x03,	2,	2
#define MHV_TEMPLATE_PIN_B3	0x04,	0x05,	0x03,	3,	3
#define MHV_TEMPLATE_PIN_B4	0x04,	0x05,	0x03,	4,	4
#define MHV_TEMPLATE_PIN_B5	0x04,	0x05,	0x03,	5,	5
#define MHV_TEMPLATE_PIN_B6	0x04,	0x05,	0x03,	6,	6
#define MHV_TEMPLATE_PIN_B7	0x04,	0x05,	0x03,	7,	7
#define MHV_TEMPLATE_PIN_C0	0x07,	0x08,	0x06,	0,	8
#define MHV_TEMPLATE_PIN_C1	0x07,	0x08,	0x06,	1,	9
#define MHV_TEMPLATE_PIN_C2	0x07,	0x08,	0x06,	2,	10
#define MHV_TEMPLATE_PIN_C3	0x07,	0x08,	0x06,	3,	11
#define MHV_TEMPLATE_PIN_C4	0x07,	0x08,	0x06,	4,	12
#define MHV_TEMPLATE_PIN_C5	0x07,	0x08,	0x06,	5,	13
#define MHV_TEMPLATE_PIN_C6	0x07,	0x08,	0x06,	6,	14
#define MHV_TEMPLATE_PIN_D0	0x0A,	0x0B,	0x09,	0,	16
#define MHV_TEMPLATE_PIN_D1	0x0A,	0x0B,	0x09,	1,	17
#define MHV_TEMPLATE_PIN_D2	0x0A,	0x0B,	0x09,	2,	18
#define MHV_TEMPLATE_PIN_D3	0x0A,	0x0B,	0x09,	3,	19
#define MHV_TEMPLATE_PIN_D4	0x0A,	0x0B,	0x09,	4,	20
#define MHV_TEMPLATE_PIN_D5	0x0A,	0x0B,	0x09,	5,	21
#define MHV_TEMPLATE_PIN_D6	0x0A,	0x0B,	0x09,	6,	22
#define MHV_TEMPLATE_PIN_D7	0x0A,	0x0B,	0x09,	7,	23

#define MHV_PIN_TIMER_0_A	MHV_PIN_D6
#define MHV_PIN_TIMER_0_B	MHV_PIN_D5
#define MHV_PIN_TIMER_1_A	MHV_PIN_B1
#define MHV_PIN_TIMER_1_B	MHV_PIN_B2
#define MHV_PIN_TIMER_2_A	MHV_PIN_B3
#define MHV_PIN_TIMER_2_B	MHV_PIN_D3

#define MHV_TEMPLATE_PIN_TIMER_0_A	MHV_TEMPLATE_PIN_D6
#define MHV_TEMPLATE_PIN_TIMER_0_B	MHV_TEMPLATE_PIN_D5
#define MHV_TEMPLATE_PIN_TIMER_1_A	MHV_TEMPLATE_PIN_B1
#define MHV_TEMPLATE_PIN_TIMER_1_B	MHV_TEMPLATE_PIN_B2
#define MHV_TEMPLATE_PIN_TIMER_2_A	MHV_TEMPLATE_PIN_B3
#define MHV_TEMPLATE_PIN_TIMER_2_B	MHV_TEMPLATE_PIN_D3


#define MHV_INTERRUPT_INT0 INT0_vect, &MCUCR, ISC00
#define MHV_INTERRUPT_INT1 INT1_vect, &MCUCR, ISC10

#define MHV_PC_INT_COUNT	24

#define MHV_EEPROM_VECT		EE_READY_vect

#endif /* MHV_IO_ATMEGA168_H_ */
