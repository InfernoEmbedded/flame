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


#ifndef MHV_IO_ATMEGA2561_H_
#define MHV_IO_ATMEGA2561_H_

#include <avr/io.h>

#ifdef INT0_vect
#define MHV_INTERRUPT_INT0 INT0_vect, &MCUCR, ISC00
#endif

#ifdef INT1_vect
#define MHV_INTERRUPT_INT1 INT1_vect, &MCUCR, ISC10
#endif

//						bits,type,                            ctrlRegA,ctrlRegB,ctrlRegC,overflow1,overflow2,overflow3,inputCapture1,counter,interrupt,intEnable
#define MHV_TIMER8_0	8,   TimerType::HAS_5_PRESCALERS,     0x44,    0x45,    0,       0x47,     0x48,     0,        0,            0x46,   0x6e,     1
#define MHV_TIMER16_1	16,  TimerType::HAS_5_PRESCALERS,     0x80,    0x81,    0x82,    0x88,     0x8a,     0x8c,     0x86,         0x84,   0x6f,     1
#define MHV_TIMER8_2	8,   TimerType::HAS_7_PRESCALERS,     0xb0,    0xb1,    0,       0xb3,     0xb4,     0,        0,            0xb2,   0x70,     1
#define MHV_TIMER16_3	16,  TimerType::HAS_5_PRESCALERS,     0x90,    0x91,    0x92,    0x98,     0x9a,     0x9c,     0x96,         0x94,   0x71,     1
#define MHV_TIMER16_4	16,  TimerType::HAS_5_PRESCALERS,     0xa0,    0xa1,    0xa2,    0xa8,     0xaa,     0xac,     0xa6,         0xa4,   0x72,     1
#define MHV_TIMER16_5	16,  TimerType::HAS_5_PRESCALERS,     0x120,   0x121,   0x122,   0x128,    0x12a,    0x12c,    0x126,        0x124,  0x73,     1


#define MHV_TIMER0_INTERRUPTS TIMER0_COMPA_vect, TIMER0_COMPB_vect, 0
#define MHV_TIMER1_INTERRUPTS TIMER1_COMPA_vect, TIMER1_COMPB_vect, TIMER1_COMPC_vect
#define MHV_TIMER2_INTERRUPTS TIMER2_COMPA_vect, TIMER2_COMPB_vect, 0
#define MHV_TIMER3_INTERRUPTS TIMER3_COMPA_vect, TIMER3_COMPB_vect, TIMER3_COMPC_vect
#define MHV_TIMER4_INTERRUPTS TIMER4_COMPA_vect, TIMER4_COMPB_vect, TIMER4_COMPC_vect
#define MHV_TIMER5_INTERRUPTS TIMER5_COMPA_vect, TIMER5_COMPB_vect, TIMER5_COMPC_vect


// USART			Baud   Status Control I/O
//      			ubrr,  ucsra, ucsrb, ucsrc,  udr,   rxen,  txen,  rxcie,  txcie,  udre,  u2x
#define MHV_USART0	0xc4,  0xc0,  0xc1,   0xc2,   0xc6,  RXEN0, TXEN0, RXCIE0, TXCIE0, UDRE0, U2X0
#define MHV_USART1	0xcc,  0xc8,  0xc9,   0xca,   0xce,  RXEN1, TXEN1, RXCIE1, TXCIE1, UDRE1, U2X1


#define MHV_USART0_INTERRUPTS	USART0_RX_vect, USART0_TX_vect
#define MHV_USART1_INTERRUPTS	USART1_RX_vect, USART1_TX_vect


enum class ADCReference {
	AREF	= (0x00 << 6),
	AVCC	= (0x01 << 6),
	REF1V1	= (0x02 << 6),
	REF2V56	= (0x03 << 6)
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
	CHANNEL_0_X10_0   = 0x08,
	CHANNEL_1_X10_0   = 0x09,
	CHANNEL_0_X200_0   = 0x0a,
	CHANNEL_1_X200_0   = 0x0b,
	CHANNEL_2_X10_2   = 0x0c,
	CHANNEL_3_X10_2   = 0x0d,
	CHANNEL_2_X200_2   = 0x0e,
	CHANNEL_3_X200_2   = 0x0f,
	CHANNEL_0_X1_1   = 0x10,
	CHANNEL_1_X1_1   = 0x11,
	CHANNEL_2_X1_1   = 0x12,
	CHANNEL_3_X1_1   = 0x13,
	CHANNEL_4_X1_1   = 0x14,
	CHANNEL_5_X1_1   = 0x15,
	CHANNEL_6_X1_1   = 0x16,
	CHANNEL_7_X1_1   = 0x17,
	CHANNEL_0_X1_2   = 0x18,
	CHANNEL_1_X1_2   = 0x19,
	CHANNEL_2_X1_2   = 0x1a,
	CHANNEL_3_X1_2   = 0x1b,
	CHANNEL_4_X1_2   = 0x1c,
	CHANNEL_5_X1_2   = 0x1d,
	CHANNEL_1V1   = 0x1e,
	CHANNEL_0V   = 0x1f,
	CHANNEL_8   = 0x20,
	CHANNEL_9   = 0x21,
	CHANNEL_10   = 0x22,
	CHANNEL_11   = 0x23,
	CHANNEL_12   = 0x24,
	CHANNEL_13   = 0x25,
	CHANNEL_14   = 0x26,
	CHANNEL_15   = 0x27,
	CHANNEL_8_X10_8   = 0x28,
	CHANNEL_9_X10_8   = 0x29,
	CHANNEL_8_X200_8   = 0x2a,
	CHANNEL_9_X200_8   = 0x2b,
	CHANNEL_10_X10_10   = 0x2c,
	CHANNEL_11_X10_10   = 0x2d,
	CHANNEL_10_X200_10   = 0x2e,
	CHANNEL_11_X200_10   = 0x2f,
	CHANNEL_8_X1_9   = 0x30,
	CHANNEL_9_X1_9   = 0x31,
	CHANNEL_10_X1_9   = 0x32,
	CHANNEL_11_X1_9   = 0x33,
	CHANNEL_12_X1_9   = 0x34,
	CHANNEL_13_X1_9   = 0x35,
	CHANNEL_14_X1_9   = 0x36,
	CHANNEL_15_X1_9   = 0x37,
	CHANNEL_8_X1_10   = 0x38,
	CHANNEL_9_X1_10   = 0x39,
	CHANNEL_10_X1_10   = 0x3a,
	CHANNEL_11_X1_10   = 0x3b,
	CHANNEL_12_X1_10   = 0x3c,
	CHANNEL_13_X1_10   = 0x3d
};

//                   Dir,   Output, Input,  Bit,PCINT
#define MHV_PIN_A0	0x21,   0x22,   0x20,   0,  -1
#define MHV_PIN_A1	0x21,   0x22,   0x20,   1,  -1
#define MHV_PIN_A2	0x21,   0x22,   0x20,   2,  -1
#define MHV_PIN_A3	0x21,   0x22,   0x20,   3,  -1
#define MHV_PIN_A4	0x21,   0x22,   0x20,   4,  -1
#define MHV_PIN_A5	0x21,   0x22,   0x20,   5,  -1
#define MHV_PIN_A6	0x21,   0x22,   0x20,   6,  -1
#define MHV_PIN_A7	0x21,   0x22,   0x20,   7,  -1
#define MHV_PIN_B0	0x24,   0x25,   0x23,   0,  0
#define MHV_PIN_B1	0x24,   0x25,   0x23,   1,  1
#define MHV_PIN_B2	0x24,   0x25,   0x23,   2,  2
#define MHV_PIN_B3	0x24,   0x25,   0x23,   3,  3
#define MHV_PIN_B4	0x24,   0x25,   0x23,   4,  4
#define MHV_PIN_B5	0x24,   0x25,   0x23,   5,  5
#define MHV_PIN_B6	0x24,   0x25,   0x23,   6,  6
#define MHV_PIN_B7	0x24,   0x25,   0x23,   7,  7
#define MHV_PIN_C0	0x27,   0x28,   0x26,   0,  -1
#define MHV_PIN_C1	0x27,   0x28,   0x26,   1,  -1
#define MHV_PIN_C2	0x27,   0x28,   0x26,   2,  -1
#define MHV_PIN_C3	0x27,   0x28,   0x26,   3,  -1
#define MHV_PIN_C4	0x27,   0x28,   0x26,   4,  -1
#define MHV_PIN_C5	0x27,   0x28,   0x26,   5,  -1
#define MHV_PIN_C6	0x27,   0x28,   0x26,   6,  -1
#define MHV_PIN_C7	0x27,   0x28,   0x26,   7,  -1
#define MHV_PIN_D0	0x2a,   0x2b,   0x29,   0,  -1
#define MHV_PIN_D1	0x2a,   0x2b,   0x29,   1,  -1
#define MHV_PIN_D2	0x2a,   0x2b,   0x29,   2,  -1
#define MHV_PIN_D3	0x2a,   0x2b,   0x29,   3,  -1
#define MHV_PIN_D4	0x2a,   0x2b,   0x29,   4,  -1
#define MHV_PIN_D5	0x2a,   0x2b,   0x29,   5,  -1
#define MHV_PIN_D6	0x2a,   0x2b,   0x29,   6,  -1
#define MHV_PIN_D7	0x2a,   0x2b,   0x29,   7,  -1
#define MHV_PIN_E0	0x2d,   0x2e,   0x2c,   0,  -1
#define MHV_PIN_E1	0x2d,   0x2e,   0x2c,   1,  -1
#define MHV_PIN_E2	0x2d,   0x2e,   0x2c,   2,  -1
#define MHV_PIN_E3	0x2d,   0x2e,   0x2c,   3,  -1
#define MHV_PIN_E4	0x2d,   0x2e,   0x2c,   4,  -1
#define MHV_PIN_E5	0x2d,   0x2e,   0x2c,   5,  -1
#define MHV_PIN_E6	0x2d,   0x2e,   0x2c,   6,  -1
#define MHV_PIN_E7	0x2d,   0x2e,   0x2c,   7,  -1
#define MHV_PIN_F0	0x30,   0x31,   0x2f,   0,  -1
#define MHV_PIN_F1	0x30,   0x31,   0x2f,   1,  -1
#define MHV_PIN_F2	0x30,   0x31,   0x2f,   2,  -1
#define MHV_PIN_F3	0x30,   0x31,   0x2f,   3,  -1
#define MHV_PIN_F4	0x30,   0x31,   0x2f,   4,  -1
#define MHV_PIN_F5	0x30,   0x31,   0x2f,   5,  -1
#define MHV_PIN_F6	0x30,   0x31,   0x2f,   6,  -1
#define MHV_PIN_F7	0x30,   0x31,   0x2f,   7,  -1
#define MHV_PIN_G0	0x33,   0x34,   0x32,   0,  -1
#define MHV_PIN_G1	0x33,   0x34,   0x32,   1,  -1
#define MHV_PIN_G2	0x33,   0x34,   0x32,   2,  -1
#define MHV_PIN_G3	0x33,   0x34,   0x32,   3,  -1
#define MHV_PIN_G4	0x33,   0x34,   0x32,   4,  -1
#define MHV_PIN_G5	0x33,   0x34,   0x32,   5,  -1

#define MHV_PC_INT_COUNT	24


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


#endif // MHV_IO_ATMEGA2561_H_

