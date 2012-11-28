/*
 * Copyright (c) 2012, Make, Hack, Void Inc
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


#ifndef MHV_IO_ATMEGA168A_H_
#define MHV_IO_ATMEGA168A_H_

#include <avr/io.h>

#ifdef INT0_vect
#define MHV_INTERRUPT_INT0 INT0_vect, &MCUCR, ISC00
#endif

#ifdef INT1_vect
#define MHV_INTERRUPT_INT1 INT1_vect, &MCUCR, ISC10
#endif

//						bits,type,                            ctrlRegA,ctrlRegB,ctrlRegC,overflow1,overflow2,overflow3,inputCapture1,counter,interrupt,intEnable
#define MHV_TIMER8_0	8,   TIMER_TYPE::HAS_5_PRESCALERS,    0x44,    0x45,    0,       0x47,     0x48,     0,        0,            0x46,   0x6e,     1
#define MHV_TIMER16_1	16,  TIMER_TYPE::HAS_5_PRESCALERS,    0x80,    0x81,    0x82,    0x88,     0x8a,     0,        0x86,         0x84,   0x6f,     1
#define MHV_TIMER8_2	8,   TIMER_TYPE::HAS_7_PRESCALERS,    0xb0,    0xb1,    0,       0xb3,     0xb4,     0,        0,            0xb2,   0x70,     1


#define MHV_TIMER0_INTERRUPTS TIMER0_COMPA_vect, TIMER0_COMPB_vect, 0
#define MHV_TIMER1_INTERRUPTS TIMER1_COMPA_vect, TIMER1_COMPB_vect, 0
#define MHV_TIMER2_INTERRUPTS TIMER2_COMPA_vect, TIMER2_COMPB_vect, 0


// USART			Baud   Status Control I/O
//      			ubrr,  ucsra, ucsrb, ucsrc,  udr,   rxen,  txen,  rxcie,  txcie,  udre,  u2x
#define MHV_USART0	0xc4,  0xc0,  0xc1,   0xc2,   0xc6,  RXEN0, TXEN0, RXCIE0, TXCIE0, UDRE0, U2X0


#define MHV_USART0_INTERRUPTS	USART_RX_vect, USART_TX_vect


enum mhv_ad_reference {
	MHV_AD_REFERENCE_AREF	= (0x00 << 6),
	MHV_AD_REFERENCE_AVCC	= (0x01 << 6),
	MHV_AD_REFERENCE_1V1	= (0x03 << 6)
};
typedef enum mhv_ad_reference MHV_AD_REFERENCE;

enum mhv_ad_channel {
	MHV_AD_CHANNEL_0   = 0x00,
	MHV_AD_CHANNEL_1   = 0x01,
	MHV_AD_CHANNEL_2   = 0x02,
	MHV_AD_CHANNEL_3   = 0x03,
	MHV_AD_CHANNEL_4   = 0x04,
	MHV_AD_CHANNEL_5   = 0x05,
	MHV_AD_CHANNEL_6   = 0x06,
	MHV_AD_CHANNEL_7   = 0x07,
	MHV_AD_CHANNEL_TEMPERATURE   = 0x08,
	MHV_AD_CHANNEL_1V1   = 0xfe,
	MHV_AD_CHANNEL_0V   = 0xff
};
typedef enum mhv_ad_reference MHV_AD_REFERENCE;


//                   Dir,   Output, Input,  Bit,PCINT
#define MHV_PIN_B0	0x24,   0x25,   0x23,   0,  0
#define MHV_PIN_B1	0x24,   0x25,   0x23,   1,  1
#define MHV_PIN_B2	0x24,   0x25,   0x23,   2,  2
#define MHV_PIN_B3	0x24,   0x25,   0x23,   3,  3
#define MHV_PIN_B4	0x24,   0x25,   0x23,   4,  4
#define MHV_PIN_B5	0x24,   0x25,   0x23,   5,  5
#define MHV_PIN_B6	0x24,   0x25,   0x23,   6,  6
#define MHV_PIN_B7	0x24,   0x25,   0x23,   7,  7
#define MHV_PIN_C0	0x27,   0x28,   0x26,   0,  8
#define MHV_PIN_C1	0x27,   0x28,   0x26,   1,  9
#define MHV_PIN_C2	0x27,   0x28,   0x26,   2,  10
#define MHV_PIN_C3	0x27,   0x28,   0x26,   3,  11
#define MHV_PIN_C4	0x27,   0x28,   0x26,   4,  12
#define MHV_PIN_C5	0x27,   0x28,   0x26,   5,  13
#define MHV_PIN_C6	0x27,   0x28,   0x26,   6,  14
#define MHV_PIN_D0	0x2a,   0x2b,   0x29,   0,  16
#define MHV_PIN_D1	0x2a,   0x2b,   0x29,   1,  17
#define MHV_PIN_D2	0x2a,   0x2b,   0x29,   2,  18
#define MHV_PIN_D3	0x2a,   0x2b,   0x29,   3,  19
#define MHV_PIN_D4	0x2a,   0x2b,   0x29,   4,  20
#define MHV_PIN_D5	0x2a,   0x2b,   0x29,   5,  21
#define MHV_PIN_D6	0x2a,   0x2b,   0x29,   6,  22
#define MHV_PIN_D7	0x2a,   0x2b,   0x29,   7,  23

#define MHV_PC_INT_COUNT	24


#define MHV_PIN_TIMER_0_A	MHV_PIN_D6
#define MHV_PIN_TIMER_0_B	MHV_PIN_D5
#define MHV_PIN_TIMER_1_A	MHV_PIN_B1
#define MHV_PIN_TIMER_1_B	MHV_PIN_B2
#define MHV_PIN_TIMER_2_A	MHV_PIN_B3
#define MHV_PIN_TIMER_2_B	MHV_PIN_D3


#endif // MHV_IO_ATMEGA168A_H_

