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


#ifndef MHV_IO_ARDUINO_MEGA_H_
#define MHV_IO_ARDUINO_MEGA_H_

#include <avr/io.h>

//							Dir,	Output,	Input,	Bit,PCINT
#define MHV_ARDUINO_PIN_0	0x0D,	0x0E,	0x0C,	0,	8	//USART0_RX
#define MHV_ARDUINO_PIN_1	0x0D,	0x0E,	0x0C,	1,	-1	//USART0_TX
#define MHV_ARDUINO_PIN_2	0x0D,	0x0E,	0x0C,	4,	-1	//PWM2
#define MHV_ARDUINO_PIN_3	0x0D,	0x0E,	0x0C,	5,	-1	//PWM3
#define MHV_ARDUINO_PIN_4	0x13,	0x14,	0x12,	5,	-1	//PWM4
#define MHV_ARDUINO_PIN_5	0x0D,	0x0E,	0x0C,	3,	-1	//PWM5
#define MHV_ARDUINO_PIN_6	0x101,	0x102,	0x100,	3,	-1	//PWM6
#define MHV_ARDUINO_PIN_7	0x101,	0x102,	0x100,	4,	-1	//PWM7
#define MHV_ARDUINO_PIN_8	0x101,	0x102,	0x100,	5,	-1	//PWM8
#define MHV_ARDUINO_PIN_9	0x101,	0x102,	0x100,	6,	-1	//PWM9
#define MHV_ARDUINO_PIN_10	0x04,	0x05,	0x03,	4,	4	//PWM10
#define MHV_ARDUINO_PIN_11	0x04,	0x05,	0x03,	5,	5	//PWM11
#define MHV_ARDUINO_PIN_12	0x04,	0x05,	0x03,	6,	6	//PWM12
#define MHV_ARDUINO_PIN_13	0x04,	0x05,	0x03,	7,	7	//PWM13
#define MHV_ARDUINO_PIN_14	0x101,	0x105,	0x103,	1,	10	//USART3_TX
#define MHV_ARDUINO_PIN_15	0x101,	0x105,	0x103,	0,	9	//USART3_RX
#define MHV_ARDUINO_PIN_16	0x101,	0x102,	0x100,	1,	-1	//USART2_TX
#define MHV_ARDUINO_PIN_17	0x101,	0x102,	0x100,	0,	-1	//USART2_RX
#define MHV_ARDUINO_PIN_18	0x0A,	0x0B,	0x09,	3,	-1	//USART1_TX
#define MHV_ARDUINO_PIN_19	0x0A,	0x0B,	0x09,	2,	-1	//USART1_RX
#define MHV_ARDUINO_PIN_20	0x0A,	0x0B,	0x09,	1,	-1	//I2C_SDA
#define MHV_ARDUINO_PIN_21	0x0A,	0x0B,	0x09,	0,	-1	//I2C_SCL
#define MHV_ARDUINO_PIN_22	0x01,	0x02,	0x00,	0,	-1	//D22
#define MHV_ARDUINO_PIN_23	0x01,	0x02,	0x00,	1,	-1	//D23
#define MHV_ARDUINO_PIN_24	0x01,	0x02,	0x00,	2,	-1	//D24
#define MHV_ARDUINO_PIN_25	0x01,	0x02,	0x00,	3,	-1	//D25
#define MHV_ARDUINO_PIN_26	0x01,	0x02,	0x00,	4,	-1	//D26
#define MHV_ARDUINO_PIN_27	0x01,	0x02,	0x00,	5,	-1	//D27
#define MHV_ARDUINO_PIN_28	0x01,	0x02,	0x00,	6,	-1	//D28
#define MHV_ARDUINO_PIN_29	0x01,	0x02,	0x00,	7,	-1	//D29
#define MHV_ARDUINO_PIN_30	0x07,	0x08,	0x06,	7,	-1	//D30
#define MHV_ARDUINO_PIN_31	0x07,	0x08,	0x06,	6,	-1	//D31
#define MHV_ARDUINO_PIN_32	0x07,	0x08,	0x06,	5,	-1	//D32
#define MHV_ARDUINO_PIN_33	0x07,	0x08,	0x06,	4,	-1	//D33
#define MHV_ARDUINO_PIN_34	0x07,	0x08,	0x06,	3,	-1	//D34
#define MHV_ARDUINO_PIN_35	0x07,	0x08,	0x06,	2,	-1	//D35
#define MHV_ARDUINO_PIN_36	0x07,	0x08,	0x06,	1,	-1	//D36
#define MHV_ARDUINO_PIN_37	0x07,	0x08,	0x06,	0,	-1	//D37
#define MHV_ARDUINO_PIN_38	0x0A,	0x0B,	0x09,	7,	-1	//D38
#define MHV_ARDUINO_PIN_39	0x13,	0x14,	0x12,	2,	-1	//D39
#define MHV_ARDUINO_PIN_40	0x13,	0x14,	0x12,	1,	-1	//D40
#define MHV_ARDUINO_PIN_41	0x13,	0x14,	0x12,	0,	-1	//D41
#define MHV_ARDUINO_PIN_42	0x10A,	0x10B,	0x109,	7,	-1	//D42
#define MHV_ARDUINO_PIN_43	0x10A,	0x10B,	0x109,	6,	-1	//D43
#define MHV_ARDUINO_PIN_44	0x10A,	0x10B,	0x109,	5,	-1	//D44
#define MHV_ARDUINO_PIN_45	0x10A,	0x10B,	0x109,	4,	-1	//D45
#define MHV_ARDUINO_PIN_46	0x10A,	0x10B,	0x109,	3,	-1	//D46
#define MHV_ARDUINO_PIN_47	0x10A,	0x10B,	0x109,	2,	-1	//D47
#define MHV_ARDUINO_PIN_48	0x10A,	0x10B,	0x109,	1,	-1	//D48
#define MHV_ARDUINO_PIN_49	0x10A,	0x10B,	0x109,	0,	-1	//D49
#define MHV_ARDUINO_PIN_50	0x04,	0x05,	0x03,	3,	3	//SPI_MISO
#define MHV_ARDUINO_PIN_51	0x04,	0x05,	0x03,	2,	2	//SPI_MOSI
#define MHV_ARDUINO_PIN_52	0x04,	0x05,	0x03,	1,	1	//SPI_SCK
#define MHV_ARDUINO_PIN_53	0x04,	0x05,	0x03,	0,	0	//SPI_SS
#define MHV_ARDUINO_PIN_A0	0x10,	0x11,	0x0F,	0,	-1	//A0
#define MHV_ARDUINO_PIN_A1	0x10,	0x11,	0x0F,	1,	-1	//A1
#define MHV_ARDUINO_PIN_A2	0x10,	0x11,	0x0F,	2,	-1	//A2
#define MHV_ARDUINO_PIN_A3	0x10,	0x11,	0x0F,	3,	-1	//A3
#define MHV_ARDUINO_PIN_A4	0x10,	0x11,	0x0F,	4,	-1	//A4
#define MHV_ARDUINO_PIN_A5	0x10,	0x11,	0x0F,	5,	-1	//A5
#define MHV_ARDUINO_PIN_A6	0x10,	0x11,	0x0F,	6,	-1	//A6
#define MHV_ARDUINO_PIN_A7	0x10,	0x11,	0x0F,	7,	-1	//A7
#define MHV_ARDUINO_PIN_A8	0x107,	0x108,	0x106,	0,	16	//A8
#define MHV_ARDUINO_PIN_A9	0x107,	0x108,	0x106,	1,	17	//A9
#define MHV_ARDUINO_PIN_A10	0x107,	0x108,	0x106,	2,	18	//A10
#define MHV_ARDUINO_PIN_A11	0x107,	0x108,	0x106,	3,	19	//A11
#define MHV_ARDUINO_PIN_A12	0x107,	0x108,	0x106,	4,	20	//A12
#define MHV_ARDUINO_PIN_A13	0x107,	0x108,	0x106,	5,	21	//A13
#define MHV_ARDUINO_PIN_A14	0x107,	0x108,	0x106,	6,	22	//A14
#define MHV_ARDUINO_PIN_A15	0x107,	0x108,	0x106,	7,	23	//A15


#endif /* MHV_IO_ARDUINO_MEGA_H_ */
