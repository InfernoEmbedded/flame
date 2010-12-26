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


#ifndef MHV_IO_ARDUINO_MEGA_H_
#define MHV_IO_ARDUINO_MEGA_H_

#include <avr/io.h>

//							Dir,	Output,	Input,	Bit,PCINT
#define MHV_ARDUINO_PIN_0	&DDRE,	&PORTE,	&PINE,	0,	8	//USART0_RX
#define MHV_ARDUINO_PIN_1	&DDRE,	&PORTE,	&PINE,	1,	-1	//USART0_TX
#define MHV_ARDUINO_PIN_2	&DDRE,	&PORTE,	&PINE,	4,	-1	//PWM2
#define MHV_ARDUINO_PIN_3	&DDRE,	&PORTE,	&PINE,	5,	-1	//PWM3
#define MHV_ARDUINO_PIN_4	&DDRG,	&PORTG,	&PING,	5,	-1	//PWM4
#define MHV_ARDUINO_PIN_5	&DDRE,	&PORTE,	&PINE,	3,	-1	//PWM5
#define MHV_ARDUINO_PIN_6	&DDRH,	&PORTH,	&PINH,	3,	-1	//PWM6
#define MHV_ARDUINO_PIN_7	&DDRH,	&PORTH,	&PINH,	4,	-1	//PWM7
#define MHV_ARDUINO_PIN_8	&DDRH,	&PORTH,	&PINH,	5,	-1	//PWM8
#define MHV_ARDUINO_PIN_9	&DDRH,	&PORTH,	&PINH,	6,	-1	//PWM9
#define MHV_ARDUINO_PIN_10	&DDRB,	&PORTB,	&PINB,	4,	4	//PWM10
#define MHV_ARDUINO_PIN_11	&DDRB,	&PORTB,	&PINB,	5,	5	//PWM11
#define MHV_ARDUINO_PIN_12	&DDRB,	&PORTB,	&PINB,	6,	6	//PWM12
#define MHV_ARDUINO_PIN_13	&DDRB,	&PORTB,	&PINB,	7,	7	//PWM13
#define MHV_ARDUINO_PIN_14	&DDRJ,	&PORTJ,	&PINJ,	1,	10	//USART3_TX
#define MHV_ARDUINO_PIN_15	&DDRJ,	&PORTJ,	&PINJ,	0,	9	//USART3_RX
#define MHV_ARDUINO_PIN_16	&DDRH,	&PORTH,	&PINH,	1,	-1	//USART2_TX
#define MHV_ARDUINO_PIN_17	&DDRH,	&PORTH,	&PINH,	0,	-1	//USART2_RX
#define MHV_ARDUINO_PIN_18	&DDRD,	&PORTD,	&PIND,	3,	-1	//USART1_TX
#define MHV_ARDUINO_PIN_19	&DDRD,	&PORTD,	&PIND,	2,	-1	//USART1_RX
#define MHV_ARDUINO_PIN_20	&DDRD,	&PORTD,	&PIND,	1,	-1	//I2C_SDA
#define MHV_ARDUINO_PIN_21	&DDRD,	&PORTD,	&PIND,	0,	-1	//I2C_SCL
#define MHV_ARDUINO_PIN_22	&DDRA,	&PORTA,	&PINA,	0,	-1	//D22
#define MHV_ARDUINO_PIN_23	&DDRA,	&PORTA,	&PINA,	1,	-1	//D23
#define MHV_ARDUINO_PIN_24	&DDRA,	&PORTA,	&PINA,	2,	-1	//D24
#define MHV_ARDUINO_PIN_25	&DDRA,	&PORTA,	&PINA,	3,	-1	//D25
#define MHV_ARDUINO_PIN_26	&DDRA,	&PORTA,	&PINA,	4,	-1	//D26
#define MHV_ARDUINO_PIN_27	&DDRA,	&PORTA,	&PINA,	5,	-1	//D27
#define MHV_ARDUINO_PIN_28	&DDRA,	&PORTA,	&PINA,	6,	-1	//D28
#define MHV_ARDUINO_PIN_29	&DDRA,	&PORTA,	&PINA,	7,	-1	//D29
#define MHV_ARDUINO_PIN_30	&DDRC,	&PORTC,	&PINC,	7,	-1	//D30
#define MHV_ARDUINO_PIN_31	&DDRC,	&PORTC,	&PINC,	6,	-1	//D31
#define MHV_ARDUINO_PIN_32	&DDRC,	&PORTC,	&PINC,	5,	-1	//D32
#define MHV_ARDUINO_PIN_33	&DDRC,	&PORTC,	&PINC,	4,	-1	//D33
#define MHV_ARDUINO_PIN_34	&DDRC,	&PORTC,	&PINC,	3,	-1	//D34
#define MHV_ARDUINO_PIN_35	&DDRC,	&PORTC,	&PINC,	2,	-1	//D35
#define MHV_ARDUINO_PIN_36	&DDRC,	&PORTC,	&PINC,	1,	-1	//D36
#define MHV_ARDUINO_PIN_37	&DDRC,	&PORTC,	&PINC,	0,	-1	//D37
#define MHV_ARDUINO_PIN_38	&DDRD,	&PORTD,	&PIND,	7,	-1	//D38
#define MHV_ARDUINO_PIN_39	&DDRG,	&PORTG,	&PING,	2,	-1	//D39
#define MHV_ARDUINO_PIN_40	&DDRG,	&PORTG,	&PING,	1,	-1	//D40
#define MHV_ARDUINO_PIN_41	&DDRG,	&PORTG,	&PING,	0,	-1	//D41
#define MHV_ARDUINO_PIN_42	&DDRL,	&PORTL,	&PINL,	7,	-1	//D42
#define MHV_ARDUINO_PIN_43	&DDRL,	&PORTL,	&PINL,	6,	-1	//D43
#define MHV_ARDUINO_PIN_44	&DDRL,	&PORTL,	&PINL,	5,	-1	//D44
#define MHV_ARDUINO_PIN_45	&DDRL,	&PORTL,	&PINL,	4,	-1	//D45
#define MHV_ARDUINO_PIN_46	&DDRL,	&PORTL,	&PINL,	3,	-1	//D46
#define MHV_ARDUINO_PIN_47	&DDRL,	&PORTL,	&PINL,	2,	-1	//D47
#define MHV_ARDUINO_PIN_48	&DDRL,	&PORTL,	&PINL,	1,	-1	//D48
#define MHV_ARDUINO_PIN_49	&DDRL,	&PORTL,	&PINL,	0,	-1	//D49
#define MHV_ARDUINO_PIN_50	&DDRB,	&PORTB,	&PINB,	3,	3	//SPI_MISO
#define MHV_ARDUINO_PIN_51	&DDRB,	&PORTB,	&PINB,	2,	2	//SPI_MOSI
#define MHV_ARDUINO_PIN_52	&DDRB,	&PORTB,	&PINB,	1,	1	//SPI_SCK
#define MHV_ARDUINO_PIN_53	&DDRB,	&PORTB,	&PINB,	0,	0	//SPI_SS
#define MHV_ARDUINO_PIN_54	&DDRF,	&PORTF,	&PINF,	0,	-1	//A0
#define MHV_ARDUINO_PIN_55	&DDRF,	&PORTF,	&PINF,	1,	-1	//A1
#define MHV_ARDUINO_PIN_56	&DDRF,	&PORTF,	&PINF,	2,	-1	//A2
#define MHV_ARDUINO_PIN_57	&DDRF,	&PORTF,	&PINF,	3,	-1	//A3
#define MHV_ARDUINO_PIN_58	&DDRF,	&PORTF,	&PINF,	4,	-1	//A4
#define MHV_ARDUINO_PIN_59	&DDRF,	&PORTF,	&PINF,	5,	-1	//A5
#define MHV_ARDUINO_PIN_60	&DDRF,	&PORTF,	&PINF,	6,	-1	//A6
#define MHV_ARDUINO_PIN_61	&DDRF,	&PORTF,	&PINF,	7,	-1	//A7
#define MHV_ARDUINO_PIN_62	&DDRK,	&PORTK,	&PINK,	0,	16	//A8
#define MHV_ARDUINO_PIN_63	&DDRK,	&PORTK,	&PINK,	1,	17	//A9
#define MHV_ARDUINO_PIN_64	&DDRK,	&PORTK,	&PINK,	2,	18	//A10
#define MHV_ARDUINO_PIN_65	&DDRK,	&PORTK,	&PINK,	3,	19	//A11
#define MHV_ARDUINO_PIN_66	&DDRK,	&PORTK,	&PINK,	4,	20	//A12
#define MHV_ARDUINO_PIN_67	&DDRK,	&PORTK,	&PINK,	5,	21	//A13
#define MHV_ARDUINO_PIN_68	&DDRK,	&PORTK,	&PINK,	6,	22	//A14
#define MHV_ARDUINO_PIN_69	&DDRK,	&PORTK,	&PINK,	7,	23	//A15


#endif /* MHV_IO_ARDUINO_MEGA_H_ */
