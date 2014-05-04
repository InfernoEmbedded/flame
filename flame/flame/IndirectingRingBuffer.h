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

#ifndef FLAME_INDIRECTINGRINGBUFFER_H
#define FLAME_INDIRECTINGRINGBUFFER_H

#include <flame/CharRingBuffer.h>

namespace flame {

class IndirectingRingBuffer: public CharRingBuffer {

// it would be a really bad idea to use magic_escape_character as a
// value in this enum.
	enum IndirectionType {
		BUFFER = 65,
		VOIDP_WITH_UINT8_LENGTH_AND_COMPLETEFUNCTION = 66,
		PGM_P_STRING = 67,
		NULL_TERMINATED_CHARSTAR_WITH_COMPLETEFUNCTION = 68,
		VOIDP_WITH_UINT16_LENGTH_AND_COMPLETEFUNCTION = 69,
	};

protected:
	const char magic_escape_character = '|';

	IndirectionType currently_consuming = BUFFER;

	uint16_t indirection_length;
	char * indirection_address;
	uint16_t indirection_offset;
	void (*completeFunction)(const char *);

public:

	PURE bool success() {
		return true;
	}
	;
	PURE bool failure() {
		return false;
	}
	;

	char a_magic_escape_character() {
		return magic_escape_character;
	}
	/**
	 * see if there is room for a character in the buffer:
	 * @param 			the character to append
	 */
	bool canFit(char c) {
		if (c == magic_escape_character) {
			return freeSpace() > 1;
		}
		return freeSpace() > 0;
	}
	/**
	 * append a character
	 * @param 			the character to append
	 */
	bool append(const char c) {
		bool ret;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			if (!canFit(c)) {
				ret = failure(); // failure
			} else {
				if (c == magic_escape_character) {
					CharRingBuffer::append(c); // ignored return value
				}
				ret = !CharRingBuffer::append(c); // INVERTED
			}
		}
		return ret;
	}
	template<class pLength_t>
	PURE uint16_t escapedLength(const void *p, pLength_t pLength) {
		pLength_t count = 0;
		for (pLength_t i = 0; i < pLength; i++) {
			if (((char*) p)[i] == magic_escape_character) {
				count++;
			}
		}
		return pLength + count;
	}

	/**
	 * see if there is room for a void pointer in the buffer (directly)
	 * @param p			pointer to the data
	 * @param pLength		amount of data to append
	 */
	template<class pLength_t>
	bool canFit(const void *p, pLength_t pLength) {
		return (escapedLength(p, pLength) > freeSpace());
	}
	/**
	 * append a specific length of data pointed to by a void pointer
	 * @param p			pointer to the data
	 * @param pLength		amount of data to append
	 */
	bool append(const void *p, const uint16_t pLength) { // can't indirect this - may be reused by caller
		bool ret;
		uint16_t escaped_length = escapedLength(p, pLength);
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			if (escaped_length > freeSpace()) {
				ret = failure();
			} else {
				for (uint16_t i = 0; i < pLength; i++) {
					append(((char*) p)[i]);
				}
				ret = success(); // success
			}
		}
		return ret;
	}

	PURE uint16_t escapedLength(const char *string) {
		uint16_t count = 0;
		for (char * x = (char*) string; *x; x++) {
			count++;
			if (*x == magic_escape_character) {
				count++;
			}
		}
		return count;
	}
	/**
	 * see if there is room for a null-terminated string in the buffer (directly)
	 * @param p			pointer to the data
	 */
	bool canFit(const char *string) {
		return (escapedLength(string) <= freeSpace());
	}
	/**
	 * see if there is room for a null-terminated string in the buffer (possibly indirected)
	 * @param p			pointer to the data
	 * @param pLength		amount of data to append
	 */
	bool canFit(const char *string, void (*completeFunction)(const char *) UNUSED) {
		// we require 6 bytes for this....
		if (freeSpace() >= 6) {
			return true;
		}
		return canFit(string);
	}

	bool canFit_P(PGM_P string) {
		uint16_t free = freeSpace();
		if (free >= 4) {
			// can be indirected
			return true;
		}
		if (strlen_P(string) < free) {
			// can just be dropped in
			return true;
		}
		return false;
	}

	/**
	 * Send a null-terminated string directly
	 * @param string		the string to to send
	 */
	bool append(const char *string) {
		uint16_t escaped_length = escapedLength(string);
		bool ret;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			if (escaped_length > freeSpace()) {
				ret = !success(); // failure
			} else {
				for(char * x=(char *)string;*x;x++) {
					append(*x);
				}
				ret = success(); // success
			}
		}
		return ret;
	}

	/**
	 * Send a null-terminated string, indirectly
	 * @param string		the string to to send
	 * @param completeFunction	called when string is no longer referenced
	 */
	bool appendIndirectly(const char *string,void (*completeFunction)(const char *)) {
		bool ret;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			if (freeSpace() < 6) {
				ret = !success(); // failure
			} else {
				CharRingBuffer::append(magic_escape_character);
				CharRingBuffer::append(NULL_TERMINATED_CHARSTAR_WITH_COMPLETEFUNCTION);
				CharRingBuffer::append((char)((uint16_t)string >> 8));
				CharRingBuffer::append((char)((uint16_t)string & 0xff));
				CharRingBuffer::append((char)((uint16_t)completeFunction >> 8));
				CharRingBuffer::append((char)((uint16_t)completeFunction & 0xff));
				ret = success(); // success
			}
		}
		return ret;
	}

	/**
	 * Send a null-terminated string, possibly indirectly
	 * @param string		the string to to send
	 * @param completeFunction	called when string is no longer referenced
	 */
	bool append(const char *string,void (*completeFunction)(const char *)) {
		uint16_t escaped_length = escapedLength(string);
		bool ret;
		if (escaped_length <= 6) { // directly append it; should 6 be higher?
			ret = append(string);
			completeFunction(string);
		} else { // indirect it
			ret = appendIndirectly(string,completeFunction);
		}
		return ret;
	}

	// *********** start VOIDP_WITH_UINT16_LENGTH_AND_COMPLETEFUNCTION

	/**
	 * Send a buffer of fixed length, indirectly
	 * @param p			pointer to buffer to send
	 * @param pLength		how many bytes to send
	 * @param completeFunction	called when buffer is no longer referenced
	 */
	bool appendIndirectly(const void *p, uint16_t pLength,void (*completeFunction)(const void *)) {
		bool ret;
		if (freeSpace() < 8) {
			ret = !success(); // failure
		} else {
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				CharRingBuffer::append(magic_escape_character);
				CharRingBuffer::append(VOIDP_WITH_UINT16_LENGTH_AND_COMPLETEFUNCTION);
				CharRingBuffer::append((char)(pLength >> 8));
				CharRingBuffer::append((char)(pLength & 0xff));
				CharRingBuffer::append((char)((uint16_t)p >> 8));
				CharRingBuffer::append((char)((uint16_t)p & 0xff));
				CharRingBuffer::append((char)((uint16_t)completeFunction >> 8));
				CharRingBuffer::append((char)((uint16_t)completeFunction & 0xff));
				ret = success(); // success
			}
		}
		return ret;
	}

	// ************ end VOIDP_WITH_UINT16_LENGTH_AND_COMPLETEFUNCTION

	/**
	 * Send a buffer of fixed length, indirectly
	 * @param p			pointer to buffer to send
	 * @param pLength		how many bytes to send
	 * @param completeFunction	called when buffer is no longer referenced
	 */
	bool appendIndirectly(const void *p, uint8_t pLength,void (*completeFunction)(const void *)) {
		bool ret;
		if (freeSpace() < 7) {
			ret = !success(); // failure
		} else {
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				CharRingBuffer::append(magic_escape_character);
				CharRingBuffer::append(VOIDP_WITH_UINT8_LENGTH_AND_COMPLETEFUNCTION);
				CharRingBuffer::append(pLength);
				CharRingBuffer::append((char)((uint16_t)p >> 8));
				CharRingBuffer::append((char)((uint16_t)p & 0xff));
				CharRingBuffer::append((char)((uint16_t)completeFunction >> 8));
				CharRingBuffer::append((char)((uint16_t)completeFunction & 0xff));
				ret = success(); // success
			}
		}
		return ret;
	}
	/**
	 * Send a buffer of fixed length, possibly indirected
	 * @param p			pointer to buffer to send
	 * @param completeFunction	called when buffer is no longer referenced
	 */
	template<class pLength_t>
	bool append(const void *p, pLength_t pLength,void (*completeFunction)(const void *)) {
		uint16_t escaped_length = escapedLength(p,pLength);
		bool ret;
		if (escaped_length <= 6 + sizeof(pLength_t)) { // directly append it
			ret = append(p,pLength);
			if (ret == success()) {
				completeFunction(p);
			}
		} else {
			ret = appendIndirectly(p,pLength,completeFunction);
		}
		return ret;
	}

	/**
	 * Return the length a string will be when it is escaped
	 * @param string	the string to return the escaped length of
	 */
	PURE uint16_t escapedLength_P(PGM_P string) {
		PGM_P x = string;
		uint16_t count = 0;
		while(char ret = pgm_read_byte((PGM_P)x++)) {
			count++;
			if (ret == magic_escape_character) {
				count++;
			}
		}
		return count;
	}

	/**
	 * Send a null-terminated program string, directly
	 * @param string		pointer to progmem string to send
	 */
	bool appendDirectly_P(PGM_P string) {
		bool ret;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			uint16_t escaped_length = escapedLength_P(string);
			if (escaped_length > freeSpace()) {
				ret = failure();
			} else {
				PGM_P x = string;
				char c;
				while((c = pgm_read_byte((PGM_P)x++))) {
					append(c);
				}
				ret = success();
			}
		}
		return ret;
	}

	/**
	 * Send a null-terminated program string, indirectly
	 * @param string		pointer to progmem string to send
	 */
	bool appendIndirectly_P(PGM_P string) {
		bool ret;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			if (freeSpace() < 4) {
				ret = !success(); // failure
			} else {
				CharRingBuffer::append(magic_escape_character);
				CharRingBuffer::append(PGM_P_STRING);
				CharRingBuffer::append((char)((uint16_t)string >> 8));
				CharRingBuffer::append((char)((uint16_t)string & 0xff));
				ret = success(); // success
			}
		}
		return ret;
	}

	/**
	 * Send a null-terminated program string, possibly indirectly
	 * @param string		pointer to progmem string to send
	 * @param completeFunction	called when buffer is no longer referenced
	 */
	bool append_P(PGM_P string) {
		uint16_t escaped_length = escapedLength_P(string);
		bool ret;
		if (escaped_length < 4) { // directly append it
			ret = appendDirectly_P(string);
		} else {
			ret = appendIndirectly_P(string);
		}
		return ret;
	}

	/*
	 * Append a null-terminated string to the buffer *including the null!*
	 * @param string		the string to append
	 */
	bool appendNullTerminatedString(const char * string) {
		uint16_t escaped_length = escapedLength(string);
		escaped_length++; // the null...
		bool ret;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			if (escaped_length > freeSpace()) {
				ret = failure();
			} else {
				for(char * x=(char *)string;*x;x++) {
					append(*x);
				}
				append((char)0);
				ret = success();
			}
		}
		return ret;
	}

	/*
	 * Consume a null-terminated string to the buffer
	 * @param buffer		buffer to plonk your stuff in
	 * @param maxlen		size of buffer
	 * @return 			length of string in buffer *including* the null character.  returns 0 if no null was found within maxlen characters
	 */
	uint16_t consumeNullTerminatedString(char * buffer,uint16_t maxlen) {
		char mychar = (char)0;
		uint16_t x = length_to_char(&mychar);
		if (x > maxlen) {
			return 0;
		}
		uint16_t i = 0;
		int c;
		for (c = consume(); c != (char)0; c = consume()) {
			buffer[i++] = (char)c;
		}
		buffer[i++] = (char)c;
		return i;
	}
	/**
	 * Return a character from the ringbuffer
	 */
	int consume() {
		int ret;
		while (1) {
			if (currently_consuming == BUFFER) {
				ret = CharRingBuffer::consume();
				if (ret != magic_escape_character) {
					goto done; // this also hadles CharRingBuffer() returning -1...
				}
				// OK, so we've hit the magic escape character.
				currently_consuming = (IndirectionType)CharRingBuffer::consume();
				if (currently_consuming == magic_escape_character) {
					// this was an escaped magic escape character
					ret = currently_consuming;
					currently_consuming = BUFFER;
					goto done;
				}
				// it wasn't an escaped magic character.  Grab all the relevant stuff out of the buffer and then loop around.
				if (currently_consuming == VOIDP_WITH_UINT8_LENGTH_AND_COMPLETEFUNCTION) {
					indirection_length = CharRingBuffer::consume();
					indirection_address = (char*)(CharRingBuffer::consume() << 8 |
					CharRingBuffer::consume());
					indirection_offset = 0;
					completeFunction = (void (*)(const char*))(CharRingBuffer::consume() << 8 |
					CharRingBuffer::consume());
				} else if(currently_consuming == PGM_P_STRING) {
					indirection_address = (char*)(CharRingBuffer::consume() << 8 |
					CharRingBuffer::consume());
					indirection_offset = 0;
				} else if(currently_consuming == NULL_TERMINATED_CHARSTAR_WITH_COMPLETEFUNCTION) {
					indirection_address = (char*)(CharRingBuffer::consume() << 8 |
					CharRingBuffer::consume());
					indirection_offset = 0;
					completeFunction = (void (*)(const char*))(CharRingBuffer::consume() << 8 |
					CharRingBuffer::consume());
				} else if (currently_consuming == VOIDP_WITH_UINT16_LENGTH_AND_COMPLETEFUNCTION) {
					indirection_length = CharRingBuffer::consume() << 8 |
					CharRingBuffer::consume();
					indirection_address = (char*)(CharRingBuffer::consume() << 8 |
					CharRingBuffer::consume());
					indirection_offset = 0;
					completeFunction = (void (*)(const char*))(CharRingBuffer::consume() << 8 |
					CharRingBuffer::consume());
				} else {
					// die horribly!
				}
			}

			if (currently_consuming == VOIDP_WITH_UINT8_LENGTH_AND_COMPLETEFUNCTION) {
				ret = indirection_address[indirection_offset++];
				if (indirection_length == indirection_offset) {
					currently_consuming = BUFFER;
					completeFunction(indirection_address);
				}
				goto done;
			}
			if (currently_consuming == PGM_P_STRING) {
				ret = pgm_read_byte((PGM_P)indirection_address + indirection_offset++);
				if (pgm_read_byte((PGM_P)indirection_address + indirection_offset) == '\0') {
					// null-terminated string - all done (we do NOT return the null!)
					currently_consuming = BUFFER;
				}
				goto done;
			}
			if (currently_consuming == NULL_TERMINATED_CHARSTAR_WITH_COMPLETEFUNCTION) {
				ret = indirection_address[indirection_offset++];
				if (indirection_address[indirection_offset] == '\0') {
					// null-terminated string - all done (we do NOT return the null!)
					completeFunction(indirection_address);
					currently_consuming = BUFFER;
				}
				goto done;
			}
			if (currently_consuming == VOIDP_WITH_UINT16_LENGTH_AND_COMPLETEFUNCTION) {
				ret = indirection_address[indirection_offset++];
				if (indirection_length == indirection_offset) {
					currently_consuming = BUFFER;
					completeFunction(indirection_address);
				}
				goto done;
			}
		}
		done:
		return ret;
	}

	/**
	 * return the number of bytes which could be popped out of the buffer
	 * @return number of bytes would could be popped out of the buffer
	 */
	uint16_t length() {
		return length_to_char(NULL);
	}

	/**
	 * return the number of characters in the buffer up to and including the first instance of a character
	 * the first occurance of *c
	 * @param	c	pointer to a character; null for length of whole buffer
	 */
	uint16_t length_to_char(char * c) {
		uint16_t length = 0;
		uint16_t bufferPeekOffset = 0;
		IndirectionType currently_perusing = currently_consuming;

		if (currently_perusing == VOIDP_WITH_UINT8_LENGTH_AND_COMPLETEFUNCTION) {
			length += indirection_length - indirection_offset;
		} else if(currently_perusing == PGM_P_STRING) {
			uint16_t tmp_offset = indirection_offset;
			while (pgm_read_byte((PGM_P)indirection_address + tmp_offset++)) {
				length++;
			}
		} else if(currently_perusing == NULL_TERMINATED_CHARSTAR_WITH_COMPLETEFUNCTION) {
			uint16_t tmp_offset = indirection_offset;
			while (indirection_address[tmp_offset++]) {
				length++;
			}
		} else if (currently_perusing == VOIDP_WITH_UINT16_LENGTH_AND_COMPLETEFUNCTION) {
			length += indirection_length - indirection_offset;
		} else {
			// hope it was in BUFFER already...
		}
		currently_perusing = BUFFER;

		while (1) {
			if (currently_perusing == BUFFER) {
				int current = CharRingBuffer::peekAtOffset(bufferPeekOffset++);
				if (current == -1) {
					return length;
				}
				if (current != magic_escape_character) {
					length++;
					if (c != NULL &&
						current == *c) {
						return length;
					}
					goto done;
				}
				// OK, so we've hit the magic escape character.
				currently_perusing = (IndirectionType)CharRingBuffer::peekAtOffset(bufferPeekOffset++);
				if (currently_perusing == magic_escape_character) {
					// this was an escaped magic escape character
					currently_perusing = BUFFER;
					length++;
					goto done;
				}
				// otherwise fall through to handle the other types
			}

			// it wasn't an escaped magic character.  Grab all the relevant stuff out of the buffer and then loop around.
			if (currently_perusing == VOIDP_WITH_UINT8_LENGTH_AND_COMPLETEFUNCTION) {
				length += CharRingBuffer::peekAtOffset(bufferPeekOffset++);
				bufferPeekOffset++; // address-high-bits
				bufferPeekOffset++; // address-low-bits
				bufferPeekOffset++; // completefunction-high-bits
				bufferPeekOffset++; // completefunction-low-bits
				currently_perusing = BUFFER;
			} else if(currently_perusing == PGM_P_STRING) {
				char * x = (char*)(CharRingBuffer::peekAtOffset(bufferPeekOffset) << 8 | CharRingBuffer::peekAtOffset(bufferPeekOffset+1));
				bufferPeekOffset++; // address high
				bufferPeekOffset++; // address low
				length += strlen_P(x);
				currently_perusing = BUFFER;
			} else if(currently_perusing == NULL_TERMINATED_CHARSTAR_WITH_COMPLETEFUNCTION) {
				char * x = (char*)(CharRingBuffer::peekAtOffset(bufferPeekOffset) << 8 | CharRingBuffer::peekAtOffset(bufferPeekOffset+1));
				bufferPeekOffset++; // address high
				bufferPeekOffset++; // address low
				bufferPeekOffset++; // completefunction high
				bufferPeekOffset++; // completefunction low
				while (*x) {
					length++;
				}
				currently_perusing = BUFFER;
			} else if (currently_perusing == VOIDP_WITH_UINT16_LENGTH_AND_COMPLETEFUNCTION) {
				length += CharRingBuffer::peekAtOffset(bufferPeekOffset) << 8 | CharRingBuffer::peekAtOffset(bufferPeekOffset+1);
				bufferPeekOffset++; // address-high-bits
				bufferPeekOffset++; // address-low-bits
				bufferPeekOffset++; // completefunction high
				bufferPeekOffset++; // completefunction low
				currently_perusing = BUFFER;
			} else {
				// die horribly!
			}
		done:
			while(0);
		}
		return length; // this should never be reached
	}

	/**
	 * Pop some stuff out of the ring buffer
	 * @param p			where to write the block
	 * @param pLength	the length of the block
	 * @return false if we succeeded, true otherwise
	 */
	bool consume(const void *p, const uint16_t pLength) {
		if (length() < pLength) {
			return failure();
		}
		for(uint16_t i=0;i<pLength;i++) {
			((char*)p)[i] = consume();
		}
		return success();
	}
};

/**
 * Am indirecting ring buffer
 * @tparam	bufferSize	the size of the ringbuffer
 */
template<uint16_t buffersize = 100>
class IndirectingRingBufferImplementation: public IndirectingRingBuffer {
	volatile uint8_t xBuffer[buffersize];
public:

	IndirectingRingBufferImplementation() :
			IndirectingRingBuffer() {
				currently_consuming = BUFFER;
		_buffer = xBuffer;
		_bufferSize = buffersize;
	}
};
// end class IndirectingRingBufferImplementation

}
;
// end namespace FLAME

#endif // FLAME_INDIRECTINGRINGBUFFER_H
