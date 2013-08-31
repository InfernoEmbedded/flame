/*
 * Copyright (c) 2013, Make, Hack, Void Inc
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


#ifndef MHV_IO_CHIP_H_
#define MHV_IO_CHIP_H_

#include <avr/io.h>

#ifdef INT0_vect
#define MHV_INTERRUPT_INT0 INT0_vect, &EICRA, ISC00, &EIMSK, INT0
#endif

#ifdef INT1_vect
#define MHV_INTERRUPT_INT1 INT1_vect, &EICRA, ISC10, &EIMSK, INT1
#endif

#if defined(__AVR_ATmega1280__)
#include <chips/MHV_io_atmega1280.h>
#elif defined(__AVR_ATmega1281__)
#include <chips/MHV_io_atmega1281.h>
#elif defined(__AVR_ATmega168__)
#include <chips/MHV_io_atmega168.h>
#elif defined(__AVR_ATmega168A__)
#include <chips/MHV_io_atmega168a.h>
#elif defined(__AVR_ATmega168P__)
#include <chips/MHV_io_atmega168p.h>
#elif defined(__AVR_ATmega2560__)
#include <chips/MHV_io_atmega2560.h>
#elif defined(__AVR_ATmega2561__)
#include <chips/MHV_io_atmega2561.h>
#elif defined(__AVR_ATmega328__)
#include <chips/MHV_io_atmega328.h>
#elif defined(__AVR_ATmega328P__)
#include <chips/MHV_io_atmega328p.h>
#elif defined(__AVR_ATmega48__)
#include <chips/MHV_io_atmega48.h>
#elif defined(__AVR_ATmega48A__)
#include <chips/MHV_io_atmega48a.h>
#elif defined(__AVR_ATmega48P__)
#include <chips/MHV_io_atmega48p.h>
#elif defined(__AVR_ATmega640__)
#include <chips/MHV_io_atmega640.h>
#elif defined(__AVR_ATmega88__)
#include <chips/MHV_io_atmega88.h>
#elif defined(__AVR_ATmega88A__)
#include <chips/MHV_io_atmega88a.h>
#elif defined(__AVR_ATmega88P__)
#include <chips/MHV_io_atmega88p.h>
#elif defined(__AVR_ATmega88PA__)
#include <chips/MHV_io_atmega88pa.h>
#elif defined(__AVR_ATtiny2313__)
#include <chips/MHV_io_attiny2313.h>
#elif defined(__AVR_ATtiny2313A__)
#include <chips/MHV_io_attiny2313a.h>
#elif defined(__AVR_ATtiny25__)
#include <chips/MHV_io_attiny25.h>
#elif defined(__AVR_ATtiny4313__)
#include <chips/MHV_io_attiny4313.h>
#elif defined(__AVR_ATtiny45__)
#include <chips/MHV_io_attiny45.h>
#elif defined(__AVR_ATtiny85__)
#include <chips/MHV_io_attiny85.h>
#else
#error unknown chip
#endif

#endif // MHV_IO_CHIP_H_

