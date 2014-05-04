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

/* a RingBuffer is a CharRingBuffer which has the concept of a "chunk"
 *  whose length is stored in _elementSize.  It has methods to append
 *  and consume "chunks" of this length.
 */

#ifndef FLAME_RINGBUFFER_H_
#define FLAME_RINGBUFFER_H_

#include <inttypes.h>
#include <flame/io.h>
#include <flame/CharRingBuffer.h>

namespace flame {


class RingBuffer : public CharRingBuffer {
protected:
	uint8_t				_elementCount = 0;
	uint8_t				_elementSize = 0;

public:
	bool append(char c) {
		return CharRingBuffer::append(c);
	}
	bool append(const void *p, uint16_t pLength) {
		return CharRingBuffer::append(p,pLength);
	}
	bool consume(void *p, uint16_t pLength) {
		return CharRingBuffer::consume(p,pLength);
	}

	/**
	 * Append an element to the buffer
	 * @param	p	the pointer to append from
	 * @return false if we succeeded, true otherwise
	 */
	bool append(const void *p) {
		if (CharRingBuffer::full(_elementSize)) {
			return true;
		}

		uint8_t i;

		char *c = (char *)p;

		ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
			for (i = 0; i < _elementSize; i++) {
				CharRingBuffer::append(*c++);
			}
		}

		return false;
	}

	int consume() {
		return CharRingBuffer::consume();
	}
	/**
	 * Pop an element off the ringbuffer
	 * @param p			where to write the block
	 * @return false if we succeeded, true otherwise
	 */
	bool consume(void *p) {
		if (length() < _elementSize) {
			return true;
		}

		uint8_t i;
		char *c = (char *)p;

		ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
			for (i = 0; i < _elementSize; i++) {
				*c++ = (char)CharRingBuffer::consume();
			}
		}

		return false;
	}
	bool full() {
//		return length() == _bufferSize - 1;
		return (length() > (_bufferSize - 1 - _elementSize));
	}
};

/**
 * A ring buffer
 * @tparam	elementCount		the number of elements
 * @tparam	elementSize		the number of bytes in an element
 */
template<uint8_t elementCount, uint8_t elementSize = 1>
class RingBufferImplementation : public RingBuffer {
protected:
	volatile uint8_t _myBuffer[elementCount * elementSize + 1];

public:
	/**
	 * Create a new ringbuffer
	 */
	RingBufferImplementation() :
		RingBuffer() {
		_buffer = _myBuffer;
		_bufferSize = elementCount* elementSize + 1;
		_elementCount = elementCount;
		_elementSize = elementSize;
	}
};

}
#endif /* FLAME_RINGBUFFER_H_ */
