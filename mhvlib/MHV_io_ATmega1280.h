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
//					 type,                        ctrlRegA,ctrlRegB,overflow1,overflow2,counter,interrupt,intEnable1, intEnable2, mode
#define MHV_TIMER8_0 MHV_TIMER_TYPE_5_PRESCALERS, &TCCR0A, &TCCR0B, &OCR0A,   &OCR0B,   &TCNT0, &TIMSK0,  _BV(OCIE0A),_BV(OCIE0B), _BV(WGM01)
#define MHV_TIMER8_2 MHV_TIMER_TYPE_7_PRESCALERS, &TCCR2A, &TCCR2B, &OCR2A,   &OCR2B,   &TCNT2, &TIMSK2,  _BV(OCIE2A),_BV(OCIE2B), _BV(WGM21)

#define MHV_TIMER0_INTERRUPTS SIG_OUTPUT_COMPARE0A, SIG_OUTPUT_COMPARE0B
#define MHV_TIMER2_INTERRUPTS SIG_OUTPUT_COMPARE2A, SIG_OUTPUT_COMPARE2B

// 16 bit timers
//					  ctrlRegA,ctrlRegB,ctrlRegC,overflow1,overflow2,overflow3,counter,interrupt,intEnable1, intEnable2, intEnable3,   mode
#define MHV_TIMER16_1 &TCCR1A, &TCCR1B, &TCCR1C, &OCR1A,   &OCR1B,   &OCR1C,   &TCNT1, &TIMSK1,  _BV(OCIE1A),_BV(OCIE1B), _BV(OCIE1C), _BV(WGM12)
#define MHV_TIMER16_3 &TCCR3A, &TCCR3B, &TCCR3C, &OCR3A,   &OCR3B,   &OCR3C,   &TCNT3, &TIMSK3,  _BV(OCIE3A),_BV(OCIE3B), _BV(OCIE3C), _BV(WGM32)
#define MHV_TIMER16_4 &TCCR4A, &TCCR4B, &TCCR4C, &OCR4A,   &OCR4B,   &OCR4C,   &TCNT4, &TIMSK4,  _BV(OCIE4A),_BV(OCIE4B), _BV(OCIE4C), _BV(WGM42)
#define MHV_TIMER16_5 &TCCR5A, &TCCR5B, &TCCR5C, &OCR5A,   &OCR5B,   &OCR5C,   &TCNT5, &TIMSK5,  _BV(OCIE5A),_BV(OCIE5B), _BV(OCIE5C), _BV(WGM52)

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


#endif /* MHV_IO_ATMEGA1280_H_ */
