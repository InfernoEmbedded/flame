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


#ifndef MHV_IO_ATTINY2313_H_
#define MHV_IO_ATTINY2313_H_

#include <avr/io.h>

// 8 bit timers
//					 type,                        ctrlRegA,ctrlRegB,overflow1,overflow2,counter,interrupt,intEnable1, intEnable2, mode
#define MHV_TIMER8_0 MHV_TIMER_TYPE_5_PRESCALERS, &TCCR0A, &TCCR0B, &OCR0A,   &OCR0B,   &TCNT0, &TIMSK,  _BV(OCIE0A),_BV(OCIE0B), _BV(WGM01)

#define MHV_TIMER0_INTERRUPTS SIG_OUTPUT_COMPARE0A, SIG_OUTPUT_COMPARE0B

// 16 bit timers
//					  ctrlRegA,ctrlRegB,ctrlRegC,overflow1,overflow2,overflow3,counter,interrupt,intEnable1, intEnable2, intEnable3,   mode
#define MHV_TIMER16_1 &TCCR1A, &TCCR1B, 0,       &OCR1A,   &OCR1B,   0,        &TCNT1, &TIMSK,  _BV(OCIE1A),_BV(OCIE1B), 0,           _BV(WGM12)

#define MHV_TIMER1_INTERRUPTS SIG_OUTPUT_COMPARE1A, SIG_OUTPUT_COMPARE1B
#define MHV_USART0 &UBRRH, &UBRRL, &UCSRA, &UCSRB, &UDR, RXEN, TXEN, RXCIE, TXCIE, UDRE, U2X
#define MHV_USART0_INTERRUPTS USART_RX_vect, USART_TX_vect

//					Dir,	Output,	Input,	Bit, Interrupt
#define MHV_PIN_A0	&DDRA,	&PORTA, &PINA,	0
#define MHV_PIN_A1	&DDRA,	&PORTA, &PINA,	1
#define MHV_PIN_A2	&DDRA,	&PORTA, &PINA,	2
#define MHV_PIN_B0	&DDRB,	&PORTB, &PINB,	0
#define MHV_PIN_B1	&DDRB,	&PORTB, &PINB,	1
#define MHV_PIN_B2	&DDRB,	&PORTB, &PINB,	2
#define MHV_PIN_B3	&DDRB,	&PORTB, &PINB,	3
#define MHV_PIN_B4	&DDRB,	&PORTB, &PINB,	4
#define MHV_PIN_B5	&DDRB,	&PORTB, &PINB,	5
#define MHV_PIN_B6	&DDRB,	&PORTB, &PINB,	6
#define MHV_PIN_B7	&DDRB,	&PORTB, &PINB,	7
#define MHV_PIN_D0	&DDRD,	&PORTD, &PIND,	0
#define MHV_PIN_D1	&DDRD,	&PORTD, &PIND,	1
#define MHV_PIN_D2	&DDRD,	&PORTD, &PIND,	2
#define MHV_PIN_D3	&DDRD,	&PORTD, &PIND,	3
#define MHV_PIN_D4	&DDRD,	&PORTD, &PIND,	4
#define MHV_PIN_D5	&DDRD,	&PORTD, &PIND,	5
#define MHV_PIN_D6	&DDRD,	&PORTD, &PIND,	6

#define MHV_INTERRUPT_INT0 INT0_vect, &MCUCR, ISC00
#define MHV_INTERRUPT_INT1 INT1_vect, &MCUCR, ISC10

#endif /* MHV_IO_ATTINY2313_H_ */
