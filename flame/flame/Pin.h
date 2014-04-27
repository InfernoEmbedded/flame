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

#ifndef FLAME_PIN_H_
#define FLAME_PIN_H_

#include <flame/io.h>

namespace flame {

class Pin {
public:
	/**
	 * Set the pin on
	 * @pre the pin must be set to output
	 */
	virtual void on() =0;

	/**
	 * Set the pin on atomically (used if the state of a pin on the same port is altered in an interrupt handler)
	 * @pre the pin must be set to output
	 */
	virtual void onAtomic() =0;

	/**
	 * Set the pin off
	 * @pre the pin must be set to output
	 */
	virtual void off() =0;

	/**
	 * Set the pin off atomically (used if the state of a pin on the same port is altered in an interrupt handler)
	 * @pre the pin must be set to output
	 */
	virtual void offAtomic() =0;

	/**
	 * Set the pin on or off
	 * @pre the pin must be set to output
	 * @param	state	true to turn the pin on
	 */
	virtual void set(bool state) =0;

	/**
	 * Set the output pin on or off
	 * 	 (used if the state of a pin on the same port is altered in an interrupt handler)
	 * @pre the pin must be set to output
	 * @param	state	true to turn the pin on
	 */
	virtual void setAtomic(bool state) =0;

	/**
	 * Set the pin to be an output
	 */
	virtual void setOutput() =0;

	/**
	 * Set the pin to be an output (used if the direction of a pin on the same port is altered in an interrupt handler)
	 */
	virtual void setOutputAtomic() =0;

	/**
	 * Set the pin to be an input
	 */
	virtual void setInput() =0;

	/**
	 * Set the pin to be an input (used if the direction of a pin on the same port is altered in an interrupt handler)
	 */
	virtual void setInputAtomic() =0;

	/**
	 * Set the pin to be an input, with the internal pullup enabled
	 */
	virtual void setInputPullup() =0;

	/**
	 * Set a pin to be an input, with the internal pullup enabled (used if the direction of a pin on the same port is altered in an interrupt handler)
	 */
	virtual void setInputPullupAtomic() =0;

	/**
	 * Toggle a pin
	 */
	virtual void toggle() =0;

	/**
	 * Toggle the pin (used if the state of a pin on the same port is altered in an interrupt handler)
	 * @pre the pin must be set to output
	 */
	virtual void toggleAtomic() =0;

	/**
	 * Read the pin
	 * @param	pin		the pin to read
	 */
	virtual bool read() =0;

	/**
	 * Get the pinchange interrupt for the pin
	 * @return the pinchange interrupt, or -1 if there is none available
	 */
	virtual uint8_t pinchangeInterrupt() =0;

	/**
	 * Get the direction port
	 */
	virtual FLAME_register dirPort() =0;

	/**
	 * Get the input port
	 */
	virtual FLAME_register inputPort() =0;

	/**
	 * Get the output port
	 */
	virtual FLAME_register outputPort() =0;

	/**
	 * Get the bit
	 */
	virtual uint8_t bit() =0;

	/**
	 * Get the mask
	 */
	virtual uint8_t mask() =0;
};

/**
 * Create a new Pin object that represents a GPIO pin
 * @tparam	dataPin...		the pin
 */
template<FLAME_DECLARE_PIN(pin)>
class PinImplementation : public Pin {
public:

/**
 * Set the pin on
 * @pre the pin must be set to output
 */
INLINE void on() {
	pinOn(FLAME_PIN_PARMS(pin));
}

/**
 * Set the pin on atomically (used if the state of a pin on the same port is altered in an interrupt handler)
 * @pre the pin must be set to output
 */
INLINE void onAtomic() {
	pinOnAtomic(FLAME_PIN_PARMS(pin));
}

/**
 * Set the pin off
 * @pre the pin must be set to output
 */
INLINE void off() {
	pinOff(FLAME_PIN_PARMS(pin));
}

/**
 * Set the pin off atomically (used if the state of a pin on the same port is altered in an interrupt handler)
 * @pre the pin must be set to output
 */
INLINE void offAtomic() {
	pinOffAtomic(FLAME_PIN_PARMS(pin));
}

/**
 * Set the pin on or off
 * @pre the pin must be set to output
 * @param	state	true to turn the pin on
 */
INLINE void set(bool state) {
	pinSet(FLAME_PIN_PARMS(pin), state);
}

/**
 * Set the output pin on or off
 * 	 (used if the state of a pin on the same port is altered in an interrupt handler)
 * @pre the pin must be set to output
 * @param	state	true to turn the pin on
 */
INLINE void setAtomic(bool state) {
	pinSetAtomic(FLAME_PIN_PARMS(pin), state);
}

/**
 * Set the pin to be an output
 */
INLINE void setOutput() {
	flame::setOutput(FLAME_PIN_PARMS(pin));
}

/**
 * Set the pin to be an output (used if the direction of a pin on the same port is altered in an interrupt handler)
 */
INLINE void setOutputAtomic() {
	flame::setOutputAtomic(FLAME_PIN_PARMS(pin));
}

/**
 * Set the pin to be an input
 */
INLINE void setInput() {
	flame::setInput(FLAME_PIN_PARMS(pin));
}

/**
 * Set the pin to be an input (used if the direction of a pin on the same port is altered in an interrupt handler)
 */
INLINE void setInputAtomic() {
	flame::setInputAtomic(FLAME_PIN_PARMS(pin));
}

/**
 * Set the pin to be an input, with the internal pullup enabled
 */
INLINE void setInputPullup() {
	flame::setInputPullup(FLAME_PIN_PARMS(pin));
}

/**
 * Set a pin to be an input, with the internal pullup enabled (used if the direction of a pin on the same port is altered in an interrupt handler)
 */
INLINE void setInputPullupAtomic() {
	flame::setInputPullupAtomic(FLAME_PIN_PARMS(pin));
}

/**
 * Toggle a pin
 */
INLINE void toggle() {
	pinToggle(FLAME_PIN_PARMS(pin));
}

/**
 * Toggle the pin (used if the state of a pin on the same port is altered in an interrupt handler)
 * @pre the pin must be set to output
 */
INLINE void toggleAtomic() {
	pinToggleAtomic(FLAME_PIN_PARMS(pin));
}

/**
 * Read the pin
 * @param	pin		the pin to read
 */
INLINE bool read() {
	return pinRead(FLAME_PIN_PARMS(pin));
}

/**
 * Get the pinchange interrupt for the pin
 * @return the pinchange interrupt, or -1 if there is none available
 */
INLINE uint8_t pinchangeInterrupt() {
	return pinPinchangeInterrupt;
}

/**
 * Get the direction port
 */
INLINE FLAME_register dirPort() {
	return pinDir;
}

/**
 * Get the input port
 */
INLINE FLAME_register inputPort() {
	return pinIn;
}

/**
 * Get the output port
 */
INLINE FLAME_register outputPort() {
	return pinOut;
}

/**
 * Get the bit
 */
INLINE uint8_t bit() {
	return pinPin;
}

/**
 * Get the bit
 */
INLINE uint8_t mask() {
	return _BV(pinPin);
}


}; //class Pin
} // namespace flame

#endif /* FLAME_PIN_H_ */
