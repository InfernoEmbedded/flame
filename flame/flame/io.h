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

#ifndef FLAME_IO_H_
#define FLAME_IO_H_

#include <avr/io.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>
#include <util/atomic.h>

#include <chips/whichchip.h>

// Some useful attributes

// A function that does not return
#define NORETURN __attribute__ ((noreturn))

// Code here is unreachable
#define UNREACHABLE __builtin_unreachable();

/* A function that has no effect other than its return value, and the return value depends
 * only on the parameters or global variables
 */
#define PURE __attribute__ ((pure))

/* A function that has no effect other than its return value, and the return value depends
 * only on the parameters
 */
#define CONST __attribute__ ((const))

// A function that should always be inlined
#define INLINE inline __attribute__((__always_inline__))

// the main declaration
#define MAIN int __attribute__ ((OS_main)) main()

/* A parameter we know is unused, used to suppress warnings for parameters required to implement an
 * API, but net necessary for a specific implementation
 */
#define UNUSED __attribute__ ((unused))

#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

namespace flame {

union Int3Axis {
	struct {
		int16_t		x;
		int16_t		y;
		int16_t		z;
	} axis;
	int16_t			value[3];
};

union Float3Axis {
	struct {
		float		x;
		float		y;
		float		z;
	} axis;
	float			value[3];
};

typedef uint16_t FLAME_register;

struct FLAME_pin {
	FLAME_register		dir;
	FLAME_register		input;
	FLAME_register		output;
	// Contains a mask of the pin
	uint8_t				bit;
	int8_t				pcInt;
};
typedef struct FLAME_pin FLAME_PIN;

/**
 * Convert a literal port and pin into a pin macro
 * @param	_FLAME_port	the port (eg, B)
 * @param	_FLAME_bit	the bit (eg, 3)
 */
#define FLAME_MAKE_PIN(_FLAME_port, _FLAME_bit) \
	_FLAME_MAKE_PIN(_FLAME_port, _FLAME_bit)

#define _FLAME_MAKE_PIN(_FLAME_port, _FLAME_bit) \
	FLAME_PIN_ ## _FLAME_port ## _FLAME_bit


/**
 * Get the list of parms for a pin declaration
 * @param _flamePrefix	the prefix to use for the variable names
 */
#define FLAME_DECLARE_PIN(_flamePrefix) \
	FLAME_register _flamePrefix ## Dir, FLAME_register _flamePrefix ## Out, \
	FLAME_register _flamePrefix ## In, uint8_t _flamePrefix ## Pin, \
	int8_t _flamePrefix ## PinchangeInterrupt

/**
 * Get the list of parms for a USART
 * @param _flamePrefix	the prefix to use for the variable names
 */
#define FLAME_DECLARE_USART(_flamePrefix) \
		FLAME_register _flamePrefix ## Baud, FLAME_register _flamePrefix ## Status, \
		FLAME_register _flamePrefix ## ControlB, FLAME_register _flamePrefix ## ControlC, \
		FLAME_register _flamePrefix ## IO, \
		FLAME_register _flamePrefix ## RxEnable, FLAME_register _flamePrefix ## TxEnable, \
		FLAME_register _flamePrefix ## RxInterruptEnable, FLAME_register _flamePrefix ## TxInterruptEnable, \
		FLAME_register _flamePrefix ## DataEmpty, FLAME_register _flamePrefix ## U2X

/**
 * Get the parameter list for a pin
 * @param _flamePrefix	the prefix to use for the variable names
 */
#define FLAME_PIN_PARMS(_flamePrefix) \
	_flamePrefix ## Dir, _flamePrefix ## Out, _flamePrefix ## In, _flamePrefix ## Pin, _flamePrefix ## PinchangeInterrupt

/**
 * Convert a pin declaration to a pin struct
 * @param flameParms	a FLAME_PIN_* macro
 */
#define FLAME_pin(flameParms) \
	_FLAME_pin(flameParms)

#define _FLAME_pin(flameDir,flameOutput,flameInput,flameBit,flamePCInt) \
		{flameDir, flameOutput, flameInput, _BV(flameBit), flamePCInt}

#pragma GCC diagnostic ignored "-Wunused-parameter"

/**
 * Set an output pin on
 * @param	pin		the pin to turn on
 */
INLINE void pinOn(FLAME_PIN *pin) {
	_MMIO_BYTE(pin->output) |= pin->bit;
}

/**
 * Set an output pin on atomically (used if the state of a pin on the same port is altered in an interrupt handler)
 * @param	pin		the pin to turn on
 */
INLINE void pinOnAtomic(FLAME_PIN *pin) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		pinOn(pin);
	}
}


/**
 * Set an output pin on
 * @param	pin		an FLAME_PIN_* macro
 */
INLINE void pinOn(FLAME_DECLARE_PIN(pin)) {
	_MMIO_BYTE(pinOut) |= _BV(pinPin);
}


/**
 * Set an output pin on (used if the state of a pin on the same port is altered in an interrupt handler)
 * @param	pin		an FLAME_PIN_* macro
 */
INLINE void pinOnAtomic(FLAME_DECLARE_PIN(pin)) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		pinOn(FLAME_PIN_PARMS(pin));
	}
}

/**
 * Set an output pin off
 * @param	pin		the pin to turn off
 */
INLINE void pinOff(FLAME_PIN *pin) {
	_MMIO_BYTE(pin->output) &= ~(pin->bit);
}

/**
 * Set an output pin off  (used if the state of a pin on the same port is altered in an interrupt handler)
 * @param	pin		the pin to turn off
 */
INLINE void pinOffAtomic(FLAME_PIN *pin) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		pinOff(pin);
	}
}


/**
 * Set an output pin off
 * @param	pin		an FLAME_PIN_* macro
 */
INLINE void pinOff(FLAME_DECLARE_PIN(pin)) {
	_MMIO_BYTE(pinOut) &= ~_BV(pinPin);
}

/**
 * Set an output pin off (used if the state of a pin on the same port is altered in an interrupt handler)
 * @param	pin		an FLAME_PIN_* macro
 */
INLINE void pinOffAtomic(FLAME_DECLARE_PIN(pin)) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		pinOff(FLAME_PIN_PARMS(pin));
	}
}

/**
 * Set an output pin on or off
 * @param	pin		an FLAME_PIN_* macro
 * @param	state	true to turn the pin on
 */
INLINE void pinSet(FLAME_DECLARE_PIN(pin), bool state) {
	_MMIO_BYTE(pinOut) = (_MMIO_BYTE(pinOut) & ~_BV(pinPin)) | (state << pinPin);
}

/**
 * Set an output pin on or off (state should really be constant for optimal performance)
 * 	 (used if the state of a pin on the same port is altered in an interrupt handler)
 * @param	pin		an FLAME_PIN_* macro
 * @param	state	true to turn the pin on
 */
INLINE void pinSetAtomic(FLAME_DECLARE_PIN(pin), bool state) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		pinSet(FLAME_PIN_PARMS(pin), state);
	}
}

/**
 * Set a pin to be an output
 * @param	pin		the pin to become an output
 */
INLINE void setOutput(FLAME_PIN *pin) {
	_MMIO_BYTE(pin->dir) |= pin->bit;
}

/**
 * Set a pin to be an output (used if the direction of a pin on the same port is altered in an interrupt handler)
 * @param	pin		the pin to become an output
 */
INLINE void setOutputAtomic(FLAME_PIN *pin) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		setOutput(pin);
	}
}

/**
 * Set a pin to be an output
 * @param	pin		an FLAME_PIN_* macro
 */
INLINE void setOutput(FLAME_DECLARE_PIN(pin)) {
	_MMIO_BYTE(pinDir) |= _BV(pinPin);
}

/**
 * Set a pin to be an output (used if the direction of a pin on the same port is altered in an interrupt handler)
 * @param	pin		an FLAME_PIN_* macro
*/
INLINE void setOutputAtomic(FLAME_DECLARE_PIN(pin)) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		setOutput(FLAME_PIN_PARMS(pin));
	}
}

/**
 * Set a pin to be an input
 * @param	pin		the pin to become an output
 */
INLINE void setInput(FLAME_PIN *pin) {
	_MMIO_BYTE(pin->dir) &= ~(pin->bit);
	_MMIO_BYTE(pin->output) &= ~(pin->bit);
}

/**
 * Set a pin to be an input (used if the direction of a pin on the same port is altered in an interrupt handler)
 * @param	pin		the pin to become an output
 */
INLINE void setInputAtomic(FLAME_PIN *pin) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		setInput(pin);
	}
}

/**
 * Set a pin to be an input
 * @param	pin		an FLAME_PIN_* macro
 */
INLINE void setInput(FLAME_DECLARE_PIN(pin)) {
	_MMIO_BYTE(pinDir) &= ~_BV(pinPin);
	_MMIO_BYTE(pinOut) &= ~_BV(pinPin);
}

/**
 * Set a pin to be an input (used if the direction of a pin on the same port is altered in an interrupt handler)
 * @param	pin		an FLAME_PIN_* macro
 */
INLINE void setInputAtomic(FLAME_DECLARE_PIN(pin)) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		setInput(FLAME_PIN_PARMS(pin));
	}
}

/**
 * Set a pin to be an input, with the internal pullup enabled
 * @param	pin		the pin to become an output
 */
INLINE void setInputPullup(FLAME_PIN *pin) {
	_MMIO_BYTE(pin->dir) &= ~(pin->bit);
	_MMIO_BYTE(pin->output) |= pin->bit;
}

/**
 * Set a pin to be an input, with the internal pullup enabled (used if the direction of a pin on the same port is altered in an interrupt handler)
 * @param	pin		the pin to become an output
 */
INLINE void setInputPullupAtomic(FLAME_PIN *pin) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		setInputPullup(pin);
	}
}



/**
 * Set a pin to be an input, with the internal pullup enabled
 * @param	pin		an FLAME_PIN_* macro
 */
INLINE void setInputPullup(FLAME_DECLARE_PIN(pin)) {
	_MMIO_BYTE(pinDir) &= ~_BV(pinPin);
	_MMIO_BYTE(pinOut) |= _BV(pinPin);
}

/**
 * Set a pin to be an input, with the internal pullup enabled (used if the direction of a pin on the same port is altered in an interrupt handler)
 * @param	pin		an FLAME_PIN_* macro
 */
INLINE void setInputPullupAtomic(FLAME_DECLARE_PIN(pin)) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		setInputPullup(FLAME_PIN_PARMS(pin));
	}
}

/**
 * Toggle a pin
 * @param	pin		the pin to toggle
 */
INLINE void pinToggle(FLAME_PIN *pin) {
	_MMIO_BYTE(pin->input) |= pin->bit;
}

/**
 * Toggle a pin (used if the state of a pin on the same port is altered in an interrupt handler)
 * @param	pin		the pin to toggle
 */
INLINE void pinToggleAtomic(FLAME_PIN *pin) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		_MMIO_BYTE(pin->input) |= pin->bit;
	}
}

/**
 * Toggle a pin
 * @param	pin		an FLAME_PIN_* macro
 */
INLINE void pinToggle(FLAME_DECLARE_PIN(pin)) {
	_MMIO_BYTE(pinIn) |= _BV(pinPin);
}

/**
 * Toggle a pin (used if the direction of a pin on the same port is altered in an interrupt handler)
 * @param	pin		an FLAME_PIN_* macro
 */
INLINE void pinToggleAtomic(FLAME_DECLARE_PIN(pin)) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		pinToggle(FLAME_PIN_PARMS(pin));
	}
}

/**
 * Read a pin
 * @param	pin		the pin to read
 */
INLINE bool pinRead(FLAME_PIN *pin) {
	return _MMIO_BYTE(pin->input) & pin->bit;
}


/**
 * Read a pin
 * @param	pin		an FLAME_PIN_* macro
 */
INLINE bool pinRead(FLAME_DECLARE_PIN(pin)) {
	return _MMIO_BYTE(pinIn) & _BV(pinPin);
}

#pragma GCC diagnostic warning "-Wunused-parameter"

/**
 * Get the size of an item as a byte
 * @param _flameItem	the item to get the size of
 * @return the size of the item
 */
#define FLAME_BYTESIZEOF(_flameItem) (uint8_t)(sizeof(_flameItem))

#include <avr/pgmspace.h>
/**
 * Compare a PROGMEM string to a region of memory
 * @param	source	the PROGMEM string
 * @param	target	the target to compare
 * @return false if the strings match
 */
INLINE PURE bool stringCompare_P(PGM_P source, char *target) {
	for (;; source++, target++) {
		char c = pgm_read_byte(source);
		if ('\0' == c  && '\0' == *target) {
				return false;
		}
		if (c != *target) {
			return true;
		}
	}

	UNREACHABLE;
	return true;
}

/**
 * Compare a PROGMEM buffer to a region of memory
 * @param	source		the PROGMEM buffer
 * @param	target	the target to compare
 * @param	length	the length of both buffer
 * @return false if the strings match
 */
INLINE bool memCompare_P(PGM_P source, char *target, uint8_t length) {
	for (;length > 0; source++, target++, length--) {
		char c = pgm_read_byte(source);
		if (c != *target) {
			return true;
		}
	}

	return false;
}


/* We have to shadow all the macros below as the precedence of macro expansion means that
 * the multi-parmeter macros will only see a single argument if one of our FLAME_PIN macros
 * is used
 */

/* See http://gcc.gnu.org/onlinedocs/gcc-3.3.6/cpp/Swallowing-the-Semicolon.html
 * to understand why we have do...while(0) on our macros
 */

/**
 * Grab the output register of a pin declaration
 * @param	flameParms	a FLAME_PIN_* macro
 */
#define FLAME_out(flameParms) \
	_FLAME_out(flameParms)

#define _FLAME_out(flameDir,flameOutput,flameInput,flameBit,flamePCInt) \
	flameOutput

/**
 * Grab the input register of a pin declaration
 * @param	flameParms	a FLAME_PIN_* macro
 */
#define FLAME_in(flameParms) \
	_FLAME_in(flameParms)

#define _FLAME_in(flameDir,flameOutput,flameInput,flameBit,flamePCInt) \
	flameInput

/**
 * Grab the bit offset of a pin declaration
 * @param	flameParms	A FLAME_PIN_* Macro
 */
#define FLAME_bit(flameParms) \
	_FLAME_bit(flameParms)

#define _FLAME_bit(flameDir,flameOutput,flameInput,flameBit,flamePCInt) \
	flameBit

/**
 * Grab the direction register of a pin declaration
 * @param	flameParms	A FLAME_PIN_* Macro
 */
#define FLAME_dir(flameParms) \
	_FLAME_dir(flameParms)

#define _FLAME_dir(flameDir,flameOutput,flameInput,flameBit,flamePCInt) \
	flameDir

/**
 * Grab the pin change interrupt of a pin
 * @param	flameParms	A FLAME_PIN_* Macro
 */
#define FLAME_pcint(flameParms) \
	_FLAME_pcint(flameParms)

#define _FLAME_PCInt(flameDir,flameOutput,flameInput,flameBit,flamePCInt) \
	flamePCInt


/**
 * Assign a function to be triggered by an external interrupt
 * @param	flameInterruptParms	A FLAME_INTERRUPT_* Macro
 * @param	flameFunction			a block to execute when the interrupt occurs
 */
#define FLAME_declareExternalInterrupt(flameInterruptParms,flameFunction) \
	_FLAME_declareExternalInterrupt(flameInterruptParms, flameFunction)

#define _FLAME_declareExternalInterrupt(flameInterruptHandler,flameModeRegister,flameModeBitshift,flameFunction) \
ISR(flameInterruptHandler) flameFunction

/**
 * Situations that interrupts can be triggered on
 */
enum class InterruptMode {
	LOW,    //!< FLAME_INTERRUPT_LOW to level trigger when low
	CHANGE, //!< FLAME_INTERRUPT_CHANGE to edge trigger on change
	FALLING,//!< FLAME_INTERRUPT_FALLING to edge trigger when falling
	RISING  //!< FLAME_INTERRUPT_RISING to edge trigger when rising
};

/**
 * Enable an external interrupt
 * @param	flameInterruptParms	A FLAME_INTERRUPT_* Macro
 * @param	flameInterruptMode	When to raise the interrupt (see FLAME_INTERRUPTMODE)
 */
#define FLAME_enableExternalInterrupt(flameInterruptParms,flameInterruptMode) \
	do { \
		_FLAME_enableExternalInterrupt(flameInterruptParms,flameInterruptMode); \
	} while (0)

#define _FLAME_enableExternalInterrupt(flameInterruptHandler,flameModeRegister,flameModeBitshift,flameInterruptMode) \
	*flameModeRegister = (*flameModeRegister & ~(0x03 << flameModeBitshift)) | (flameInterruptMode << flameModeBitshift)

}

#endif /* FLAME_IO_H_ */
