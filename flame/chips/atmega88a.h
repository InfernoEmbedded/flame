/*
 * Copyright (c) 2014, Inferno Embedded
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


#ifndef FLAME_IO_ATMEGA88A_H_
#define FLAME_IO_ATMEGA88A_H_

#include <avr/io.h>

#ifdef INT0_vect
#define FLAME_INTERRUPT_INT0 INT0_vect, &EICRA, ISC00, &EIMSK, INT0
#endif

#ifdef INT1_vect
#define FLAME_INTERRUPT_INT1 INT1_vect, &EICRA, ISC10, &EIMSK, INT1
#endif

//						bits,type,                            ctrlRegA,ctrlRegB,ctrlRegC,overflow1,overflow2,overflow3,inputCapture1,counter,interrupt,intEnable
#define FLAME_TIMER8_0	8,   TimerType::HAS_5_PRESCALERS,     0x44,    0x45,    0,       0x47,     0x48,     0,        0,            0x46,   0x35,    0x6e,     1
#define FLAME_TIMER16_1	16,  TimerType::HAS_5_PRESCALERS,     0x80,    0x81,    0x82,    0x88,     0x8a,     0,        0x86,         0x84,   0x36,    0x6f,     1
#define FLAME_TIMER8_2	8,   TimerType::HAS_7_PRESCALERS,     0xb0,    0xb1,    0,       0xb3,     0xb4,     0,        0,            0xb2,   0x37,    0x70,     1


#define FLAME_TIMER0_INTERRUPTS TIMER0_COMPA_vect, TIMER0_COMPB_vect, 0
#define FLAME_TIMER1_INTERRUPTS TIMER1_COMPA_vect, TIMER1_COMPB_vect, 0
#define FLAME_TIMER2_INTERRUPTS TIMER2_COMPA_vect, TIMER2_COMPB_vect, 0


// USART			Baud   Status Control I/O
//      			ubrr,  ucsra, ucsrb, ucsrc,  udr,   rxen,  txen,  rxcie,  txcie,  udre,  u2x
#define FLAME_USART0	0xc4,  0xc0,  0xc1,   0xc2,   0xc6,  RXEN0, TXEN0, RXCIE0, TXCIE0, UDRE0, U2X0


#define FLAME_USART0_INTERRUPTS	USART_RX_vect, USART_TX_vect


enum class ADCReference {
	AREF	= (0x00 << 6),
	AVCC	= (0x01 << 6),
	REF1V1	= (0x03 << 6)
};

enum class ADCChannel {
	UNDEFINED   = 0xff,
	CHANNEL_0   = 0x00,
	CHANNEL_1   = 0x01,
	CHANNEL_2   = 0x02,
	CHANNEL_3   = 0x03,
	CHANNEL_4   = 0x04,
	CHANNEL_5   = 0x05,
	CHANNEL_6   = 0x06,
	CHANNEL_7   = 0x07,
	CHANNEL_TEMPERATURE   = 0x08,
	CHANNEL_1V1   = 0xfe,
	CHANNEL_0V   = 0xff
};

//                   Dir,   Output, Input,  Bit,PCINT
#define FLAME_PIN_B0	0x24,   0x25,   0x23,   0,  0
#define FLAME_PIN_B1	0x24,   0x25,   0x23,   1,  1
#define FLAME_PIN_B2	0x24,   0x25,   0x23,   2,  2
#define FLAME_PIN_B3	0x24,   0x25,   0x23,   3,  3
#define FLAME_PIN_B4	0x24,   0x25,   0x23,   4,  4
#define FLAME_PIN_B5	0x24,   0x25,   0x23,   5,  5
#define FLAME_PIN_B6	0x24,   0x25,   0x23,   6,  6
#define FLAME_PIN_B7	0x24,   0x25,   0x23,   7,  7
#define FLAME_PIN_C0	0x27,   0x28,   0x26,   0,  8
#define FLAME_PIN_C1	0x27,   0x28,   0x26,   1,  9
#define FLAME_PIN_C2	0x27,   0x28,   0x26,   2,  10
#define FLAME_PIN_C3	0x27,   0x28,   0x26,   3,  11
#define FLAME_PIN_C4	0x27,   0x28,   0x26,   4,  12
#define FLAME_PIN_C5	0x27,   0x28,   0x26,   5,  13
#define FLAME_PIN_C6	0x27,   0x28,   0x26,   6,  14
#define FLAME_PIN_D0	0x2a,   0x2b,   0x29,   0,  16
#define FLAME_PIN_D1	0x2a,   0x2b,   0x29,   1,  17
#define FLAME_PIN_D2	0x2a,   0x2b,   0x29,   2,  18
#define FLAME_PIN_D3	0x2a,   0x2b,   0x29,   3,  19
#define FLAME_PIN_D4	0x2a,   0x2b,   0x29,   4,  20
#define FLAME_PIN_D5	0x2a,   0x2b,   0x29,   5,  21
#define FLAME_PIN_D6	0x2a,   0x2b,   0x29,   6,  22
#define FLAME_PIN_D7	0x2a,   0x2b,   0x29,   7,  23

#define FLAME_PC_INT_COUNT	24


#define FLAME_PIN_TIMER_0_A	FLAME_PIN_D6
#define FLAME_PIN_TIMER_0_B	FLAME_PIN_D5
#define FLAME_PIN_TIMER_1_A	FLAME_PIN_B1
#define FLAME_PIN_TIMER_1_B	FLAME_PIN_B2
#define FLAME_PIN_TIMER_2_A	FLAME_PIN_B3
#define FLAME_PIN_TIMER_2_B	FLAME_PIN_D3


#endif // FLAME_IO_ATMEGA88A_H_

