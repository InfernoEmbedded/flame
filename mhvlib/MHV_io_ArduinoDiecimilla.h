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


#ifndef MHV_IO_ARDUINO_DIECIMILLA_H_
#define MHV_IO_ARDUINO_DIECIMILLA_H_

#include <avr/io.h>

//							Dir,	Output,	Input,	Bit,PCINT
#define MHV_ARDUINO_PIN_0	&DDRD,	&PORTD,	&PIND,	0,	16
#define MHV_ARDUINO_PIN_1	&DDRD,	&PORTD,	&PIND,	1,	17
#define MHV_ARDUINO_PIN_2	&DDRD,	&PORTD,	&PIND,	2,	18
#define MHV_ARDUINO_PIN_3	&DDRD,	&PORTD,	&PIND,	3,	19
#define MHV_ARDUINO_PIN_4	&DDRD,	&PORTD,	&PIND,	4,	20
#define MHV_ARDUINO_PIN_5	&DDRD,	&PORTD,	&PIND,	5,	21
#define MHV_ARDUINO_PIN_6	&DDRD,	&PORTD,	&PIND,	6,	22
#define MHV_ARDUINO_PIN_7	&DDRD,	&PORTD,	&PIND,	7,	23
#define MHV_ARDUINO_PIN_8	&DDRB,	&PORTB,	&PINB,	0,	0
#define MHV_ARDUINO_PIN_9	&DDRB,	&PORTB,	&PINB,	1,	1
#define MHV_ARDUINO_PIN_10	&DDRB,	&PORTB,	&PINB,	2,	2
#define MHV_ARDUINO_PIN_11	&DDRB,	&PORTB,	&PINB,	3,	3
#define MHV_ARDUINO_PIN_12	&DDRB,	&PORTB,	&PINB,	4,	4
#define MHV_ARDUINO_PIN_13	&DDRB,	&PORTB,	&PINB,	5,	5
#define MHV_ARDUINO_PIN_A0	&DDRC,	&PORTC,	&PINC,	0,	8
#define MHV_ARDUINO_PIN_A1	&DDRC,	&PORTC,	&PINC,	1,	9
#define MHV_ARDUINO_PIN_A2	&DDRC,	&PORTC,	&PINC,	2,	10
#define MHV_ARDUINO_PIN_A3	&DDRC,	&PORTC,	&PINC,	3,	11
#define MHV_ARDUINO_PIN_A4	&DDRC,	&PORTC,	&PINC,	4,	12
#define MHV_ARDUINO_PIN_A5	&DDRC,	&PORTC,	&PINC,	5,	13

#endif /* MHV_IO_ARDUINO_DIECIMILLA_H_ */
