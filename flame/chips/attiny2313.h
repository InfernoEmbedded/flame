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


#ifndef FLAME_IO_ATTINY2313_H_
#define FLAME_IO_ATTINY2313_H_

#include <avr/io.h>

#ifdef INT0_vect
#define FLAME_INTERRUPT_INT0 INT0_vect, &EICRA, ISC00, &EIMSK, INT0
#endif

#ifdef INT1_vect
#define FLAME_INTERRUPT_INT1 INT1_vect, &EICRA, ISC10, &EIMSK, INT1
#endif

//						bits,type,                            ctrlRegA,ctrlRegB,ctrlRegC,overflow1,overflow2,overflow3,inputCapture1,counter,interrupt,intEnable
#define FLAME_TIMER8_0	8,   TimerType::HAS_5_PRESCALERS,     0x50,    0x53,    0,       0x56,     0x5c,     0,        0,            0x52,   0,       0x59,     0
#define FLAME_TIMER16_1	16,  TimerType::HAS_5_PRESCALERS,     0x4f,    0x4e,    0x42,    0x4a,     0x48,     0,        0x44,         0x4c,   0,       0x59,     6


#define FLAME_TIMER0_INTERRUPTS TIMER0_COMPA_vect, TIMER0_COMPB_vect, 0
#define FLAME_TIMER1_INTERRUPTS TIMER1_COMPA_vect, TIMER1_COMPB_vect, 0


// USART			Baud   Status Control I/O
//      			ubrr,  ucsra, ucsrb, ucsrc,  udr,   rxen,  txen,  rxcie,  txcie,  udre,  u2x
#define FLAME_USART0	0x29,  0x2b,  0x2a,   0x23,   0x2c,  RXEN,  TXEN,  RXCIE,  TXCIE,  UDRE,  U2X


#define FLAME_USART0_INTERRUPTS	USART_RX_vect, USART_TX_vect


//                   Dir,   Output, Input,  Bit,PCINT
#define FLAME_PIN_A0	0x3a,   0x3b,   0x39,   0,  8
#define FLAME_PIN_A1	0x3a,   0x3b,   0x39,   1,  9
#define FLAME_PIN_A2	0x3a,   0x3b,   0x39,   2,  10
#define FLAME_PIN_B0	0x37,   0x38,   0x36,   0,  0
#define FLAME_PIN_B1	0x37,   0x38,   0x36,   1,  1
#define FLAME_PIN_B2	0x37,   0x38,   0x36,   2,  2
#define FLAME_PIN_B3	0x37,   0x38,   0x36,   3,  3
#define FLAME_PIN_B4	0x37,   0x38,   0x36,   4,  4
#define FLAME_PIN_B5	0x37,   0x38,   0x36,   5,  5
#define FLAME_PIN_B6	0x37,   0x38,   0x36,   6,  6
#define FLAME_PIN_B7	0x37,   0x38,   0x36,   7,  7
#define FLAME_PIN_D0	0x31,   0x32,   0x30,   0,  11
#define FLAME_PIN_D1	0x31,   0x32,   0x30,   1,  12
#define FLAME_PIN_D2	0x31,   0x32,   0x30,   2,  13
#define FLAME_PIN_D3	0x31,   0x32,   0x30,   3,  14
#define FLAME_PIN_D4	0x31,   0x32,   0x30,   4,  15
#define FLAME_PIN_D5	0x31,   0x32,   0x30,   5,  16
#define FLAME_PIN_D6	0x31,   0x32,   0x30,   6,  17

#define FLAME_PC_INT_COUNT	18


#define FLAME_PIN_TIMER_0_A	FLAME_PIN_B2
#define FLAME_PIN_TIMER_0_B	FLAME_PIN_D5
#define FLAME_PIN_TIMER_1_A	FLAME_PIN_B3
#define FLAME_PIN_TIMER_1_B	FLAME_PIN_B4


#endif // FLAME_IO_ATTINY2313_H_

