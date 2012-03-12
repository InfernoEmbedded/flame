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


#include "MHV_RingBuffer.h"

/**
 * Create a new ringbuffer
 * @param	buffer	memory to use for the ringbuffer
 * @param	size	the size of the available memory
 */
MHV_RingBuffer::MHV_RingBuffer(char *buffer, uint8_t size) {
	_buffer = buffer;
	_size = size;
	_head = 0; // Where we are about to write to
	_tail = 0; // Where we will start reading from
}

/**
 * Determine where the next location will be
 * @param	index	the current index
 * @return the next index
 */
inline uint8_t MHV_RingBuffer::increment (uint8_t index) {
	uint8_t next = index + 1;
	if (next == _size) {
		next = 0;
	}

	return next;
}

/**
 * Append a character to the buffer
 * @return false if we succeeded, true otherwise
 */
bool MHV_RingBuffer::append(char c) {
	uint8_t next = increment(_head);

	// Don't overwrite valid data in the buffer
	if (next == _tail) {
		return true;
	}

	_buffer[_head] = c;
	_head = next;

	return false;
}

/**
 * Append a block of data to the buffer
 * @param	p	the pointer to append from
 * @param	pLength the number of bytes to append
 * @return false if we succeeded, true otherwise
 */
bool MHV_RingBuffer::append(const void *p, uint8_t pLength) {
	if (full(pLength)) {
		return true;
	}

	uint8_t i;

	char *c = (char *)p;
	for (i = 0; i < pLength; i++) {
		append(*c++);
	}

	return false;
}

/**
 * Pop a byte off the ringbuffer
 */
int MHV_RingBuffer::consume() {
	if (_head == _tail) {
		return -1;
	}

	unsigned char c = _buffer[_tail];
	_tail = increment(_tail);
	return c;
}

/**
 * Pop a block off the ringbuffer
 * @param p			where to write the block
 * @param pLength	the length of the block
 * @return false if we succeeded, true otherwise
 */
bool MHV_RingBuffer::consume(void *p, uint8_t pLength) {
	if (length() < pLength) {
		return true;
	}

	uint8_t i;
	char *c = (char *)p;

	for (i = 0; i < pLength; i++) {
		*c++ = (char)consume();
	}

	return false;
}


/**
 * Discard the contents of the ringbuffer
 */
void MHV_RingBuffer::flush() {
	_head = _tail = 0;
}

/**
 * Get the size of the ringbuffer
 * @return the size of the ringbuffer
 */
PURE uint8_t MHV_RingBuffer::size() {
	return _size;
}

/**
 * Get the length of the contents of the ringbuffer
 * Return the number of bytes in the ringbuffer
 */
PURE uint8_t MHV_RingBuffer::length() {
	int16_t length = _head - _tail;
	if (length < 0) {
// The pointers have wrapped
		length = (_size - _tail) + _head + 1;
	}

	return (uint8_t) length;
}

/**
 * Check if the ringbuffer is full
 * @return true if the ringbuffer is full
 */
PURE bool MHV_RingBuffer::full() {
	return length() == _size - 1;
}

/**
 * Check if an object can fit in the ringbuffer
 * @param blockLength	the length of the object to fit in
 * @return true if the ringbuffer is full
 */
PURE bool MHV_RingBuffer::full(uint8_t blockLength) {
	return length() > (_size - 1 - blockLength);
}


/**
 * Check the first character in the buffer
 * @return the character, or -1 if the buffer is empty
 */
PURE int MHV_RingBuffer::peekHead() {
	if (_head == _tail) {
		return -1;
	}

	// We want the character just before head
	int offset;
	if (0 == _head) {
		offset = _size - 1;
	} else {
		offset = _head - 1;
	}
	return (int) _buffer[offset];
}
