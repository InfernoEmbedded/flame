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


#ifndef MHV_CHARRINGBUFFER_H_
#define MHV_CHARRINGBUFFER_H_

#include <inttypes.h>
#include <mhvlib/io.h>

namespace mhvlib {


class CharRingBuffer {
	//protected:
public:
	volatile uint16_t 	_head;
	volatile uint16_t 	_used;
	volatile uint8_t	*_buffer = NULL;
	uint16_t			_bufferSize;

public:
	/**
	 * Create a new ringbuffer
	 */
	CharRingBuffer() :
		_head(0),
		_used(0) {};

	/**
	 * Get the size of the ringbuffer
	 */
	uint16_t size() {
		return _bufferSize;
	}

	/**
	 * Get the head offset
	 */
	uint16_t head() {
		return _head;
	}

	/**
	 * Append a character to the buffer
	 * @return false if we succeeded, true otherwise
	 */
	bool append(const char c) {
		if (_used == _bufferSize) { // we're full
			return true; // failure
		}
		ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
			_buffer[_head] = c;
			if (_head == _bufferSize - 1) {
				_head = 0;
			} else {
				_head++;
			}
			_used++;
		}
		return false; // success
	}

	/**
	 * Append a block of data to the buffer
	 * @param	p	the pointer to append from
	 * @param	pLength the number of bytes to append
	 * @return false if we succeeded, true otherwise
	 */
	bool append(const void *p, uint8_t pLength) {
		if (full(pLength)) {
			return true;
		}

		uint8_t i;

		char *c = (char *)p;

		ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
			for (i = 0; i < pLength; i++) {
				append(*c++);
			}
		}

		return false;
	}

	/**
	 * Pop a byte off the ringbuffer
	 */
	int consume() {
		int ret;
		
		if (_used == 0) {
			return -1;
		}
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			uint16_t used_start = _head;
			if (used_start < _used) {
				// tail is at other end of buffer
				ret = _buffer[_bufferSize - (_used - used_start)];
			} else {
				ret = _buffer[_head - _used];
			}
			_used--;
		}
		return ret;
	}

	/**
	 * Pop a block off the ringbuffer
	 * @param p			where to write the block
	 * @param pLength	the length of the block
	 * @return false if we succeeded, true otherwise
	 */
	bool consume(void *p, uint8_t pLength) {
		if (length() < pLength) {
			return true;
		}

		uint8_t i;
		char *c = (char *)p;

		ATOMIC_BLOCK (ATOMIC_RESTORESTATE) {
			for (i = 0; i < pLength; i++) {
				*c++ = (char)consume();
			}
		}

		return false;
	}

	/**
	 * Discard the contents of the ringbuffer
	 */
	void flush() {
		_head = 0;
		_used = 0;
	}

	/**
	 * Get the length of the contents of the ringbuffer
	 * Return the number of bytes in the ringbuffer
	 */
	uint16_t length() {
		return _used;
	}

	/**
	 * Check if an object can fit in the ringbuffer
	 * @param blockLength	the length of the object to fit in
	 * @return true if the ringbuffer is full
	 */
	bool full(uint8_t blockLength) {
		return blockLength > freeSpace();
	}

	/**
	 * return amount of free space in the buffer
	 * @return amount of free space in bytes
	 */
	 uint16_t freeSpace() {
		return (_bufferSize - _used);
	 }

	/**
	 * Check the most recent character in the buffer
	 * @return the character, or -1 if the buffer is empty
	 */
	int peekHead() {
		if (_used == 0) {
			return -1;
		}

		// We want the character just before head
		int offset;
		if (0 == _head) {
			offset = _bufferSize - 1;
		} else {
			offset = _head - 1;
		}
		return (int) _buffer[offset];
	}
};

/**
 * A ring buffer
 * @tparam	bufferSize	the size of the ringbuffer
 */
template<uint16_t buffersize = 100>
class CharRingBufferImplementation : public CharRingBuffer {
protected:
	volatile uint8_t _myBuffer[buffersize];

public:
	/**
	 * Create a new ringbuffer
	 */
	CharRingBufferImplementation() :
		CharRingBuffer() {
		_buffer = _myBuffer;
		_bufferSize = buffersize;
		_used = 0;
	}
};

}
#endif /* MHV_CHARRINGBUFFER_H_ */
