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


#include "MHV_RingBuffer.h"

MHV_RingBuffer::MHV_RingBuffer(char *buffer, uint8_t size) {
	_buffer = buffer;
	_size = size;
	_head = 0; // Where we are about to write to
	_tail = 0; // Where we will start reading from
}

/* Append a character to the buffer
 * return false if we succeeded, true otherwise
 */
bool MHV_RingBuffer::append(char c) {
	uint8_t next = _head + 1;
	if (next > _size) {
		next = 0;
	}

	// Don't overwrite valid data in the buffer
	if (next == _tail) {
		return true;
	}

	_buffer[_head] = c;
	_head = next;

	return false;
}

/* Pop a byte off the ringbuffer
 */
int MHV_RingBuffer::consume(void) {
	if (_head == _tail) {
		return -1;
	}

	unsigned char c = _buffer[_tail];
	_tail = _tail + 1;
	if (_tail > _size) {
			_tail -= _size;
	}
	return c;
}

/* Discard the contents of the ringbuffer
 */
void MHV_RingBuffer::flush(void) {
	_head = _tail;
}

/* Return the size of the ringbuffer
 */
uint8_t MHV_RingBuffer::size(void) {
	return _size;
}

/* Return the number of bytes in the ringbuffer
 */
uint8_t MHV_RingBuffer::length(void) {
	uint16_t length = _size + _head - _tail;
	if (length >= _size) {
		length -= _size;
	}

	return (uint8_t) length;
}

/* Return true if the ringbuffer is full
 */
bool MHV_RingBuffer::full(void) {
	return length() == _size - 1;
}

int MHV_RingBuffer::peekHead(void) {
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
