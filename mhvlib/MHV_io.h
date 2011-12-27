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


#ifdef MHVLIB_NEED_PURE_VIRTUAL
#ifndef MHVLIB_PURE_VIRTUAL_DECLARED
#ifndef MHVLIB_CORE
#define MHVLIB_PURE_VIRTUAL_DECLARED
#include <avr/interrupt.h>
extern "C" void __cxa_pure_virtual() {
	cli();
	for (;;);
}
#endif
#endif
#endif


#ifndef MHV_IO_H_
#define MHV_IO_H_

#include <avr/io.h>
#include <inttypes.h>
#include <stddef.h>
#include <util/atomic.h>

// Some useful attributes

// A function that does not return
#define NORETURN __attribute__ ((noreturn))

// Code here is unreachable
#define UNREACHABLE __builtin_unreachable();

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

struct mhv_pin {
	volatile uint8_t	*dir;
	volatile uint8_t	*input;
	volatile uint8_t	*output;
	// Contains a mask of the pin
	uint8_t				bit;
	int8_t				pcInt;
};
typedef struct mhv_pin MHV_PIN;

/**
 * Convert a literal port and pin into a pin macro
 * @param	_mhv_port	the port (eg, B)
 * @param	_mhv_bit	the bit (eg, 3)
 */
#define MHV_MAKE_PIN(_mhv_port, _mhv_bit) \
	_MHV_MAKE_PIN(_mhv_port, _mhv_bit)

#define _MHV_MAKE_PIN(_mhv_port, _mhv_bit) \
	MHV_PIN_ ## _mhv_port ## _mhv_bit

/**
 * Get the list of parms for a pin declaration
 * @param _mhvPrefix	the prefix to use for the variable names
 */
#define MHV_DECLARE_PIN(_mhvPrefix) \
	volatile uint8_t *_mhvPrefix ## Dir, volatile uint8_t *_mhvPrefix ## Out, \
	volatile uint8_t *_mhvPrefix ## In, uint8_t _mhvPrefix ## Pin, \
	int8_t _mhvPrefix ## PinchangeInterrupt

/**
 * Get the parameter list for a pin
 * @param _mhvPrefix	the prefix to use for the variable names
 */
#define MHV_PIN_PARMS(_mhvPrefix) \
	_mhvPrefix ## Dir, _mhvPrefix ## Out, _mhvPrefix ## In, _mhvPrefix ## Pin, _mhvPrefix ## PinchangeInterrupt

/**
 *  Convert a pin declaration to a pin struct
 * @param mhvParms	a MHV_PIN_* macro
 */
#define mhv_pin(mhvParms) \
	_mhv_pin(mhvParms)

#define _mhv_pin(mhvDir,mhvOutput,mhvInput,mhvBit,mhvPCInt) \
		{mhvDir, mhvOutput, mhvInput, _BV(mhvBit), mhvPCInt}

/**
 * Set an output pin on
 * @param	pin		the pin to turn on
 */
inline void mhv_pinOn(MHV_PIN *pin) {
	*(pin->output) |= pin->bit;
}

/**
 * Set an output pin on atomically (used if the state of a pin on the same port is altered in an interrupt handler)
 * @param	pin		the pin to turn on
 */
inline void mhv_pinOnAtomic(MHV_PIN *pin) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		mhv_pinOn(pin);
	}
}


/**
 * Set an output pin on
 * @param	pin		an MHV_PIN_* macro
 */
inline void mhv_pinOn(MHV_DECLARE_PIN(pin)) {
	*pinOut |= _BV(pinPin);
}

/**
 * Set an output pin on (used if the state of a pin on the same port is altered in an interrupt handler)
 * @param	pin		an MHV_PIN_* macro
 */
inline void mhv_pinOnAtomic(MHV_DECLARE_PIN(pin)) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		mhv_pinOn(MHV_PIN_PARMS(pin));
	}
}


/**
 * Set an output pin off
 * @param	pin		the pin to turn off
 */
inline void mhv_pinOff(MHV_PIN *pin) {
	*(pin->output) &= ~(pin->bit);
}

/**
 * Set an output pin off  (used if the state of a pin on the same port is altered in an interrupt handler)
 * @param	pin		the pin to turn off
 */
inline void mhv_pinOffAtomic(MHV_PIN *pin) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		mhv_pinOff(pin);
	}
}


/**
 * Set an output pin off
 * @param	pin		an MHV_PIN_* macro
 */
inline void mhv_pinOff(MHV_DECLARE_PIN(pin)) {
	*pinOut &= ~_BV(pinPin);
}

/**
 * Set an output pin off (used if the state of a pin on the same port is altered in an interrupt handler)
 * @param	pin		an MHV_PIN_* macro
 */
inline void mhv_pinOffAtomic(MHV_DECLARE_PIN(pin)) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		mhv_pinOff(MHV_PIN_PARMS(pin));
	}
}


/**
 * Set an output pin on or off (state should really be constant for optimal performance)
 * @param	pin		an MHV_PIN_* macro
 * @param	state	true to turn the pin on
 */
inline void mhv_pinSet(MHV_DECLARE_PIN(pin), bool state) {
	if (state) {
		mhv_pinOn(pinDir, pinOut, pinIn, pinPin, pinPinchangeInterrupt);
	} else {
		mhv_pinOff(pinDir, pinOut, pinIn, pinPin, pinPinchangeInterrupt);
	}
}

/**
 * Set an output pin on or off (state should really be constant for optimal performance)
 * 	 (used if the state of a pin on the same port is altered in an interrupt handler)
 * @param	pin		an MHV_PIN_* macro
 * @param	state	true to turn the pin on
 */
inline void mhv_pinSetAtomic(MHV_DECLARE_PIN(pin), bool state) {
	if (state) {
		mhv_pinOnAtomic(pinDir, pinOut, pinIn, pinPin, pinPinchangeInterrupt);
	} else {
		mhv_pinOffAtomic(pinDir, pinOut, pinIn, pinPin, pinPinchangeInterrupt);
	}
}


/**
 * Set a pin to be an output
 * @param	pin		the pin to become an output
 */
inline void mhv_setOutput(MHV_PIN *pin) {
	*(pin->dir) |= pin->bit;
}

/**
 * Set a pin to be an output (used if the direction of a pin on the same port is altered in an interrupt handler)
 * @param	pin		the pin to become an output
 */
inline void mhv_setOutputAtomic(MHV_PIN *pin) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		mhv_setOutput(pin);
	}
}


/**
 * Set a pin to be an output
 * @param	pin		an MHV_PIN_* macro
 */
inline void mhv_setOutput(MHV_DECLARE_PIN(pin)) {
	*pinDir |= _BV(pinPin);
}

/**
 * Set a pin to be an output (used if the direction of a pin on the same port is altered in an interrupt handler)
  * @param	pin		an MHV_PIN_* macro
*/
inline void mhv_setOutputAtomic(MHV_DECLARE_PIN(pin)) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		mhv_setOutput(MHV_PIN_PARMS(pin));
	}
}

/**
 * Set a pin to be an input
 * @param	pin		the pin to become an output
 */
inline void mhv_setInput(MHV_PIN *pin) {
	*(pin->dir) &= ~(pin->bit);
	*(pin->output) &= ~(pin->bit);
}

/**
 * Set a pin to be an input (used if the direction of a pin on the same port is altered in an interrupt handler)
 * @param	pin		the pin to become an output
 */
inline void mhv_setInputAtomic(MHV_PIN *pin) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		mhv_setInput(pin);
	}
}

/**
 * Set a pin to be an input
 * @param	pin		an MHV_PIN_* macro
 */
inline void mhv_setInput(MHV_DECLARE_PIN(pin)) {
	*pinDir &= ~_BV(pinPin);
	*pinOut &= ~_BV(pinPin);
}

/**
 * Set a pin to be an input (used if the direction of a pin on the same port is altered in an interrupt handler)
 * @param	pin		an MHV_PIN_* macro
 */
inline void mhv_setInputAtomic(MHV_DECLARE_PIN(pin)) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		mhv_setInput(MHV_PIN_PARMS(pin));
	}
}


/**
 * Set a pin to be an input, with the internal pullup enabled
 * @param	pin		the pin to become an output
 */
inline void mhv_setInputPullup(MHV_PIN *pin) {
	*(pin->dir) &= ~(pin->bit);
	*(pin->output) |= pin->bit;
}

/**
 * Set a pin to be an input, with the internal pullup enabled (used if the direction of a pin on the same port is altered in an interrupt handler)
 * @param	pin		the pin to become an output
 */
inline void mhv_setInputPullupAtomic(MHV_PIN *pin) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		mhv_setInputPullup(pin);
	}
}


/**
 * Set a pin to be an input, with the internal pullup enabled
 * @param	pin		an MHV_PIN_* macro
 */
inline void mhv_setInputPullup(MHV_DECLARE_PIN(pin)) {
	*pinDir &= ~_BV(pinPin);
	*pinOut |= _BV(pinPin);
}

/**
 * Set a pin to be an input, with the internal pullup enabled (used if the direction of a pin on the same port is altered in an interrupt handler)
 * @param	pin		an MHV_PIN_* macro
 */
inline void mhv_setInputPullupAtomic(MHV_DECLARE_PIN(pin)) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		mhv_setInputPullup(MHV_PIN_PARMS(pin));
	}
}


/**
 * Toggle a pin
 * @param	pin		the pin to toggle
 */
inline void mhv_pinToggle(MHV_PIN *pin) {
	*(pin->input) |= pin->bit;
}

/**
 * Toggle a pin (used if the state of a pin on the same port is altered in an interrupt handler)
 * @param	pin		the pin to toggle
 */
inline void mhv_pinToggleAtomic(MHV_PIN *pin) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		*(pin->input) |= pin->bit;
	}
}


/**
 * Toggle a pin
 * @param	pin		an MHV_PIN_* macro
 */
inline void mhv_pinToggle(MHV_DECLARE_PIN(pin)) {
	*pinIn |= _BV(pinPin);
}

/**
 * Toggle a pin (used if the direction of a pin on the same port is altered in an interrupt handler)
 * @param	pin		an MHV_PIN_* macro
 */
inline void mhv_pinToggleAtomic(MHV_DECLARE_PIN(pin)) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		*pinIn |= _BV(pinPin);
	}
}


/**
 * Read a pin
 * @param	pin		the pin to read
 */
inline bool mhv_pinRead(MHV_PIN *pin) {
	return *(pin->input) & pin->bit;
}


/**
 * Read a pin
 * @param	pin		an MHV_PIN_* macro
 */
inline bool mhv_pinRead(MHV_DECLARE_PIN(pin)) {
	return *pinIn & _BV(pinPin);
}


/**
 * Get the size of an item as a byte
 * @param _mhvItem	the item to get the size of
 * @return the size of the item
 */
#define MHV_BYTESIZEOF(_mhvItem) (uint8_t)(sizeof(_mhvItem))

/**
 * Cheap memset to 0
 * @param	bufIn	a pointer to the buffer
 * @param	len		the length of an element in the buffer
 * @param	count	the number of elements in the buffer
 */
inline void mhv_memClear(void *bufIn, uint8_t len, uint8_t count) {
	char *buf = (char *)bufIn;

	for (uint8_t i = 0; i < count; i++) {
		for (uint8_t j = 0; j < len; j++, buf++) {
			*buf = 0;
		}
	}
}

/**
 * Cheap memset to 0
 * @param	bufIn	a pointer to the buffer
 * @param	len		the length of the buffer
 */
inline void mhv_memClear(void *bufIn, uint8_t len) {
	char *buf = (char *)bufIn;

	for (uint8_t i = 0; i < len; i++, buf++) {
		*buf = 0;
	}
}


/**
 * Cheap memset to 0
 * @param	bufIn	a pointer to the buffer
 * @param	len		the length of an element in the buffer
 * @param	count	the number of elements in the buffer
 */
inline void mhv_memClear(void *bufIn, uint8_t len, uint16_t count) {
	char *buf = (char *)bufIn;

	for (uint8_t i = 0; i < count; i++) {
		for (uint8_t j = 0; j < len; j++, buf++) {
			*buf = 0;
		}
	}
}

/**
 * Cheap memset to 0
 * @param	bufIn	a pointer to the buffer
 * @param	len		the length of the buffer
 */
inline void mhv_memClear(void *bufIn, uint16_t len) {
	char *buf = (char *)bufIn;

	for (uint8_t i = 0; i < len; i++, buf++) {
		*buf = 0;
	}
}


/**
 * Cheap memcpy
 * @param	bufOut	a pointer to the destination buffer
 * @param	bufIn	a pointer to the source buffer
 * @param	len		the length of an element in the buffer
 * @param	count	the number of elements in the buffer
 */
inline void mhv_memcpy(void *bufOut, void *bufIn, uint8_t len, uint8_t count) {
	char *src = (char *)bufIn;
	char *dst = (char *)bufOut;

	for (uint8_t i = 0; i < count; i++) {
		for (uint8_t j = 0; j < len; j++, src++, dst++) {
			*dst = *src;
		}
	}
}

/**
 * Cheap memcpy
 * @param	bufOut	a pointer to the destination buffer
 * @param	bufIn	a pointer to the source buffer
 * @param	len		the length of the buffer
 */
inline void mhv_memcpy(void *bufOut, void *bufIn, uint8_t len) {
	char *src = (char *)bufIn;
	char *dst = (char *)bufOut;

	for (uint8_t i = 0; i < len; i++,  src++, dst++) {
		*dst = *src;
	}
}


/**
 * Cheap memcpy
 * @param	bufOut	a pointer to the destination buffer
 * @param	bufIn	a pointer to the source buffer
 * @param	len		the length of an element in the buffer
 * @param	count	the number of elements in the buffer
 */
inline void mhv_memcpy(void *bufOut, void *bufIn, uint8_t len, uint16_t count) {
	char *src = (char *)bufIn;
	char *dst = (char *)bufOut;

	for (uint8_t i = 0; i < count; i++) {
		for (uint8_t j = 0; j < len; j++,  src++, dst++) {
			*dst = *src;
		}
	}
}

/**
 * Cheap memcpy
 * @param	bufOut	a pointer to the destination buffer
 * @param	bufIn	a pointer to the source buffer
 * @param	len		the length of the buffer
 */
inline void mhv_memcpy(void *bufOut, void *bufIn, uint16_t len) {
	char *src = (char *)bufIn;
	char *dst = (char *)bufOut;

	for (uint8_t i = 0; i < len; i++,  src++, dst++) {
		*dst = *src;
	}
}

/**
 * Cheap memcpy, starting from the tail
 * @param	bufOut	a pointer to the destination buffer
 * @param	bufIn	a pointer to the source buffer
 * @param	len		the length of an element in the buffer
 * @param	count	the number of elements in the buffer
 */
inline void mhv_memcpyTailFirst(void *bufOut, void *bufIn, uint8_t len, uint8_t count) {
	size_t offset = (size_t)len * count;
	char *src = (char *)bufIn + offset;
	char *dst = (char *)bufOut + offset;

	for (uint8_t i = 0; i < count; i++) {
		for (uint8_t j = 0; j < len; j++, --src, --dst) {
			*dst = *src;
		}
	}
}

/**
 * Cheap memcpy, starting from the tail
 * @param	bufOut	a pointer to the destination buffer
 * @param	bufIn	a pointer to the source buffer
 * @param	len		the length of the buffer
 */
inline void mhv_memcpyTailFirst(void *bufOut, void *bufIn, uint8_t len) {
	char *src = (char *)bufIn + len;
	char *dst = (char *)bufOut + len;

	for (uint8_t i = 0; i < len; i++,  --src, --dst) {
		*dst = *src;
	}
}


/**
 * Cheap memcpy, starting from the tail
 * @param	bufOut	a pointer to the destination buffer
 * @param	bufIn	a pointer to the source buffer
 * @param	len		the length of an element in the buffer
 * @param	count	the number of elements in the buffer
 */
inline void mhv_memcpyTailFirst(void *bufOut, void *bufIn, uint8_t len, uint16_t count) {
	size_t offset = (size_t)len * count;
	char *src = (char *)bufIn + offset;
	char *dst = (char *)bufOut + offset;

	for (uint8_t i = 0; i < count; i++) {
		for (uint8_t j = 0; j < len; j++,  --src, --dst) {
			*dst = *src;
		}
	}
}

/**
 * Cheap memcpy, starting from the tail
 * @param	bufOut	a pointer to the destination buffer
 * @param	bufIn	a pointer to the source buffer
 * @param	len		the length of the buffer
 */
inline void mhv_memcpyTailFirst(void *bufOut, void *bufIn, uint16_t len) {
	char *src = (char *)bufIn + len;
	char *dst = (char *)bufOut + len;

	for (uint8_t i = 0; i < len; i++,  --src, --dst) {
		*dst = *src;
	}
}



/* We have to shadow all the macros below as the precedence of macro expansion means that
 * the multi-parmeter macros will only see a single argument if one of our MHV_PIN macros
 * is used
 */

/* See http://gcc.gnu.org/onlinedocs/gcc-3.3.6/cpp/Swallowing-the-Semicolon.html
 * to understand why we have do...while(0) on our macros
 */

/**
 * Grab the output register of a pin declaration
 * @param	mhvParms	a MHV_PIN_* macro
 */
#define mhv_out(mhvParms) \
	_mhv_out(mhvParms)

#define _mhv_out(mhvDir,mhvOutput,mhvInput,mhvBit,mhvPCInt) \
	mhvOutput

/**
 * Grab the input register of a pin declaration
 * @param	mhvParms	a MHV_PIN_* macro
 */
#define mhv_in(mhvParms) \
	_mhv_in(mhvParms)

#define _mhv_in(mhvDir,mhvOutput,mhvInput,mhvBit,mhvPCInt) \
	mhvInput

/**
 * Grab the bit offset of a pin declaration
 * @param	mhvParms	A MHV_PIN_* Macro
 */
#define mhv_bit(mhvParms) \
	_mhv_bit(mhvParms)

#define _mhv_bit(mhvDir,mhvOutput,mhvInput,mhvBit,mhvPCInt) \
	mhvBit

/**
 * Grab the direction register of a pin declaration
 * @param	mhvParms	A MHV_PIN_* Macro
 */
#define mhv_dir(mhvParms) \
	_mhv_dir(mhvParms)

#define _mhv_dir(mhvDir,mhvOutput,mhvInput,mhvBit,mhvPCInt) \
	mhvDir

/**
 * Grab the pin change interrupt of a pin
 * @param	mhvParms	A MHV_PIN_* Macro
 */
#define mhv_pcint(mhvParms) \
	_mhv_pcint(mhvParms)

#define _mhv_PCInt(mhvDir,mhvOutput,mhvInput,mhvBit,mhvPCInt) \
	mhvPCInt


/**
 * Assign a function to be triggered by an external interrupt
 * @param	mhvInterruptParms	A MHV_INTERRUPT_* Macro
 * @param	mhvFunction			a block to execute when the interrupt occurs
 */
#define mhv_declareExternalInterrupt(mhvInterruptParms,mhvFunction) \
	_mhv_declareExternalInterrupt(mhvInterruptParms, mhvFunction)

#define _mhv_declareExternalInterrupt(mhvInterruptHandler,mhvModeRegister,mhvModeBitshift,mhvFunction) \
ISR(mhvInterruptHandler) mhvFunction

/**
 * Situations that interrupts can be triggered on
 */
enum mhv_interruptMode {
	MHV_INTERRUPT_LOW,    //!< MHV_INTERRUPT_LOW to level trigger when low
	MHV_INTERRUPT_CHANGE, //!< MHV_INTERRUPT_CHANGE to edge trigger on change
	MHV_INTERRUPT_FALLING,//!< MHV_INTERRUPT_FALLING to edge trigger when falling
	MHV_INTERRUPT_RISING  //!< MHV_INTERRUPT_RISING to edge trigger when rising
};
typedef enum mhv_interruptMode MHV_INTERRUPTMODE;


/**
 * Enable an external interrupt
 * @param	mhvInterruptParms	A MHV_INTERRUPT_* Macro
 * @param	mhvInterruptMode	When to raise the interrupt (see MHV_INTERRUPTMODE)
 */
#define mhv_enableExternalInterrupt(mhvInterruptParms,mhvInterruptMode) \
	do { \
		_mhv_enableExternalInterrupt(mhvInterruptParms,mhvInterruptMode); \
	} while (0)

#define _mhv_enableExternalInterrupt(mhvInterruptHandler,mhvModeRegister,mhvModeBitshift,mhvInterruptMode) \
	*mhvModeRegister = (*mhvModeRegister & ~(0x03 << mhvModeBitshift)) | (mhvInterruptMode << mhvModeBitshift)

#endif /* MHV_IO_H_ */
