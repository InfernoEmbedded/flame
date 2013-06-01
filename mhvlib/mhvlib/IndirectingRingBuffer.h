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


#ifndef MHV_INDIRECTINGRINGBUFFER_H
#define MHV_INDIRECTINGRINGBUFFER_H

#include <mhvlib/RingBuffer.h>

namespace mhvlib {

class IndirectingRingBuffer : public RingBuffer {
  };


/**
 * A ring buffer
 * @tparam	elementCount	the number of elements
 * @tparam	elementSize		the number of bytes in an element
 */
template<uint8_t elementCount, uint8_t elementSize = 1>
  class IndirectingRingBufferImplementation : public IndirectingRingBuffer {
protected:
	volatile uint8_t _myBuffer[elementCount * elementSize + 1];

	const char magic_escape_character = '|';
	const uint8_t magic_worth_indirecting_threshold = 0;

	uint8_t currently_consuming;

	uint16_t indirection_length;
	char * indirection_address;
	uint16_t indirection_offset;
	void (*completeFunction)(const char *);

// it would be a really bad idea to use magic_escape_character as a
// value in this enum. 
enum IndirectionType {
  BUFFER = 0,
  VOIDP_WITH_UINT8_LENGTH_AND_COMPLETEFUNCTION = 1,
  PGM_P_STRING = 2,
  NULL_TERMINATED_CHARSTAR_WITH_COMPLETEFUNCTION = 3,
};
public:
	/**
	 * Create a new ringbuffer
	 */
	IndirectingRingBufferImplementation() :
		IndirectingRingBuffer() {
		_buffer = _myBuffer;
		_bufferSize = elementCount* elementSize + 1;
		_elementCount = elementCount;
		_elementSize = elementSize;
		currently_consuming = BUFFER;
	}

	bool append(char c) { // should check can-fit here...
		if (c == magic_escape_character) {
			RingBuffer::append(c);
		}
		return RingBuffer::append(c);
	}
	bool append(const void *p) { // can't indirect this - may be reused by caller
		return RingBuffer::append(p);
	}
	bool append(const void *p, uint8_t pLength) { // can't indirect this - may be reused by caller
		return RingBuffer::append(p,pLength);
	}
	/**
	 * Send a null-terminated string, possibly indirected
	 * @param string		the string to to send
	 * @param completeFunction	called when string is no longer referenced
	 */
	bool append(const char *string,void (*completeFunction)(const char *)) {
			RingBuffer::append(magic_escape_character);
			RingBuffer::append(NULL_TERMINATED_CHARSTAR_WITH_COMPLETEFUNCTION);
			RingBuffer::append((char)((uint16_t)string >> 8));
			RingBuffer::append((char)((uint16_t)string & 0xff));
			RingBuffer::append((char)((uint16_t)completeFunction >> 8));
			RingBuffer::append((char)((uint16_t)completeFunction & 0xff));
			return 0; // success
	}

	/**
	 * Send a buffer of fixed length, possibly indirected
	 * @param p			pointer to buffer to send
	 * @param completeFunction	called when buffer is no longer referenced
	 */
	bool append(const void *p, uint8_t pLength,void (*completeFunction)(const char *)) {
		if (pLength > magic_worth_indirecting_threshold) {
			// should check can_fit here
			RingBuffer::append(magic_escape_character);
			RingBuffer::append(VOIDP_WITH_UINT8_LENGTH_AND_COMPLETEFUNCTION);
			RingBuffer::append(pLength);
			RingBuffer::append((char)((uint16_t)p >> 8));
			RingBuffer::append((char)((uint16_t)p & 0xff));
			RingBuffer::append((char)((uint16_t)completeFunction >> 8));
			RingBuffer::append((char)((uint16_t)completeFunction & 0xff));
			return 0; // success
		}
		return RingBuffer::append(p,pLength);
	}

	/**
	 * Send a null-terminated program string 
	 * @param string		pointer to progmem string to send
	 * @param completeFunction	called when buffer is no longer referenced
	 */
	bool append_P(PGM_P string) {
		// could check to see whether it was work indirecting here
		RingBuffer::append(magic_escape_character);
		RingBuffer::append(PGM_P_STRING);
		RingBuffer::append((char)((uint16_t)string >> 8));
		RingBuffer::append((char)((uint16_t)string & 0xff));
		return 0; // success
	}

	// should really override _full here and count the extra
	// characters required for escaping!


	/**
	 * Return a character from the ringbuffer
	 */
	int consume() {
		int ret;
		while (1) {
			if (currently_consuming == BUFFER) {
				ret = RingBuffer::consume();
				if (ret != magic_escape_character) {
					goto done;
				}
				// OK, so we've hit the magic escape character.
				currently_consuming = RingBuffer::consume();
				if (currently_consuming == magic_escape_character) {
					// this was an escaped magic escape character
					ret = currently_consuming;
					goto done;
				}
				// it wasn't an escaped magic character.  Grab all the relevant stuff out of the buffer and then loop around.
				if (currently_consuming == VOIDP_WITH_UINT8_LENGTH_AND_COMPLETEFUNCTION) {
					indirection_length = RingBuffer::consume();
					indirection_address = (char*)(RingBuffer::consume() << 8 |
								      RingBuffer::consume());
					indirection_offset = 0;
					completeFunction =  (void (*)(const char*))(RingBuffer::consume() << 8 |
						     RingBuffer::consume());
				} else if(currently_consuming == PGM_P_STRING) {
					indirection_address = (char*)(RingBuffer::consume() << 8 |
								      RingBuffer::consume());
					indirection_offset = 0;
				} else if(currently_consuming == NULL_TERMINATED_CHARSTAR_WITH_COMPLETEFUNCTION) {	
					indirection_address = (char*)(RingBuffer::consume() << 8 |
								      RingBuffer::consume());
					indirection_offset = 0;
					completeFunction =  (void (*)(const char*))(RingBuffer::consume() << 8 |
						     RingBuffer::consume());
				} else {
					// die horribly!
				}
			}
			if (currently_consuming == VOIDP_WITH_UINT8_LENGTH_AND_COMPLETEFUNCTION) {
				if (indirection_length == indirection_offset) {
					currently_consuming = BUFFER;
					completeFunction(indirection_address);
				} else {
					ret = indirection_address[indirection_offset++];
					//					ret = indirection_offset-1 + '0';
					goto done;
				}
			}
			if (currently_consuming == PGM_P_STRING) {
				ret = pgm_read_byte((PGM_P)indirection_address + indirection_offset++);
				if (ret == '\0') {
					// null-terminated string - all done (we do NOT return the null!)
					currently_consuming = BUFFER;
				} else {
					goto done;
				}
			}
			if (currently_consuming == NULL_TERMINATED_CHARSTAR_WITH_COMPLETEFUNCTION) {
				ret = indirection_address[indirection_offset++];
				if (ret == '\0') {
					// null-terminated string - all done (we do NOT return the null!)
					completeFunction(indirection_address);
					currently_consuming = BUFFER;
				} else {
					goto done;
				}
			}
		}
	done:
		return ret;
	}

	bool consume(void *p, uint8_t pLength) {
		return RingBuffer::consume(p,pLength);
	}
	bool consume(void *p) {
		return RingBuffer::consume(p);
	}

};

}

#endif // MHV_INDIRECTINGRINGBUFFER_H
