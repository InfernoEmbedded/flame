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


#ifndef FLAME_IO_ATMEGA48HVF_H_
#define FLAME_IO_ATMEGA48HVF_H_

#include <avr/io.h>

#ifdef INT0_vect
#define FLAME_INTERRUPT_INT0 INT0_vect, &MCUCR, ISC00
#endif

#ifdef INT1_vect
#define FLAME_INTERRUPT_INT1 INT1_vect, &MCUCR, ISC10
#endif

//						bits,type,                            ctrlRegA,ctrlRegB,ctrlRegC,overflow1,overflow2,overflow3,inputCapture1,counter,interrupt,intEnable




// USART			Baud   Status Control I/O
//      			ubrr,  ucsra, ucsrb, ucsrc,  udr,   rxen,  txen,  rxcie,  txcie,  udre,  u2x




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

#define FLAME_PC_INT_COUNT	24


#define FLAME_PIN_TIMER_0_A	FLAME_PIN_D6
#define FLAME_PIN_TIMER_0_B	FLAME_PIN_D5
#define FLAME_PIN_TIMER_1_A	FLAME_PIN_B1
#define FLAME_PIN_TIMER_1_B	FLAME_PIN_B2
#define FLAME_PIN_TIMER_2_A	FLAME_PIN_B3
#define FLAME_PIN_TIMER_2_B	FLAME_PIN_D3


#endif // FLAME_IO_ATMEGA48HVF_H_

