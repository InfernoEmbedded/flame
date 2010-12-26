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


#ifndef MHV_IO_H_
#define MHV_IO_H_

#include <avr/io.h>

// Some useful attributes

// A function that does not return
#define NORETURN __attribute__ ((noreturn))

// A function that has no effect other than its return value
#define PURE __attribute__ ((pure))


#if defined(__AVR_ATtiny2313__)
#include <MHV_io_ATtiny2313.h>
#elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
#include <MHV_io_ATtiny85.h>
#elif defined(__AVR_ATmega1280__)
#include <MHV_io_ATmega1280.h>
#include <MHV_io_ArduinoMega.h>
#elif defined(__AVR_ATmega48__) || defined(__AVR_ATmega48A__) || defined(__AVR_ATmega88__) || \
	defined(__AVR_ATmega88A__) || defined(__AVR_ATmega88PA__) || defined(__AVR_ATmega168__) || \
	defined(__AVR_ATmega168A__) || defined(__AVR_ATmega168PA__) || defined(__AVR_ATmega328__) || \
	defined(__AVR_ATmega328P__)
#include <MHV_io_ATmega168.h>
#include <MHV_io_ArduinoDiecimilla.h>
#endif

#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

#if GCC_VERSION < 40500
extern "C" void __cxa_pure_virtual() {
	cli();
	for (;;);
}
#endif


enum mhv_interruptMode {
	MHV_INTERRUPT_LOW,
	MHV_INTERRUPT_CHANGE,
	MHV_INTERRUPT_FALLING,
	MHV_INTERRUPT_RISING
};
typedef enum mhv_interruptMode MHV_INTERRUPTMODE;

struct mhv_pin {
	volatile uint8_t	*port;
	uint8_t				pin;
};
typedef struct mhv_pin MHV_PIN;

// Convert a pin declaration to a pin struct for output
#define mhv_pin_out(mhvParms) \
	_mhv_pin_out(mhvParms)

#define _mhv_pin_out(mhvDir,mhvOutput,mhvInput,mhvBit,mhvPCInt) \
		{mhvOutput, _BV(mhvBit)}

// Convert a pin declaration to a pin struct for input
#define mhv_pin_in(mhvParms) \
	_mhv_pin_in(mhvParms)

#define _mhv_pin_in(mhvDir,mhvOutput,mhvInput,mhvBit,mhvPCInt) \
		{mhvInput, _BV(mhvBit)}

// Set an output pin struct on
#define mhv_pin_struct_on(mhvPinStruct) \
		*((mhvPinStruct).port) |= (uint8_t)(mhvPinStruct).pin

// Set an output pin struct off
#define mhv_pin_struct_off(mhvPinStruct) \
		*((mhvPinStruct).port) &= ~(uint8_t)(mhvPinStruct).pin

// Read an input pin struct
#define mhv_pin_struct_read(mhvPinStruct) \
		*((mhvPinStruct).port) & (mhvPinStruct).pin


/* We have to shadow all the macros below as the precedence of macro expansion means that
 * the multi-parmeter macros will only see a single argument if one of our MHV_PIN macros
 * is used
 */

/* See http://gcc.gnu.org/onlinedocs/gcc-3.3.6/cpp/Swallowing-the-Semicolon.html
 * to understand why we have do...while(0) on our macros
 */

// Grab the output register of a pin declaration
#define mhv_out(mhvParms) \
	_mhv_out(mhvParms)

#define _mhv_out(mhvDir,mhvOutput,mhvInput,mhvBit,mhvPCInt) \
	mhvOutput

// Grab the input register of a pin declaration
#define mhv_in(mhvParms) \
	_mhv_in(mhvParms)

#define _mhv_in(mhvDir,mhvOutput,mhvInput,mhvBit,mhvPCInt) \
	mhvInput

// Grab the bit offset of a pin declaration
#define mhv_Bit(mhvParms) \
	_mhv_Bit(mhvParms)

#define _mhv_Bit(mhvDir,mhvOutput,mhvInput,mhvBit,mhvPCInt) \
	mhvBit

// Grab the direction register of a pin declaration
#define mhv_dir(mhvParms) \
	_mhv_dir(mhvParms)

#define _mhv_dir(mhvDir,mhvOutput,mhvInput,mhvBit,mhvPCInt) \
	mhvDir

// Grab the pin change interrupt of a pin
#define mhv_PCInt(mhvParms) \
	_mhv_PCInt(mhvParms)

#define _mhv_PCInt(mhvDir,mhvOutput,mhvInput,mhvBit,mhvPCInt) \
	mhvPCInt

// Set a pin to be an output
#define mhv_setOutput(mhvParms) \
	do { \
		_mhv_setOutput(mhvParms); \
	} while(0)

#define _mhv_setOutput(mhvDir,mhvOutput,mhvInput,mhvBit,mhvPCInt) \
	*mhvDir |= (uint8_t)_BV(mhvBit);

// Set a pin to be an input
#define mhv_setInput(mhvParms) \
	do { \
		_mhv_setInput(mhvParms); \
	} while(0)

#define _mhv_setInput(mhvDir,mhvOutput,mhvInput,mhvBit,mhvPCInt) \
	*mhvDir &= ~(uint8_t)_BV(mhvBit); \
	*mhvOutput &= ~(uint8_t)_BV(mhvBit);

// Set a pin to be an input, with the internal pullup enabled
#define mhv_setInputPullup(mhvParms) \
	do { \
		_mhv_setInputPullup(mhvParms) \
	} while(0)

#define _mhv_setInputPullup(mhvDir,mhvOutput,mhvInput,mhvBit,mhvPCInt) \
	*mhvDir &= ~(uint8_t)_BV(mhvBit); \
	*mhvOutput |= (uint8_t)_BV(mhvBit);

// turn a pin on
#define mhv_pinOn(mhvParms) \
	do { \
		_mhv_pinOn(mhvParms) \
	} while(0)

#define _mhv_pinOn(mhvDir,mhvOutput,mhvInput,mhvBit,mhvPCInt) \
		*mhvOutput |= (uint8_t)_BV(mhvBit);

// turn a pin off
#define mhv_pinOff(mhvParms) \
	do { \
		_mhv_pinOff(mhvParms) \
	} while(0)

#define _mhv_pinOff(mhvDir,mhvOutput,mhvInput,mhvBit,mhvPCInt) \
	*mhvOutput &= ~(uint8_t)_BV(mhvBit); \

// Toggle a pin
#define mhv_pinToggle(mhvParms) \
	do { \
		_mhv_pinToggle(mhvParms) \
	} while(0)

#define _mhv_pinToggle(mhvDir,mhvOutput,mhvInput,mhvBit,mhvPCInt) \
	*mhvInput |= (uint8_t)_BV(mhvBit); \

// Read a pin
#define mhv_pinRead(mhvParms) \
	_mhv_pinRead(mhvParms)

#define _mhv_pinRead(mhvDir,mhvOutput,mhvInput,mhvBit,mhvPCInt) \
	(*mhvInput & (uint8_t)_BV(mhvBit))

// Assign a function to be triggered by an external interrupt
#define mhv_declareExternalInterrupt(mhvInterruptParms,mhvFunction) \
	_mhv_declareExternalInterrupt(mhvInterruptParms, mhvFunction)

#define _mhv_declareExternalInterrupt(mhvInterruptHandler,mhvModeRegister,mhvModeBitshift,mhvFunction) \
ISR(mhvInterruptHandler) mhvFunction

// Enable an external interrupt
#define mhv_enableExternalInterrupt(mhvInterruptParms,mhvInterruptMode) \
	do { \
		_mhv_enableExternalInterrupt(mhvInterruptParms,mhvInterruptMode); \
	} while (0)

#define _mhv_enableExternalInterrupt(mhvInterruptHandler,mhvModeRegister,mhvModeBitshift,mhvInterruptMode) \
	*mhvModeRegister = (*mhvModeRegister & ~(0x03 << mhvModeBitshift)) | (mhvInterruptMode << mhvModeBitshift)

#endif /* MHV_IO_H_ */
