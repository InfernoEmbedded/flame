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


#ifndef FLAME_IO_ATTINY45_H_
#define FLAME_IO_ATTINY45_H_

#include <avr/io.h>

#ifdef INT0_vect
#define FLAME_INTERRUPT_INT0 INT0_vect, &EICRA, ISC00, &EIMSK, INT0
#endif

#ifdef INT1_vect
#define FLAME_INTERRUPT_INT1 INT1_vect, &EICRA, ISC10, &EIMSK, INT1
#endif

//						bits,type,                            ctrlRegA,ctrlRegB,ctrlRegC,overflow1,overflow2,overflow3,inputCapture1,counter,interrupt,intEnable
#define FLAME_TIMER8_0	8,   TimerType::HAS_5_PRESCALERS,     0x4a,    0x53,    0,       0x49,     0x48,     0,        0,            0x52,   0,       0x59,     4


#define FLAME_TIMER0_INTERRUPTS TIMER0_COMPA_vect, TIMER0_COMPB_vect, 0
#define FLAME_TIMER1_INTERRUPTS TIMER1_COMPA_vect, TIMER1_COMPB_vect, 0


// USART			Baud   Status Control I/O
//      			ubrr,  ucsra, ucsrb, ucsrc,  udr,   rxen,  txen,  rxcie,  txcie,  udre,  u2x




enum class ADCReference {
	VCC	= (0X00 << 4),
	AREF	= (0x04 << 4),
	REF1V1	= (0x08 << 6),
	REF2V56	= (0x09 << 6),
	REF2V56_AREF	= (0x0d << 6)
};

enum class ADCChannel {
	UNDEFINED   = 0xff,
	CHANNEL_0   = 0x00,
	CHANNEL_1   = 0x01,
	CHANNEL_2   = 0x02,
	CHANNEL_3   = 0x03,
	CHANNEL_2_X1_2   = 0x04,
	CHANNEL_2_X20_2   = 0x05,
	CHANNEL_2_X1_3   = 0x06,
	CHANNEL_2_X20_3   = 0x07,
	CHANNEL_0_X1_0   = 0x08,
	CHANNEL_0_X20_0   = 0x09,
	CHANNEL_0_X1_1   = 0x0a,
	CHANNEL_0_X20_1   = 0x0b,
	CHANNEL_V_BANDGAP   = 0x0c,
	CHANNEL_0V   = 0x0d,
	CHANNEL_TEMPERATURE   = 0x0f
};

//                   Dir,   Output, Input,  Bit,PCINT
#define FLAME_PIN_B0	0x37,   0x38,   0x36,   0,  0
#define FLAME_PIN_B1	0x37,   0x38,   0x36,   1,  1
#define FLAME_PIN_B2	0x37,   0x38,   0x36,   2,  2
#define FLAME_PIN_B3	0x37,   0x38,   0x36,   3,  3
#define FLAME_PIN_B4	0x37,   0x38,   0x36,   4,  4
#define FLAME_PIN_B5	0x37,   0x38,   0x36,   5,  5

#define FLAME_PC_INT_COUNT	6


#define FLAME_PIN_TIMER_0_A	FLAME_PIN_B0
#define FLAME_PIN_TIMER_0_B	FLAME_PIN_B1
#define FLAME_PIN_TIMER_1_A	FLAME_PIN_B1
#define FLAME_PIN_TIMER_1_B	FLAME_PIN_B4


#endif // FLAME_IO_ATTINY45_H_

