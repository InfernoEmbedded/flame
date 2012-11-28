/*
 * Copyright (c) 2012, Make, Hack, Void Inc
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


#ifndef MHV_IO_ARDUINO_H_
#define MHV_IO_ARDUINO_H_

#include <avr/io.h>

#ifdef INT0_vect
#define MHV_INTERRUPT_INT0 INT0_vect, &MCUCR, ISC00
#endif

#ifdef INT1_vect
#define MHV_INTERRUPT_INT1 INT1_vect, &MCUCR, ISC10
#endif

#if defined(__AVR_ATmega1280__)
#include <boards/ArduinoMega.h>
#elif defined(__AVR_ATmega168__)
#include <boards/ArduinoDiecimilla.h>
#elif defined(__AVR_ATmega168A__)
#include <boards/ArduinoDiecimilla.h>
#elif defined(__AVR_ATmega168P__)
#include <boards/ArduinoDiecimilla.h>
#elif defined(__AVR_ATmega2560__)
#include <boards/ArduinoMega.h>
#elif defined(__AVR_ATmega328__)
#include <boards/ArduinoDiecimilla.h>
#elif defined(__AVR_ATmega328P__)
#include <boards/ArduinoDiecimilla.h>
#else
#error unknown chip
#endif

#endif // MHV_IO_ARDUINO_H_

