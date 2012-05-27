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


#ifndef MHV_IO_ATTINY25_H_
#define MHV_IO_ATTINY25_H_

#include <avr/io.h>

#ifdef INT0_vect
#define MHV_INTERRUPT_INT0 INT0_vect, &MCUCR, ISC00
#endif

#ifdef INT1_vect
#define MHV_INTERRUPT_INT1 INT1_vect, &MCUCR, ISC10
#endif

//						bits,type,                            ctrlRegA,ctrlRegB,ctrlRegC,overflow1,overflow2,overflow3,inputCapture1,counter,interrupt,intEnable
#define MHV_TIMER8_0	8,   MHV_TIMER_5_PRESCALERS,          0x4a,    0x53,    0,       0x49,     0x48,     0,        0,            0x52,   0x59,     4


#define MHV_TIMER0_INTERRUPTS TIMER0_COMPA_vect, TIMER0_COMPB_vect, 0
#define MHV_TIMER1_INTERRUPTS TIMER1_COMPA_vect, TIMER1_COMPB_vect, 0


// USART			Baud   Status Control I/O
//      			ubrr,  ucsra, ucsrb,  udr,   rxen,  txen,  rxcie,  txcie,  udre,  u2x




enum mhv_ad_reference {
	MHV_AD_REFERENCE_VCC	= (0X00 << 4),
	MHV_AD_REFERENCE_AREF	= (0x04 << 4),
	MHV_AD_REFERENCE_1V1	= (0x08 << 6),
	MHV_AD_REFERENCE_2V56	= (0x09 << 6),
	MHV_AD_REFERENCE_2V56_AREF	= (0x0d << 6)
};
typedef enum mhv_ad_reference MHV_AD_REFERENCE;

enum mhv_ad_channel {
	MHV_AD_CHANNEL_0   = 0x00,
	MHV_AD_CHANNEL_1   = 0x01,
	MHV_AD_CHANNEL_2   = 0x02,
	MHV_AD_CHANNEL_3   = 0x03,
	MHV_AD_CHANNEL_2_X1_2   = 0x04,
	MHV_AD_CHANNEL_2_X20_2   = 0x05,
	MHV_AD_CHANNEL_2_X1_3   = 0x06,
	MHV_AD_CHANNEL_2_X20_3   = 0x07,
	MHV_AD_CHANNEL_0_X1_0   = 0x08,
	MHV_AD_CHANNEL_0_X20_0   = 0x09,
	MHV_AD_CHANNEL_0_X1_1   = 0x0a,
	MHV_AD_CHANNEL_0_X20_1   = 0x0b,
	MHV_AD_CHANNEL_V_BANDGAP   = 0x0c,
	MHV_AD_CHANNEL_0V   = 0x0d,
	MHV_AD_CHANNEL_TEMPERATURE   = 0x0f
};
typedef enum mhv_ad_reference MHV_AD_REFERENCE;


//                   Dir,   Output, Input,  Bit,PCINT
#define MHV_PIN_B0	0x37,   0x38,   0x36,   0,  0
#define MHV_PIN_B1	0x37,   0x38,   0x36,   1,  1
#define MHV_PIN_B2	0x37,   0x38,   0x36,   2,  2
#define MHV_PIN_B3	0x37,   0x38,   0x36,   3,  3
#define MHV_PIN_B4	0x37,   0x38,   0x36,   4,  4
#define MHV_PIN_B5	0x37,   0x38,   0x36,   5,  5

#define MHV_PC_INT_COUNT	6


#define MHV_PIN_TIMER_0_A	MHV_PIN_B0
#define MHV_PIN_TIMER_0_B	MHV_PIN_B1
#define MHV_PIN_TIMER_1_A	MHV_PIN_B1
#define MHV_PIN_TIMER_1_B	MHV_PIN_B4


#endif // MHV_IO_ATTINY25_H_

