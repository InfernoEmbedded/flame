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
#define MHV_ARDUINO_PIN_0	0x0A,	0x0B,	0x09,	0,	16
#define MHV_ARDUINO_PIN_1	0x0A,	0x0B,	0x09,	1,	17
#define MHV_ARDUINO_PIN_2	0x0A,	0x0B,	0x09,	2,	18
#define MHV_ARDUINO_PIN_3	0x0A,	0x0B,	0x09,	3,	19
#define MHV_ARDUINO_PIN_4	0x0A,	0x0B,	0x09,	4,	20
#define MHV_ARDUINO_PIN_5	0x0A,	0x0B,	0x09,	5,	21
#define MHV_ARDUINO_PIN_6	0x0A,	0x0B,	0x09,	6,	22
#define MHV_ARDUINO_PIN_7	0x0A,	0x0B,	0x09,	7,	23
#define MHV_ARDUINO_PIN_8	0x04,	0x05,	0x03,	0,	0
#define MHV_ARDUINO_PIN_9	0x04,	0x05,	0x03,	1,	1
#define MHV_ARDUINO_PIN_10	0x04,	0x05,	0x03,	2,	2
#define MHV_ARDUINO_PIN_11	0x04,	0x05,	0x03,	3,	3
#define MHV_ARDUINO_PIN_12	0x04,	0x05,	0x03,	4,	4
#define MHV_ARDUINO_PIN_13	0x04,	0x05,	0x03,	5,	5
#define MHV_ARDUINO_PIN_A0	0x07,	0x08,	0x06,	0,	8
#define MHV_ARDUINO_PIN_A1	0x07,	0x08,	0x06,	1,	9
#define MHV_ARDUINO_PIN_A2	0x07,	0x08,	0x06,	2,	10
#define MHV_ARDUINO_PIN_A3	0x07,	0x08,	0x06,	3,	11
#define MHV_ARDUINO_PIN_A4	0x07,	0x08,	0x06,	4,	12
#define MHV_ARDUINO_PIN_A5	0x07,	0x08,	0x06,	5,	13

#endif /* MHV_IO_ARDUINO_DIECIMILLA_H_ */
