/*
 * Copyright (c) 2010, Make, Hack, Void Inc
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


#ifndef MHV_IO_ATMEGA1280_H_
#define MHV_IO_ATMEGA1280_H_

#include <avr/io.h>

// 8 bit timers
//					 type,                        ctrlRegA,ctrlRegB,overflow1,overflow2,counter,interrupt
#define MHV_TIMER8_0 MHV_TIMER_TYPE_5_PRESCALERS, &TCCR0A, &TCCR0B, &OCR0A,   &OCR0B,   &TCNT0, &TIMSK0
#define MHV_TIMER8_2 MHV_TIMER_TYPE_7_PRESCALERS, &TCCR2A, &TCCR2B, &OCR2A,   &OCR2B,   &TCNT2, &TIMSK2

#define MHV_TIMER0_INTERRUPTS SIG_OUTPUT_COMPARE0A, SIG_OUTPUT_COMPARE0B, 0
#define MHV_TIMER2_INTERRUPTS SIG_OUTPUT_COMPARE2A, SIG_OUTPUT_COMPARE2B, 0

// 16 bit timers
//					  ctrlRegA,ctrlRegB,ctrlRegC,overflow1,overflow2,overflow3,counter,interrupt,inputCapture1
#define MHV_TIMER16_1 &TCCR1A, &TCCR1B, &TCCR1C, &OCR1A,   &OCR1B,   &OCR1C,   &TCNT1, &TIMSK1,  &ICR1
#define MHV_TIMER16_3 &TCCR3A, &TCCR3B, &TCCR3C, &OCR3A,   &OCR3B,   &OCR3C,   &TCNT3, &TIMSK3,  &ICR3
#define MHV_TIMER16_4 &TCCR4A, &TCCR4B, &TCCR4C, &OCR4A,   &OCR4B,   &OCR4C,   &TCNT4, &TIMSK4,  &ICR4
#define MHV_TIMER16_5 &TCCR5A, &TCCR5B, &TCCR5C, &OCR5A,   &OCR5B,   &OCR5C,   &TCNT5, &TIMSK5,  &ICR5

#define MHV_TIMER1_INTERRUPTS SIG_OUTPUT_COMPARE1A, SIG_OUTPUT_COMPARE1B, SIG_OUTPUT_COMPARE1C
#define MHV_TIMER3_INTERRUPTS SIG_OUTPUT_COMPARE3A, SIG_OUTPUT_COMPARE3B, SIG_OUTPUT_COMPARE3C
#define MHV_TIMER4_INTERRUPTS SIG_OUTPUT_COMPARE4A, SIG_OUTPUT_COMPARE4B, SIG_OUTPUT_COMPARE4C
#define MHV_TIMER5_INTERRUPTS SIG_OUTPUT_COMPARE5A, SIG_OUTPUT_COMPARE5B, SIG_OUTPUT_COMPARE5C

#define MHV_USART0 &UBRR0, &UCSR0A, &UCSR0B, &UDR0, RXEN0, TXEN0, RXCIE0, TXCIE0, UDRE0, U2X0
#define MHV_USART1 &UBRR1, &UCSR1A, &UCSR1B, &UDR1, RXEN1, TXEN1, RXCIE1, TXCIE1, UDRE1, U2X1
#define MHV_USART2 &UBRR2, &UCSR2A, &UCSR2B, &UDR2, RXEN2, TXEN2, RXCIE2, TXCIE2, UDRE2, U2X2
#define MHV_USART3 &UBRR3, &UCSR3A, &UCSR3B, &UDR3, RXEN3, TXEN3, RXCIE3, TXCIE3, UDRE3, U2X3

#define MHV_USART0_INTERRUPTS USART0_RX_vect, USART0_TX_vect
#define MHV_USART1_INTERRUPTS USART1_RX_vect, USART1_TX_vect
#define MHV_USART2_INTERRUPTS USART2_RX_vect, USART2_TX_vect
#define MHV_USART3_INTERRUPTS USART3_RX_vect, USART3_TX_vect

#define MHV_AD_REFERENCE_AREF	0x00 << 6
#define MHV_AD_REFERENCE_AVCC	0x01 << 6
#define MHV_AD_REFERENCE_1V1	0x02 << 6
#define MHV_AD_REFERENCE_2V56	0x03 << 6

#define MHV_AD_CHANNEL_0			0x00
#define MHV_AD_CHANNEL_1			0x01
#define MHV_AD_CHANNEL_2			0x02
#define MHV_AD_CHANNEL_3			0x03
#define MHV_AD_CHANNEL_4			0x04
#define MHV_AD_CHANNEL_5			0x05
#define MHV_AD_CHANNEL_6			0x06
#define MHV_AD_CHANNEL_7			0x07
#define MHV_AD_CHANNEL_0_X10_0		0x08
#define MHV_AD_CHANNEL_1_X10_0		0x09
#define MHV_AD_CHANNEL_0_X200_0		0x0a
#define MHV_AD_CHANNEL_1_X200_0		0x0b
#define MHV_AD_CHANNEL_2_X10_2		0x0c
#define MHV_AD_CHANNEL_3_X10_2		0x0d
#define MHV_AD_CHANNEL_2_X200_2		0x0e
#define MHV_AD_CHANNEL_3_X200_2		0x0f
#define MHV_AD_CHANNEL_0_X1_1		0x10
#define MHV_AD_CHANNEL_1_X1_1		0x11
#define MHV_AD_CHANNEL_2_X1_1		0x12
#define MHV_AD_CHANNEL_3_X1_1		0x13
#define MHV_AD_CHANNEL_4_X1_1		0x14
#define MHV_AD_CHANNEL_5_X1_1		0x15
#define MHV_AD_CHANNEL_6_X1_1		0x16
#define MHV_AD_CHANNEL_7_X1_1		0x17
#define MHV_AD_CHANNEL_0_X1_2		0x18
#define MHV_AD_CHANNEL_1_X1_2		0x19
#define MHV_AD_CHANNEL_2_X1_2		0x1a
#define MHV_AD_CHANNEL_3_X1_2		0x1b
#define MHV_AD_CHANNEL_4_X1_2		0x1c
#define MHV_AD_CHANNEL_5_X1_2		0x1d
#define MHV_AD_CHANNEL_1V1			0x1e
#define MHV_AD_CHANNEL_0V			0x1f
#define MHV_AD_CHANNEL_8			0x20
#define MHV_AD_CHANNEL_9			0x21
#define MHV_AD_CHANNEL_10			0x22
#define MHV_AD_CHANNEL_11			0x23
#define MHV_AD_CHANNEL_12			0x24
#define MHV_AD_CHANNEL_13			0x25
#define MHV_AD_CHANNEL_14			0x26
#define MHV_AD_CHANNEL_15			0x27
#define MHV_AD_CHANNEL_8_X10_8		0x28
#define MHV_AD_CHANNEL_9_X10_8		0x29
#define MHV_AD_CHANNEL_8_X200_8		0x2a
#define MHV_AD_CHANNEL_9_X200_8		0x2b
#define MHV_AD_CHANNEL_10_X10_10	0x2c
#define MHV_AD_CHANNEL_11_X10_10	0x2d
#define MHV_AD_CHANNEL_10_X200_10	0x2e
#define MHV_AD_CHANNEL_11_X200_10	0x2f
#define MHV_AD_CHANNEL_8_X1_9		0x30
#define MHV_AD_CHANNEL_9_X1_9		0x31
#define MHV_AD_CHANNEL_10_X1_9		0x32
#define MHV_AD_CHANNEL_11_X1_9		0x33
#define MHV_AD_CHANNEL_12_X1_9		0x34
#define MHV_AD_CHANNEL_13_X1_9		0x35
#define MHV_AD_CHANNEL_14_X1_9		0x36
#define MHV_AD_CHANNEL_15_X1_9		0x37
#define MHV_AD_CHANNEL_8_X1_10		0x38
#define MHV_AD_CHANNEL_9_X1_10		0x39
#define MHV_AD_CHANNEL_10_X1_10		0x3a
#define MHV_AD_CHANNEL_11_X1_10		0x3b
#define MHV_AD_CHANNEL_12_X1_10		0x3c
#define MHV_AD_CHANNEL_13_X1_10		0x3d

// Power reduction register for ADC
#define MHV_AD_PRR	PRR0


//					Dir,	Output,	Input,	Bit
#define MHV_PIN_A0	&DDRA,	&PORTA, &PINA,	0
#define MHV_PIN_A1	&DDRA,	&PORTA, &PINA,	1
#define MHV_PIN_A2	&DDRA,	&PORTA, &PINA,	2
#define MHV_PIN_A3	&DDRA,	&PORTA, &PINA,	3
#define MHV_PIN_A4	&DDRA,	&PORTA, &PINA,	4
#define MHV_PIN_A5	&DDRA,	&PORTA, &PINA,	5
#define MHV_PIN_A6	&DDRA,	&PORTA, &PINA,	6
#define MHV_PIN_A7	&DDRA,	&PORTA, &PINA,	7
#define MHV_PIN_B0	&DDRB,	&PORTB, &PINB,	0
#define MHV_PIN_B1	&DDRB,	&PORTB, &PINB,	1
#define MHV_PIN_B2	&DDRB,	&PORTB, &PINB,	2
#define MHV_PIN_B3	&DDRB,	&PORTB, &PINB,	3
#define MHV_PIN_B4	&DDRB,	&PORTB, &PINB,	4
#define MHV_PIN_B5	&DDRB,	&PORTB, &PINB,	5
#define MHV_PIN_B6	&DDRB,	&PORTB, &PINB,	6
#define MHV_PIN_B7	&DDRB,	&PORTB, &PINB,	7
#define MHV_PIN_C0	&DDRC,	&PORTC, &PINC,	0
#define MHV_PIN_C1	&DDRC,	&PORTC, &PINC,	1
#define MHV_PIN_C2	&DDRC,	&PORTC, &PINC,	2
#define MHV_PIN_C3	&DDRC,	&PORTC, &PINC,	3
#define MHV_PIN_C4	&DDRC,	&PORTC, &PINC,	4
#define MHV_PIN_C5	&DDRC,	&PORTC, &PINC,	5
#define MHV_PIN_C6	&DDRC,	&PORTC, &PINC,	6
#define MHV_PIN_C7	&DDRC,	&PORTC, &PINC,	7
#define MHV_PIN_D0	&DDRD,	&PORTD, &PIND,	0
#define MHV_PIN_D1	&DDRD,	&PORTD, &PIND,	1
#define MHV_PIN_D2	&DDRD,	&PORTD, &PIND,	2
#define MHV_PIN_D3	&DDRD,	&PORTD, &PIND,	3
#define MHV_PIN_D4	&DDRD,	&PORTD, &PIND,	4
#define MHV_PIN_D5	&DDRD,	&PORTD, &PIND,	5
#define MHV_PIN_D6	&DDRD,	&PORTD, &PIND,	6
#define MHV_PIN_D7	&DDRD,	&PORTD, &PIND,	7
#define MHV_PIN_E0	&DDRE,	&PORTE, &PINE,	0
#define MHV_PIN_E1	&DDRE,	&PORTE, &PINE,	1
#define MHV_PIN_E2	&DDRE,	&PORTE, &PINE,	2
#define MHV_PIN_E3	&DDRE,	&PORTE, &PINE,	3
#define MHV_PIN_E4	&DDRE,	&PORTE, &PINE,	4
#define MHV_PIN_E5	&DDRE,	&PORTE, &PINE,	5
#define MHV_PIN_E6	&DDRE,	&PORTE, &PINE,	6
#define MHV_PIN_E7	&DDRE,	&PORTE, &PINE,	7
#define MHV_PIN_F0	&DDRF,	&PORTF, &PINF,	0
#define MHV_PIN_F1	&DDRF,	&PORTF, &PINF,	1
#define MHV_PIN_F2	&DDRF,	&PORTF, &PINF,	2
#define MHV_PIN_F3	&DDRF,	&PORTF, &PINF,	3
#define MHV_PIN_F4	&DDRF,	&PORTF, &PINF,	4
#define MHV_PIN_F5	&DDRF,	&PORTF, &PINF,	5
#define MHV_PIN_F6	&DDRF,	&PORTF, &PINF,	6
#define MHV_PIN_F7	&DDRF,	&PORTF, &PINF,	7
#define MHV_PIN_G0	&DDRG,	&PORTG, &PING,	0
#define MHV_PIN_G1	&DDRG,	&PORTG, &PING,	1
#define MHV_PIN_G2	&DDRG,	&PORTG, &PING,	2
#define MHV_PIN_G3	&DDRG,	&PORTG, &PING,	3
#define MHV_PIN_G4	&DDRG,	&PORTG, &PING,	4
#define MHV_PIN_G5	&DDRG,	&PORTG, &PING,	5
#define MHV_PIN_H0	&DDRH,	&PORTH, &PINH,	0
#define MHV_PIN_H1	&DDRH,	&PORTH, &PINH,	1
#define MHV_PIN_H2	&DDRH,	&PORTH, &PINH,	2
#define MHV_PIN_H3	&DDRH,	&PORTH, &PINH,	3
#define MHV_PIN_H4	&DDRH,	&PORTH, &PINH,	4
#define MHV_PIN_H5	&DDRH,	&PORTH, &PINH,	5
#define MHV_PIN_H6	&DDRH,	&PORTH, &PINH,	6
#define MHV_PIN_H7	&DDRH,	&PORTH, &PINH,	7
#define MHV_PIN_J0	&DDRJ,	&PORTJ, &PINJ,	0
#define MHV_PIN_J1	&DDRJ,	&PORTJ, &PINJ,	1
#define MHV_PIN_J2	&DDRJ,	&PORTJ, &PINJ,	2
#define MHV_PIN_J3	&DDRJ,	&PORTJ, &PINJ,	3
#define MHV_PIN_J4	&DDRJ,	&PORTJ, &PINJ,	4
#define MHV_PIN_J5	&DDRJ,	&PORTJ, &PINJ,	5
#define MHV_PIN_J6	&DDRJ,	&PORTJ, &PINJ,	6
#define MHV_PIN_J7	&DDRJ,	&PORTJ, &PINJ,	7
#define MHV_PIN_K0	&DDRK,	&PORTK, &PINK,	0
#define MHV_PIN_K1	&DDRK,	&PORTK, &PINK,	1
#define MHV_PIN_K2	&DDRK,	&PORTK, &PINK,	2
#define MHV_PIN_K3	&DDRK,	&PORTK, &PINK,	3
#define MHV_PIN_K4	&DDRK,	&PORTK, &PINK,	4
#define MHV_PIN_K5	&DDRK,	&PORTK, &PINK,	5
#define MHV_PIN_K6	&DDRK,	&PORTK, &PINK,	6
#define MHV_PIN_K7	&DDRK,	&PORTK, &PINK,	7
#define MHV_PIN_L0	&DDRL,	&PORTL, &PINL,	0
#define MHV_PIN_L1	&DDRL,	&PORTL, &PINL,	1
#define MHV_PIN_L2	&DDRL,	&PORTL, &PINL,	2
#define MHV_PIN_L3	&DDRL,	&PORTL, &PINL,	3
#define MHV_PIN_L4	&DDRL,	&PORTL, &PINL,	4
#define MHV_PIN_L5	&DDRL,	&PORTL, &PINL,	5
#define MHV_PIN_L6	&DDRL,	&PORTL, &PINL,	6
#define MHV_PIN_L7	&DDRL,	&PORTL, &PINL,	7
#endif /* MHV_IO_ATMEGA1280_H_ */
