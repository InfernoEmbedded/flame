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


#ifndef MHV_IO_ATMEGA1280_H_
#define MHV_IO_ATMEGA1280_H_

#include <avr/io.h>

// 8 bit timers
//					 type,                        ctrlRegA,ctrlRegB,overflow1,overflow2,counter,interrupt,intEnable
#define MHV_TIMER8_0 MHV_TIMER_TYPE_5_PRESCALERS, &TCCR0A, &TCCR0B, &OCR0A,   &OCR0B,   &TCNT0, &TIMSK0,  OCIE0A
#define MHV_TIMER8_2 MHV_TIMER_TYPE_7_PRESCALERS, &TCCR2A, &TCCR2B, &OCR2A,   &OCR2B,   &TCNT2, &TIMSK2,  OCIE2A

#define MHV_TIMER0_INTERRUPTS TIMER0_COMPA_vect, TIMER0_COMPB_vect, 0
#define MHV_TIMER2_INTERRUPTS TIMER2_COMPA_vect, TIMER2_COMPB_vect, 0

// 16 bit timers
//					  ctrlRegA,ctrlRegB,ctrlRegC,overflow1,overflow2,overflow3,counter,interrupt,inputCapture1
#define MHV_TIMER16_1 &TCCR1A, &TCCR1B, &TCCR1C, &OCR1A,   &OCR1B,   &OCR1C,   &TCNT1, &TIMSK1,  &ICR1
#define MHV_TIMER16_3 &TCCR3A, &TCCR3B, &TCCR3C, &OCR3A,   &OCR3B,   &OCR3C,   &TCNT3, &TIMSK3,  &ICR3
#define MHV_TIMER16_4 &TCCR4A, &TCCR4B, &TCCR4C, &OCR4A,   &OCR4B,   &OCR4C,   &TCNT4, &TIMSK4,  &ICR4
#define MHV_TIMER16_5 &TCCR5A, &TCCR5B, &TCCR5C, &OCR5A,   &OCR5B,   &OCR5C,   &TCNT5, &TIMSK5,  &ICR5

#define MHV_TIMER1_INTERRUPTS TIMER1_COMPA_vect, TIMER1_COMPB_vect, TIMER1_COMPC_vect
#define MHV_TIMER3_INTERRUPTS TIMER3_COMPA_vect, TIMER3_COMPB_vect, TIMER3_COMPC_vect
#define MHV_TIMER4_INTERRUPTS TIMER4_COMPA_vect, TIMER4_COMPB_vect, TIMER4_COMPC_vect
#define MHV_TIMER5_INTERRUPTS TIMER5_COMPA_vect, TIMER5_COMPB_vect, TIMER5_COMPC_vect

// USART           Baud    Status   Control  I/O
//				   ubrr,   ucsra,   ucsrb,   udr,   rxen,  txen,  rxcie,  txcie,  udre,  u2x
#define MHV_USART0 0xC4,   0xC0,    0xC1,    0xC6,  RXEN0, TXEN0, RXCIE0, TXCIE0, UDRE0, U2X0
#define MHV_USART1 0xCC,   0xC8,    0xC9,    0xCE,  RXEN1, TXEN1, RXCIE1, TXCIE1, UDRE1, U2X1
#define MHV_USART2 0xD4,   0xC0,    0xD1,    0xD6,  RXEN2, TXEN2, RXCIE2, TXCIE2, UDRE2, U2X2
#define MHV_USART3 0x134,  0x130,   0x131,   0x136, RXEN3, TXEN3, RXCIE3, TXCIE3, UDRE3, U2X3

#define MHV_USART0_INTERRUPTS USART0_RX_vect, USART0_TX_vect
#define MHV_USART1_INTERRUPTS USART1_RX_vect, USART1_TX_vect
#define MHV_USART2_INTERRUPTS USART2_RX_vect, USART2_TX_vect
#define MHV_USART3_INTERRUPTS USART3_RX_vect, USART3_TX_vect

#define MHV_UART_REGISTER		_SFR_MEM8
#define MHV_UART_BAUD_REGISTER	_SFR_MEM16

#define MHV_AD_RESOLUTION		1024


#define MHV_AD_REFERENCE_AREF	(uint8_t)(0x00 << 6)
#define MHV_AD_REFERENCE_AVCC	(uint8_t)(0x01 << 6)
#define MHV_AD_REFERENCE_1V1	(uint8_t)(0x02 << 6)
#define MHV_AD_REFERENCE_2V56	(uint8_t)(0x03 << 6)

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


//					Dir,	Output,	Input,	Bit,PCINT
#define MHV_PIN_A0	0x01,	0x02, 0x00,		0,	-1
#define MHV_PIN_A1	0x01,	0x02, 0x00,		1,	-1
#define MHV_PIN_A2	0x01,	0x02, 0x00,		2,	-1
#define MHV_PIN_A3	0x01,	0x02, 0x00,		3,	-1
#define MHV_PIN_A4	0x01,	0x02, 0x00,		4,	-1
#define MHV_PIN_A5	0x01,	0x02, 0x00,		5,	-1
#define MHV_PIN_A6	0x01,	0x02, 0x00,		6,	-1
#define MHV_PIN_A7	0x01,	0x02, 0x00,		7,	-1
#define MHV_PIN_B0	0x04,	0x05, 0x03,		0,	0
#define MHV_PIN_B1	0x04,	0x05, 0x03,		1,	1
#define MHV_PIN_B2	0x04,	0x05, 0x03,		2,	2
#define MHV_PIN_B3	0x04,	0x05, 0x03,		3,	3
#define MHV_PIN_B4	0x04,	0x05, 0x03,		4,	4
#define MHV_PIN_B5	0x04,	0x05, 0x03,		5,	5
#define MHV_PIN_B6	0x04,	0x05, 0x03,		6,	6
#define MHV_PIN_B7	0x04,	0x05, 0x03,		7,	7
#define MHV_PIN_C0	0x07,	0x08, 0x06,		0,	-1
#define MHV_PIN_C1	0x07,	0x08, 0x06,		1,	-1
#define MHV_PIN_C2	0x07,	0x08, 0x06,		2,	-1
#define MHV_PIN_C3	0x07,	0x08, 0x06,		3,	-1
#define MHV_PIN_C4	0x07,	0x08, 0x06,		4,	-1
#define MHV_PIN_C5	0x07,	0x08, 0x06,		5,	-1
#define MHV_PIN_C6	0x07,	0x08, 0x06,		6,	-1
#define MHV_PIN_C7	0x07,	0x08, 0x06,		7,	-1
#define MHV_PIN_D0	0x0A,	0x0B, 0x09,		0,	-1
#define MHV_PIN_D1	0x0A,	0x0B, 0x09,		1,	-1
#define MHV_PIN_D2	0x0A,	0x0B, 0x09,		2,	-1
#define MHV_PIN_D3	0x0A,	0x0B, 0x09,		3,	-1
#define MHV_PIN_D4	0x0A,	0x0B, 0x09,		4,	-1
#define MHV_PIN_D5	0x0A,	0x0B, 0x09,		5,	-1
#define MHV_PIN_D6	0x0A,	0x0B, 0x09,		6,	-1
#define MHV_PIN_D7	0x0A,	0x0B, 0x09,		7,	-1
#define MHV_PIN_E0	0x0D,	0x0E, 0x0C,		0,	8
#define MHV_PIN_E1	0x0D,	0x0E, 0x0C,		1,	-1
#define MHV_PIN_E2	0x0D,	0x0E, 0x0C,		2,	-1
#define MHV_PIN_E3	0x0D,	0x0E, 0x0C,		3,	-1
#define MHV_PIN_E4	0x0D,	0x0E, 0x0C,		4,	-1
#define MHV_PIN_E5	0x0D,	0x0E, 0x0C,		5,	-1
#define MHV_PIN_E6	0x0D,	0x0E, 0x0C,		6,	-1
#define MHV_PIN_E7	0x0D,	0x0E, 0x0C,		7,	-1
#define MHV_PIN_F0	0x10,	0x11, 0x0F,		0,	-1
#define MHV_PIN_F1	0x10,	0x11, 0x0F,		1,	-1
#define MHV_PIN_F2	0x10,	0x11, 0x0F,		2,	-1
#define MHV_PIN_F3	0x10,	0x11, 0x0F,		3,	-1
#define MHV_PIN_F4	0x10,	0x11, 0x0F,		4,	-1
#define MHV_PIN_F5	0x10,	0x11, 0x0F,		5,	-1
#define MHV_PIN_F6	0x10,	0x11, 0x0F,		6,	-1
#define MHV_PIN_F7	0x10,	0x11, 0x0F,		7,	-1
#define MHV_PIN_G0	0x13,	0x14, 0x12,		0,	-1
#define MHV_PIN_G1	0x13,	0x14, 0x12,		1,	-1
#define MHV_PIN_G2	0x13,	0x14, 0x12,		2,	-1
#define MHV_PIN_G3	0x13,	0x14, 0x12,		3,	-1
#define MHV_PIN_G4	0x13,	0x14, 0x12,		4,	-1
#define MHV_PIN_G5	0x13,	0x14, 0x12,		5,	-1
#define MHV_PIN_H0	0x101,	0x102, 0x100,	0,	-1
#define MHV_PIN_H1	0x101,	0x102, 0x100,	1,	-1
#define MHV_PIN_H2	0x101,	0x102, 0x100,	2,	-1
#define MHV_PIN_H3	0x101,	0x102, 0x100,	3,	-1
#define MHV_PIN_H4	0x101,	0x102, 0x100,	4,	-1
#define MHV_PIN_H5	0x101,	0x102, 0x100,	5,	-1
#define MHV_PIN_H6	0x101,	0x102, 0x100,	6,	-1
#define MHV_PIN_H7	0x101,	0x102, 0x100,	7,	-1
#define MHV_PIN_J0	0x104,	0x105, 0x103,	0,	9
#define MHV_PIN_J1	0x104,	0x105, 0x103,	1,	10
#define MHV_PIN_J2	0x104,	0x105, 0x103,	2,	11
#define MHV_PIN_J3	0x104,	0x105, 0x103,	3,	12
#define MHV_PIN_J4	0x104,	0x105, 0x103,	4,	13
#define MHV_PIN_J5	0x104,	0x105, 0x103,	5,	14
#define MHV_PIN_J6	0x104,	0x105, 0x103,	6,	15
#define MHV_PIN_J7	0x104,	0x105, 0x103,	7,	-1
#define MHV_PIN_K0	0x107,	0x108, 0x106,	0,	16
#define MHV_PIN_K1	0x107,	0x108, 0x106,	1,	17
#define MHV_PIN_K2	0x107,	0x108, 0x106,	2,	18
#define MHV_PIN_K3	0x107,	0x108, 0x106,	3,	19
#define MHV_PIN_K4	0x107,	0x108, 0x106,	4,	20
#define MHV_PIN_K5	0x107,	0x108, 0x106,	5,	21
#define MHV_PIN_K6	0x107,	0x108, 0x106,	6,	22
#define MHV_PIN_K7	0x107,	0x108, 0x106,	7,	23
#define MHV_PIN_L0	0x10A,	0x10B, 0x109,	0,	-1
#define MHV_PIN_L1	0x10A,	0x10B, 0x109,	1,	-1
#define MHV_PIN_L2	0x10A,	0x10B, 0x109,	2,	-1
#define MHV_PIN_L3	0x10A,	0x10B, 0x109,	3,	-1
#define MHV_PIN_L4	0x10A,	0x10B, 0x109,	4,	-1
#define MHV_PIN_L5	0x10A,	0x10B, 0x109,	5,	-1
#define MHV_PIN_L6	0x10A,	0x10B, 0x109,	6,	-1
#define MHV_PIN_L7	0x10A,	0x10B, 0x109,	7,	-1


#define MHV_PIN_TIMER_0_A	MHV_PIN_B7
#define MHV_PIN_TIMER_0_B	MHV_PIN_G5
#define MHV_PIN_TIMER_1_A	MHV_PIN_B5
#define MHV_PIN_TIMER_1_B	MHV_PIN_B6
#define MHV_PIN_TIMER_1_C	MHV_PIN_B7
#define MHV_PIN_TIMER_2_A	MHV_PIN_B4
#define MHV_PIN_TIMER_2_B	MHV_PIN_H6
#define MHV_PIN_TIMER_3_A	MHV_PIN_E3
#define MHV_PIN_TIMER_3_B	MHV_PIN_E4
#define MHV_PIN_TIMER_3_C	MHV_PIN_E5
#define MHV_PIN_TIMER_4_A	MHV_PIN_H3
#define MHV_PIN_TIMER_4_B	MHV_PIN_H4
#define MHV_PIN_TIMER_4_C	MHV_PIN_H5
#define MHV_PIN_TIMER_5_A	MHV_PIN_L3
#define MHV_PIN_TIMER_5_B	MHV_PIN_L4
#define MHV_PIN_TIMER_5_C	MHV_PIN_L6


#define MHV_PC_INT_COUNT	24

#define MHV_EEPROM_VECT		EE_READY_vect

#endif /* MHV_IO_ATMEGA1280_H_ */
