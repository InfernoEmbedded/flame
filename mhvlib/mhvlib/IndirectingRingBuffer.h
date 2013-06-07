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

#include <mhvlib/CharRingBuffer.h>

namespace mhvlib {

class IndirectingRingBuffer : public CharRingBuffer {

protected:
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
  VOIDP_WITH_UINT16_LENGTH_AND_COMPLETEFUNCTION = 4,
};
public:
	PURE bool success() { return true; };

	/* see if there is room for a character in the buffer:
	 * @param 			the character to append
	 */
	bool canFit(char c) {
		if (c == magic_escape_character) {
			return freeSpace() > 1;
		}
		return freeSpace() > 0;
	}
	/* append a character
	 * @param 			the character to append
	 */
	bool append(char c) {
		bool ret;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			if (!canFit(c)) {
				ret = !success(); // failure
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
	PURE uint16_t escapedLength(const void *p,pLength_t pLength) {
		pLength_t count = 0;
		for (pLength_t i = 0; i < pLength; i++) {
			if (((char*)p)[i] == magic_escape_character) {
				count++;
			}
		}
		return pLength + count;
	}
	
	/* see if there is room for a void pointer in the buffer (directly)
	 * @param p			pointer to the data
	 * @param pLength		amount of data to append
	 */
	template<class pLength_t>
	bool canFit(const void *p, pLength_t pLength) {
		return (escapedLength(p,pLength) > freeSpace());
	}
	/* append a specific length of data pointed to by a void pointer
	 * @param p			pointer to the data
	 * @param pLength		amount of data to append
	 */
	template<class pLength_t>
	bool append(const void *p, pLength_t pLength) { // can't indirect this - may be reused by caller
		bool ret;
		uint16_t escaped_length = escapedLength(p,pLength);
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			if (escaped_length > freeSpace()) {
				ret = !success(); // failure
			} else {
				for (pLength_t i = 0; i < pLength; i++) {
					append(((char*)p)[i]);
					if (((char*)p)[i] == magic_escape_character) {
						append(((char*)p)[i]);
					}
				}
				ret = success(); // success
			}
		}
		return ret;
	}

	PURE uint16_t escapedLength(const char *string) {
		uint16_t count = 0;
		for (char * x = (char*)string;*x;x++) {
			count++;
			if (*x == magic_escape_character) {
				count++;
			}
		}
		return count;
	}
	/* see if there is room for a null-terminated string in the buffer (directly)
	 * @param p			pointer to the data
	 */
	bool canFit(const char *string) {
		return (escapedLength(string) <= freeSpace());
	}
	/* see if there is room for a null-terminated string in the buffer (possibly indirected)
	 * @param p			pointer to the data
	 * @param pLength		amount of data to append
	 */
	bool canFit(const char *string,void (*completeFunction)(const char *) UNUSED) {
		// we require 6 bytes for this....
		if (freeSpace() >= 6) {
			return true;
		}
		return canFit(string);
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
					if (*x == magic_escape_character) {
						append(*x);
					}
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
		uint16_t escaped_length = escapedLength(p,pLength);
		bool ret;
		if (escaped_length > 7) {
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
		uint16_t escaped_length = escapedLength(p,pLength);
		bool ret;
		if (escaped_length > 7) {
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
			completeFunction(p);
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


	/*
	 * Send a null-terminated program string, directly
	 * @param string		pointer to progmem string to send
	 */
	bool appendDirectly_P(PGM_P string) {
		bool ret;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			uint16_t escaped_length = escapedLength_P(string);
			if (escaped_length > freeSpace()) {
				ret = !success(); // failure
			} else {
				PGM_P x = string;
				char c;
				while((c = pgm_read_byte((PGM_P)x++))) {
					append(c);
					if (c == magic_escape_character) {
						append(c);
					}
				}
				ret = success(); // success
			}
		}
		return ret;
	}
	/*
	 * Send a null-terminated program string, indirectly
	 * @param string		pointer to progmem string to send
	 */
	bool appendIndirectly_P(PGM_P string) {
		bool ret;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			if (freeSpace() < 5) {
				ret = !success(); // failure
			} else {
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
					CharRingBuffer::append(magic_escape_character);
					CharRingBuffer::append(PGM_P_STRING);
					CharRingBuffer::append((char)((uint16_t)string >> 8));
					CharRingBuffer::append((char)((uint16_t)string & 0xff));
				}
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
		if (escaped_length < 0) { // directly append it
			ret = appendDirectly_P(string);
		} else {
			ret = appendIndirectly_P(string);
		}
		return ret;
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
					goto done;
				}
				// OK, so we've hit the magic escape character.
				currently_consuming = CharRingBuffer::consume();
				if (currently_consuming == magic_escape_character) {
					// this was an escaped magic escape character
					ret = currently_consuming;
					goto done;
				}
				// it wasn't an escaped magic character.  Grab all the relevant stuff out of the buffer and then loop around.
				if (currently_consuming == VOIDP_WITH_UINT8_LENGTH_AND_COMPLETEFUNCTION) {
					indirection_length = CharRingBuffer::consume();
					indirection_address = (char*)(CharRingBuffer::consume() << 8 |
								      CharRingBuffer::consume());
					indirection_offset = 0;
					completeFunction =  (void (*)(const char*))(CharRingBuffer::consume() << 8 |
						     CharRingBuffer::consume());
				} else if(currently_consuming == PGM_P_STRING) {
					indirection_address = (char*)(CharRingBuffer::consume() << 8 |
								      CharRingBuffer::consume());
					indirection_offset = 0;
				} else if(currently_consuming == NULL_TERMINATED_CHARSTAR_WITH_COMPLETEFUNCTION) {	
					indirection_address = (char*)(CharRingBuffer::consume() << 8 |
								      CharRingBuffer::consume());
					indirection_offset = 0;
					completeFunction =  (void (*)(const char*))(CharRingBuffer::consume() << 8 |
						     CharRingBuffer::consume());
				} else if (currently_consuming == VOIDP_WITH_UINT16_LENGTH_AND_COMPLETEFUNCTION) {
					indirection_length = CharRingBuffer::consume() << 8 |
						CharRingBuffer::consume();
					indirection_address = (char*)(CharRingBuffer::consume() << 8 |
								      CharRingBuffer::consume());
					indirection_offset = 0;
					completeFunction =  (void (*)(const char*))(CharRingBuffer::consume() << 8 |
						     CharRingBuffer::consume());
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
			if (currently_consuming == VOIDP_WITH_UINT16_LENGTH_AND_COMPLETEFUNCTION) {
				if (indirection_length == indirection_offset) {
					currently_consuming = BUFFER;
					completeFunction(indirection_address);
				} else {
					ret = indirection_address[indirection_offset++];
					goto done;
				}
			}
		}
	done:
		return ret;
	}

};

/**
 * Am indirecting ring buffer
 * @tparam	bufferSize	the size of the ringbuffer
 */
template<uint16_t buffersize = 100>
class IndirectingRingBufferImplementation : public IndirectingRingBuffer {
	volatile uint8_t xBuffer[buffersize + 1];
public:

	IndirectingRingBufferImplementation() :	IndirectingRingBuffer() {
		_buffer = xBuffer;
		_bufferSize = buffersize + 1;
	}
}; // end class IndirectingRingBufferImplementation

}; // end namespace MHV

#endif // MHV_INDIRECTINGRINGBUFFER_H
