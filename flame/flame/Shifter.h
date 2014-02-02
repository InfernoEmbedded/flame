/* Copyright (c) 2014, Inferno Embedded
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

#ifndef FLAME_SHIFTER_H_
#define FLAME_SHIFTER_H_

#include <flame/io.h>
#include <util/delay.h>

namespace flame{

void shiftout_byte_lsb(FLAME_PIN *data, FLAME_PIN *clock, uint8_t byte);
void shiftout_byte_msb(FLAME_PIN *data, FLAME_PIN *clock, uint8_t byte);


class Shifter {
public:
	virtual void shiftOut(uint8_t data, uint8_t bits) =0;
	virtual void shiftOut(uint8_t data) =0;
	virtual void shiftOut(uint8_t *data, uint8_t length) =0;
	virtual void shiftOut(uint8_t *data, uint8_t length, uint16_t elements) =0;
};

#undef FLAME_SHIFT_DELAY
#define FLAME_SHIFT_DELAY if (delay) _delay_us(delay);

/**
 * Create a new shifter
 * @tparam	clock...	an pin describing the clock line
 * @tparam	data...		an pin describing the data line
 * @tparam	msb			true to output MSB first. false for LSB
 * @tparam	rising		true if the receiver is clocked on the rising edge, false otherwise
 * @tparam	delay		the delay between state changes (us)
 */
template<FLAME_DECLARE_PIN(clock), FLAME_DECLARE_PIN(data), bool msb = true, bool rising = true,
		uint16_t delay = 0>
class ShifterImplementation : public Shifter {

private:
	/**
	 * Shift out data in MSB order, clocked on the rising edge
	 * @param shiftData		the data to shift out
	 * @param shiftLength	the number of bytes to shift
	 */
	INLINE void shiftOutMSBRising(uint8_t *shiftData, uint8_t shiftLength) {
		uint8_t dataOffClockOff = _MMIO_BYTE(dataOut) & ~_BV(dataPin) & ~_BV(clockPin);
		uint8_t dataOffClockOn = (_MMIO_BYTE(dataOut) & ~_BV(dataPin)) | _BV(clockPin);
		uint8_t dataOnClockOff = (_MMIO_BYTE(dataOut) & ~_BV(clockPin)) | _BV(dataPin);
		uint8_t dataOnClockOn = _MMIO_BYTE(dataOut) | _BV(clockPin) | _BV(dataPin);
		uint8_t FLAME_dataCopy;

		while (shiftLength--) {
			FLAME_dataCopy = *shiftData++;
			if (FLAME_dataCopy & _BV(7)) {
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(6)) {
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(5)) {
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(4)) {
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(3)) {
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(2)) {
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(1)) {
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(0)) {
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
			}
		}
		_MMIO_BYTE(dataOut) = dataOffClockOff;
	}

	/**
	 * Shift out data in MSB order, clocked on the falling edge
	 * @param shiftData		the data to shift out
	 * @param shiftLength	the number of bytes to shift
	 */
	INLINE void shiftOutMSBFalling(uint8_t *shiftData, uint8_t shiftLength) {
		uint8_t dataOffClockOff = _MMIO_BYTE(dataOut) & ~_BV(dataPin) & ~_BV(clockPin);
		uint8_t dataOffClockOn = (_MMIO_BYTE(dataOut) & ~_BV(dataPin)) | _BV(clockPin);
		uint8_t dataOnClockOff = (_MMIO_BYTE(dataOut) & ~_BV(clockPin)) | _BV(dataPin);
		uint8_t dataOnClockOn = _MMIO_BYTE(dataOut) | _BV(clockPin) | _BV(dataPin);
		uint8_t FLAME_dataCopy;

		while (!shiftLength--) {
			FLAME_dataCopy = *shiftData++;
			if (FLAME_dataCopy & _BV(7)) {
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(6)) {
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(5)) {
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(4)) {
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(3)) {
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(2)) {
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(1)) {
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(0)) {
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
			}
		}
		_MMIO_BYTE(dataOut) = dataOffClockOn;
	}

	/**
	 * Shift out data in LSB order, clocked on the rising edge
	 * @param shiftData		the data to shift out
	 * @param shiftLength	the number of bytes to shift
	 */
	INLINE void shiftOutLSBRising(uint8_t *shiftData, uint8_t shiftLength) {
		uint8_t dataOffClockOff = _MMIO_BYTE(dataOut) & ~_BV(dataPin) & ~_BV(clockPin);
		uint8_t dataOffClockOn = (_MMIO_BYTE(dataOut) & ~_BV(dataPin)) | _BV(clockPin);
		uint8_t dataOnClockOff = (_MMIO_BYTE(dataOut) & ~_BV(clockPin)) | _BV(dataPin);
		uint8_t dataOnClockOn = _MMIO_BYTE(dataOut) | _BV(clockPin) | _BV(dataPin);
		uint8_t FLAME_dataCopy;

		while (shiftLength--) {
			FLAME_dataCopy = *shiftData++;
			if (FLAME_dataCopy & _BV(0)) {
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(1)) {
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(2)) {
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(3)) {
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(4)) {
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(5)) {
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(6)) {
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(7)) {
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
			}
		}
		_MMIO_BYTE(dataOut) = dataOffClockOff;
	}

	/**
	 * Shift out data in LSB order, clocked on the falling edge
	 * @param shiftData		the data to shift out
	 * @param shiftLength	the number of bytes to shift
	 */
	INLINE void shiftOutLSBFalling(uint8_t *shiftData, uint8_t shiftLength) {
		uint8_t dataOffClockOff = _MMIO_BYTE(dataOut) & ~_BV(dataPin) & ~_BV(clockPin);
		uint8_t dataOffClockOn = (_MMIO_BYTE(dataOut) & ~_BV(dataPin)) | _BV(clockPin);
		uint8_t dataOnClockOff = (_MMIO_BYTE(dataOut) & ~_BV(clockPin)) | _BV(dataPin);
		uint8_t dataOnClockOn = _MMIO_BYTE(dataOut) | _BV(clockPin) | _BV(dataPin);
		uint8_t FLAME_dataCopy;

		while (!shiftLength--) {
			FLAME_dataCopy = *shiftData++;
			if (FLAME_dataCopy & _BV(0)) {
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(1)) {
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(2)) {
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(3)) {
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(4)) {
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(5)) {
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(6)) {
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
			}
			if (FLAME_dataCopy & _BV(7)) {
				_MMIO_BYTE(dataOut) = dataOnClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOnClockOff;
				FLAME_SHIFT_DELAY;
			} else {
				_MMIO_BYTE(dataOut) = dataOffClockOn;
				FLAME_SHIFT_DELAY;
				_MMIO_BYTE(dataOut) = dataOffClockOff;
				FLAME_SHIFT_DELAY;
			}
		}
		_MMIO_BYTE(dataOut) = dataOffClockOn;
	}

	/**
	 * Shift out data in MSB order, clocked on the rising edge
	 * @param shiftData		the data to shift out
	 * @param shiftLength	the number of bytes in an element
	 * @param elements		the number of elements
	 */
	INLINE void shiftOutElementsMSBRising(uint8_t *shiftData, uint8_t shiftLength, uint16_t elements) {
		uint8_t dataOffClockOff = _MMIO_BYTE(dataOut) & ~_BV(dataPin) & ~_BV(clockPin);
		uint8_t dataOffClockOn = (_MMIO_BYTE(dataOut) & ~_BV(dataPin)) | _BV(clockPin);
		uint8_t dataOnClockOff = (_MMIO_BYTE(dataOut) & ~_BV(clockPin)) | _BV(dataPin);
		uint8_t dataOnClockOn = _MMIO_BYTE(dataOut) | _BV(clockPin) | _BV(dataPin);
		uint8_t FLAME_dataCopy;

		while (elements--) {
			uint8_t length = shiftLength;
			while (length--) {
				FLAME_dataCopy = *shiftData++;
				if (FLAME_dataCopy & _BV(7)) {
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(6)) {
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(5)) {
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(4)) {
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(3)) {
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(2)) {
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(1)) {
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(0)) {
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
				}
			}
			_MMIO_BYTE(dataOut) = dataOffClockOff;
		}
	}

	/**
	 * Shift out data in MSB order, clocked on the falling edge
	 * @param shiftData		the data to shift out
	 * @param shiftLength	the number of bytes to shift
	 */
	INLINE void shiftOutElementsMSBFalling(uint8_t *shiftData, uint8_t shiftLength, uint16_t elements) {
		uint8_t dataOffClockOff = _MMIO_BYTE(dataOut) & ~_BV(dataPin) & ~_BV(clockPin);
		uint8_t dataOffClockOn = (_MMIO_BYTE(dataOut) & ~_BV(dataPin)) | _BV(clockPin);
		uint8_t dataOnClockOff = (_MMIO_BYTE(dataOut) & ~_BV(clockPin)) | _BV(dataPin);
		uint8_t dataOnClockOn = _MMIO_BYTE(dataOut) | _BV(clockPin) | _BV(dataPin);
		uint8_t FLAME_dataCopy;

		while (elements--) {
			uint8_t length = shiftLength;
			while (length--) {
				FLAME_dataCopy = *shiftData++;
				if (FLAME_dataCopy & _BV(7)) {
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(6)) {
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(5)) {
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(4)) {
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(3)) {
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(2)) {
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(1)) {
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(0)) {
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
				}
			}
			_MMIO_BYTE(dataOut) = dataOffClockOn;
		}
	}

	/**
	 * Shift out data in LSB order, clocked on the rising edge
	 * @param shiftData		the data to shift out
	 * @param shiftLength	the number of bytes to shift
	 */
	INLINE void shiftOutElementsLSBRising(uint8_t *shiftData, uint8_t shiftLength, uint16_t elements) {
		uint8_t dataOffClockOff = _MMIO_BYTE(dataOut) & ~_BV(dataPin) & ~_BV(clockPin);
		uint8_t dataOffClockOn = (_MMIO_BYTE(dataOut) & ~_BV(dataPin)) | _BV(clockPin);
		uint8_t dataOnClockOff = (_MMIO_BYTE(dataOut) & ~_BV(clockPin)) | _BV(dataPin);
		uint8_t dataOnClockOn = _MMIO_BYTE(dataOut) | _BV(clockPin) | _BV(dataPin);
		uint8_t FLAME_dataCopy;

		while (elements--) {
			uint8_t length = shiftLength;
			while (length--) {
				FLAME_dataCopy = *shiftData++;
				if (FLAME_dataCopy & _BV(0)) {
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(1)) {
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(2)) {
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(3)) {
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(4)) {
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(5)) {
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(6)) {
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(7)) {
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
				}
			}
			_MMIO_BYTE(dataOut) = dataOffClockOff;
		}
	}

	/**
	 * Shift out data in LSB order, clocked on the falling edge
	 * @param shiftData		the data to shift out
	 * @param shiftLength	the number of bytes to shift
	 */
	INLINE void shiftOutElementsLSBFalling(uint8_t *shiftData, uint8_t shiftLength, uint16_t elements) {
		uint8_t dataOffClockOff = _MMIO_BYTE(dataOut) & ~_BV(dataPin) & ~_BV(clockPin);
		uint8_t dataOffClockOn = (_MMIO_BYTE(dataOut) & ~_BV(dataPin)) | _BV(clockPin);
		uint8_t dataOnClockOff = (_MMIO_BYTE(dataOut) & ~_BV(clockPin)) | _BV(dataPin);
		uint8_t dataOnClockOn = _MMIO_BYTE(dataOut) | _BV(clockPin) | _BV(dataPin);
		uint8_t FLAME_dataCopy;

		while (elements--) {
			uint8_t length = shiftLength;
			while (length--) {
				FLAME_dataCopy = *shiftData++;
				if (FLAME_dataCopy & _BV(0)) {
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(1)) {
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(2)) {
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(3)) {
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(4)) {
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(5)) {
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(6)) {
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
				}
				if (FLAME_dataCopy & _BV(7)) {
					_MMIO_BYTE(dataOut) = dataOnClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOnClockOff;
					FLAME_SHIFT_DELAY;
				} else {
					_MMIO_BYTE(dataOut) = dataOffClockOn;
					FLAME_SHIFT_DELAY;
					_MMIO_BYTE(dataOut) = dataOffClockOff;
					FLAME_SHIFT_DELAY;
				}
			}
			_MMIO_BYTE(dataOut) = dataOffClockOn;
		}
	}

public:
	/**
	 * Constructor - set the pins to output
	 */
	ShifterImplementation() {
		setOutput(FLAME_PIN_PARMS(clock));
		setOutput(FLAME_PIN_PARMS(data));
	}

	/**
	 * Shift out a number of bits (LSB aligned)
	 * @param data	a byte containing the bits to shift
	 * @param bits	the number of bits to shift
	 */
	void shiftOut(uint8_t data, uint8_t bits) {
		if (msb) {
			for (int8_t i = bits - 1; i >= 0; i--) {
				if ((data >> i) & 0x01) {
					pinOn(FLAME_PIN_PARMS(data));
				} else {
					pinOff(FLAME_PIN_PARMS(data));
				}
				pinOn(FLAME_PIN_PARMS(clock));
				FLAME_SHIFT_DELAY;

				pinOff(FLAME_PIN_PARMS(clock));
				FLAME_SHIFT_DELAY;
			}
		} else {
			for (uint8_t i = 0; i < bits; i++) {
				if ((data >> i) & 0x01) {
					pinOn(FLAME_PIN_PARMS(data));
				} else {
					pinOff(FLAME_PIN_PARMS(data));
				}

				pinOn(FLAME_PIN_PARMS(clock));
				FLAME_SHIFT_DELAY;

				pinOff(FLAME_PIN_PARMS(clock));
				FLAME_SHIFT_DELAY;
			}
		}
	}

	/**
	 * Shift out a byte of data
	 * @param data the data to output
	 */
	void shiftOut(uint8_t data) {
		if (msb) {
			for (int8_t i = 7; i >= 0; i--) {
				if ((data >> i) & 0x01) {
					pinOn(FLAME_PIN_PARMS(data));
				} else {
					pinOff(FLAME_PIN_PARMS(data));
				}

				pinOn(FLAME_PIN_PARMS(clock));
				FLAME_SHIFT_DELAY;

				pinOff(FLAME_PIN_PARMS(clock));
				FLAME_SHIFT_DELAY;
			}
		} else {
			for (uint8_t i = 0; i < 8; i++) {
				if ((data >> i) & 0x01) {
					pinOn(FLAME_PIN_PARMS(data));
				} else {
					pinOff(FLAME_PIN_PARMS(data));
				}

				pinOn(FLAME_PIN_PARMS(clock));
				FLAME_SHIFT_DELAY;

				pinOff(FLAME_PIN_PARMS(clock));
				FLAME_SHIFT_DELAY;
			}
		}
	}

	void shiftOut(uint8_t *shiftData, uint8_t shiftLength) {
		if (msb) {
			if (rising) {
				shiftOutMSBRising(shiftData, shiftLength);
			} else {
				shiftOutMSBFalling(shiftData, shiftLength);
			}
		} else {
			if (rising) {
				shiftOutLSBRising(shiftData, shiftLength);
			} else {
				shiftOutLSBFalling(shiftData, shiftLength);
			}
		}
	}

	void shiftOut(uint8_t *shiftData, uint8_t dataLength, uint16_t elements) {
		if (msb) {
			if (rising) {
				shiftOutElementsMSBRising(shiftData, dataLength, elements);
			} else {
				shiftOutElementsMSBFalling(shiftData, dataLength, elements);
			}
		} else {
			if (rising) {
				shiftOutElementsLSBRising(shiftData, dataLength, elements);
			} else {
				shiftOutElementsLSBFalling(shiftData, dataLength, elements);
			}
		}
	}
};

}
#endif /* FLAME_SHIFTER_H_ */
