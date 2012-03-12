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


#ifndef MHV_IO_ATTINY2313_H_
#define MHV_IO_ATTINY2313_H_

#include <avr/io.h>

// 8 bit timers
//					 type,                        ctrlRegA,ctrlRegB,overflow1,overflow2,counter,interrupt,intEnableA
#define MHV_TIMER8_0 MHV_TIMER_TYPE_5_PRESCALERS, &TCCR0A, &TCCR0B, &OCR0A,   &OCR0B,   &TCNT0, &TIMSK,   OCIE0A

#define MHV_TIMER0_INTERRUPTS SIG_OUTPUT_COMPARE0A, SIG_OUTPUT_COMPARE0B

// 16 bit timers
//					  ctrlRegA,ctrlRegB,ctrlRegC,overflow1,overflow2,overflow3,counter,interrupt,inputCapture1
#define MHV_TIMER16_1 &TCCR1A, &TCCR1B, 0,       &OCR1A,   &OCR1B,   0,        &TCNT1, &TIMSK,   &ICR1

#define MHV_TIMER1_INTERRUPTS SIG_OUTPUT_COMPARE1A, SIG_OUTPUT_COMPARE1B
#define MHV_USART0 &UBRRH, &UBRRL, &UCSRA, &UCSRB, &UDR, RXEN, TXEN, RXCIE, TXCIE, UDRE, U2X
#define MHV_USART0_INTERRUPTS USART_RX_vect, USART_TX_vect

//							Dir,	Output,	Input,	Bit,PCINT
#define MHV_PIN_A0			&DDRA,	&PORTA, &PINA,	0,	-1
#define MHV_PIN_A1			&DDRA,	&PORTA, &PINA,	1,	-1
#define MHV_PIN_A2			&DDRA,	&PORTA, &PINA,	2,	-1
#define MHV_PIN_B0			&DDRB,	&PORTB, &PINB,	0,	0
#define MHV_PIN_B1			&DDRB,	&PORTB, &PINB,	1,	1
#define MHV_PIN_B2			&DDRB,	&PORTB, &PINB,	2,	2
#define MHV_PIN_B3			&DDRB,	&PORTB, &PINB,	3,	3
#define MHV_PIN_B4			&DDRB,	&PORTB, &PINB,	4,	4
#define MHV_PIN_B5			&DDRB,	&PORTB, &PINB,	5,	5
#define MHV_PIN_B6			&DDRB,	&PORTB, &PINB,	6,	6
#define MHV_PIN_B7			&DDRB,	&PORTB, &PINB,	7,	7
#define MHV_PIN_D0			&DDRD,	&PORTD, &PIND,	0,	-1
#define MHV_PIN_D1			&DDRD,	&PORTD, &PIND,	1,	-1
#define MHV_PIN_D2			&DDRD,	&PORTD, &PIND,	2,	-1
#define MHV_PIN_D3			&DDRD,	&PORTD, &PIND,	3,	-1
#define MHV_PIN_D4			&DDRD,	&PORTD, &PIND,	4,	-1
#define MHV_PIN_D5			&DDRD,	&PORTD, &PIND,	5,	-1
#define MHV_PIN_D6			&DDRD,	&PORTD, &PIND,	6,	-1

#define MHV_TEMPLATE_PIN_A0	0x01A,	0x01B,	0x019,	0,	-1
#define MHV_TEMPLATE_PIN_A1	0x01A,	0x01B,	0x019,	1,	-1
#define MHV_TEMPLATE_PIN_A2	0x01A,	0x01B,	0x019,	2,	-1
#define MHV_TEMPLATE_PIN_B0	0x017,	0x018,	0x016,	0,	0
#define MHV_TEMPLATE_PIN_B1	0x017,	0x018,	0x016,	1,	1
#define MHV_TEMPLATE_PIN_B2	0x017,	0x018,	0x016,	2,	2
#define MHV_TEMPLATE_PIN_B3	0x017,	0x018,	0x016,	3,	3
#define MHV_TEMPLATE_PIN_B4	0x017,	0x018,	0x016,	4,	4
#define MHV_TEMPLATE_PIN_B5	0x017,	0x018,	0x016,	5,	5
#define MHV_TEMPLATE_PIN_B6	0x017,	0x018,	0x016,	6,	6
#define MHV_TEMPLATE_PIN_B7	0x017,	0x018,	0x016,	7,	7
#define MHV_TEMPLATE_PIN_D0	0x011,	0x012,	0x010,	0,	-1
#define MHV_TEMPLATE_PIN_D1	0x011,	0x012,	0x010,	1,	-1
#define MHV_TEMPLATE_PIN_D2	0x011,	0x012,	0x010,	2,	-1
#define MHV_TEMPLATE_PIN_D3	0x011,	0x012,	0x010,	3,	-1
#define MHV_TEMPLATE_PIN_D4	0x011,	0x012,	0x010,	4,	-1
#define MHV_TEMPLATE_PIN_D5	0x011,	0x012,	0x010,	5,	-1
#define MHV_TEMPLATE_PIN_D6	0x011,	0x012,	0x010,	6,	-1


#define MHV_PIN_TIMER_0_A	MHV_PIN_B2
#define MHV_PIN_TIMER_0_B	MHV_PIN_D5
#define MHV_PIN_TIMER_1_A	MHV_PIN_B3
#define MHV_PIN_TIMER_1_B	MHV_PIN_B4

#define MHV_TEMPLATE_PIN_TIMER_0_A	MHV_TEMPLATE_PIN_B2
#define MHV_TEMPLATE_PIN_TIMER_0_B	MHV_TEMPLATE_PIN_D5
#define MHV_TEMPLATE_PIN_TIMER_1_A	MHV_TEMPLATE_PIN_B3
#define MHV_TEMPLATE_PIN_TIMER_1_B	MHV_TEMPLATE_PIN_B4


#define MHV_INTERRUPT_INT0 INT0_vect, &MCUCR, ISC00
#define MHV_INTERRUPT_INT1 INT1_vect, &MCUCR, ISC10

#define MHV_PC_INT_COUNT	8

#define MHV_EEPROM_VECT		EEPROM_READY_vect

#endif /* MHV_IO_ATTINY2313_H_ */
