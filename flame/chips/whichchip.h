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


#ifndef FLAME_IO_CHIP_H_
#define FLAME_IO_CHIP_H_

#include <avr/io.h>

#ifdef INT0_vect
#define FLAME_INTERRUPT_INT0 INT0_vect, &MCUCR, ISC00
#endif

#ifdef INT1_vect
#define FLAME_INTERRUPT_INT1 INT1_vect, &MCUCR, ISC10
#endif

#if defined(__AVR_ATmega1280__)
#include <chips/atmega1280.h>
#elif defined(__AVR_ATmega1281__)
#include <chips/atmega1281.h>
#elif defined(__AVR_ATmega168__)
#include <chips/atmega168.h>
#elif defined(__AVR_ATmega168A__)
#include <chips/atmega168a.h>
#elif defined(__AVR_ATmega168P__)
#include <chips/atmega168p.h>
#elif defined(__AVR_ATmega168PA__)
#include <chips/atmega168pa.h>
#elif defined(__AVR_ATmega2560__)
#include <chips/atmega2560.h>
#elif defined(__AVR_ATmega2561__)
#include <chips/atmega2561.h>
#elif defined(__AVR_ATmega328__)
#include <chips/atmega328.h>
#elif defined(__AVR_ATmega328P__)
#include <chips/atmega328p.h>
#elif defined(__AVR_ATmega48__)
#include <chips/atmega48.h>
#elif defined(__AVR_ATmega48A__)
#include <chips/atmega48a.h>
#elif defined(__AVR_ATmega48HVF__)
#include <chips/atmega48hvf.h>
#elif defined(__AVR_ATmega48P__)
#include <chips/atmega48p.h>
#elif defined(__AVR_ATmega48PA__)
#include <chips/atmega48pa.h>
#elif defined(__AVR_ATmega640__)
#include <chips/atmega640.h>
#elif defined(__AVR_ATmega88__)
#include <chips/atmega88.h>
#elif defined(__AVR_ATmega88A__)
#include <chips/atmega88a.h>
#elif defined(__AVR_ATmega88P__)
#include <chips/atmega88p.h>
#elif defined(__AVR_ATmega88PA__)
#include <chips/atmega88pa.h>
#elif defined(__AVR_ATtiny2313__)
#include <chips/attiny2313.h>
#elif defined(__AVR_ATtiny2313A__)
#include <chips/attiny2313a.h>
#elif defined(__AVR_ATtiny25__)
#include <chips/attiny25.h>
#elif defined(__AVR_ATtiny4313__)
#include <chips/attiny4313.h>
#elif defined(__AVR_ATtiny45__)
#include <chips/attiny45.h>
#elif defined(__AVR_ATtiny85__)
#include <chips/attiny85.h>
#else
#error unknown chip
#endif

#endif // FLAME_IO_CHIP_H_

