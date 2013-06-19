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


#ifndef MHV_TXBUFFER_H
#define MHV_TXBUFFER_H

#include <inttypes.h>
#include <avr/interrupt.h>
#include <mhvlib/io.h>
#include <stdio.h>
#include <mhvlib/IndirectingRingBuffer.h>
#include <mhvlib/CharRingBuffer.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

#include <util/delay.h>

#define MHV_DEBUG(__dbg_tx, __dbg_format, __dbg_args...) \
do {\
	__dbg_tx.debug(__FILE__, __LINE__, __FUNCTION__, PSTR(__dbg_format), ## __dbg_args); \
} while (0)

namespace mhvlib {

enum class AddressType {
	NORMAL,
#if 0 && defined __FLASH
	FLASH,
#endif
#if 0 && defined __MEMX
	MEMX,
#endif
	PROG_MEM
};

void device_tx_free(const char *buf);

/**
 * A device that can transmit data
 * @tparam	txPointers	the number of available output buffers for non-blocking I/O
 */
class Device_TX {
protected:
	/**
	 * Constructor
	 * @param	txbuffer	a ringbuffer to store TX elements in
	 */
	Device_TX(IndirectingRingBuffer &txbuffer) :
			_txbuffer(txbuffer) {}

	virtual void runTxBuffers()=0;

	/**
	 * Get the next character
	 * @return the next character, or -1 if there is no more
	 */
	int nextCharacter() {
		int ret;
		ret = _txbuffer.consume();
		return ret;
	}

	//	virtual ~MHV_Device_TX();

public:
	IndirectingRingBuffer		&_txbuffer;

	/**
	 * Can we accept another buffer?
	 * Can't support this - and didn't make sense for malloced-char* stuff anyway! -pb201306021333
	 * *can* support "canWrite(char)", "canWrite(char *)", "canWrite(void*,uint8)" etc etc, 'though...
	 */
	// bool canWrite() {
	// 	return !(_txPointers.full());
	// }


	void flush() {
		_txbuffer.flush();
	}
	uint16_t txQueueLength() {
		return _txbuffer.length();
	}
	uint16_t txQueueSpace() {
		return _txbuffer.freeSpace();
	}

	INLINE uint16_t spaceRequired(const char * x) {
		return _txbuffer.escapedLength(x);
	}
	uint16_t printfSpaceRequired(PGM_P format, va_list ap) {
		uint16_t length = vsnprintf_P(NULL, 0, format, ap);

		char *buf = (char *)malloc(length+1); // +1 for "\0"

		uint16_t ret;
		if (NULL != buf) {
			vsnprintf_P(buf, length+1, format, ap);
			ret = spaceRequired(buf);
			free(buf);
		} else {
			// ?!
		}
		return ret;
	}

	bool canFit(char c) {
		return _txbuffer.canFit(c);
	}
	bool canFit(const char * string) {
		return _txbuffer.canFit(string);
	}
	bool canFit_P(const char * string) {
		return _txbuffer.canFit(string);
	}

	/**
	 * Write a progmem string asynchronously
	 * @param	string	the progmem string
	 * @return 	false on success
	 * 			true if there is already a string being sent
	 */
	bool write_P(PGM_P string) {
		bool ret = _txbuffer.append_P(string);
		if (ret == _txbuffer.success()) {
			return false;
		}
		runTxBuffers();
		ret = _txbuffer.append_P(string);
		if (ret == _txbuffer.success()) {
			return false;
		}
		return true;
	}

#ifdef UNSUPPORTED
#ifdef __FLASH
	/**
	 * Write a flash string asynchronously
	 * @param	string	the string
	 * @return 	false on success
	 * 			true if there is already a string being sent
	 */
	bool write(const __flash char *string) {
		bool ret = _txbuffer.append(string);
		if (ret == _txbuffer.success()) {
			return false;
		}
		runTxBuffers();
		ret = _txbuffer.append(string);
		if (ret == _txbuffer.success()) {
			return false;
		}
		return true;
	}
#endif

#ifdef __MEMX
	/**
	 * Write a memx string asynchronously
	 * @param	string	the string
	 * @return 	false on success
	 * 			true if there is already a string being sent
	 */
	bool write(const __memx char *string) {
		bool ret = _txbuffer.append(string);
		if (ret == _txbuffer.success()) {
			return false;
		}
		runTxBuffers();
		ret = _txbuffer.append(string);
		if (ret == _txbuffer.success()) {
			return false;
		}
		return true;
	}
#endif
#endif

	/**
	 * Write a string asynchronously
	 * @param	string	the string
	 * @return 	false on success
	 * 			true if there is already a string being sent
	 */
	bool write(const char *string) {
		bool ret = _txbuffer.append(string);
		if (ret == _txbuffer.success()) {
			return false;
		}
		runTxBuffers();
		ret = _txbuffer.append(string);
		if (ret == _txbuffer.success()) {
			return false;
		}
		return true;
	}

	/**
	 * Write a string asynchronously
	 * @param	string				the string
	 * @param	completeFunction	a function to call when the string has been written (the string is passed as a parameter)
	 * @return 	false on success
	 * 			true if there is already a string being sent
	 */
	bool write(const char *string, void (*completeFunction)(const char *)) {
		bool ret = _txbuffer.append(string,completeFunction);
		if (ret == _txbuffer.success()) {
			return false;
		}
		runTxBuffers();
		ret = _txbuffer.append(string,completeFunction);
		if (ret == _txbuffer.success()) {
			return false;
		}
		return true;
	}

	/**
	 * Write a buffer asynchronously
	 * @param	buffer	the buffer
	 * @param	length	the length of the buffer
	 * @return 	false on success
	 * 			true if there is already a string being sent
	 */
	bool write_P(PGM_P buffer, uint16_t length) {
		bool ret = _txbuffer.append(buffer,length);
		if (ret == _txbuffer.success()) {
			return false;
		}
		runTxBuffers();
		ret = _txbuffer.append(buffer,length);
		if (ret == _txbuffer.success()) {
			return false;
		}
		return true;
	}

#ifdef UNSUPPORTED
#ifdef __FLASH
	/**
	 * Write a flash buffer asynchronously
	 * @param	buffer	the buffer
	 * @param	length	the length of the buffer
	 * @return 	false on success
	 * 			true if there is already a string being sent
	 */
	bool write(const __flash char *buffer, uint16_t length) {
		bool ret = _txbuffer.append(buffer,length);
		if (ret == _txbuffer.success()) {
			return false;
		}
		runTxBuffers();
		ret = _txbuffer.append(buffer,length);
		if (ret == _txbuffer.success()) {
			return false;
		}
		return true;
	}
#endif

#ifdef __MEMX
	/**
	 * Write a memx buffer asynchronously
	 * @param	buffer	the buffer
	 * @param	length	the length of the buffer
	 * @return 	false on success
	 * 			true if there is already a string being sent
	 */
	bool write(const __memx char *buffer, uint16_t length) {
		bool ret = _txbuffer.append(buffer,length);
		if (ret == _txbuffer.success()) {
			return false;
		}
		runTxBuffers();
		ret = _txbuffer.append(buffer,length);
		if (ret == _txbuffer.success()) {
			return false;
		}
		return true;
	}
#endif
#endif

	/**
	 * Write a buffer asynchronously
	 * @param	buffer	the buffer
	 * @param	length	the length of the buffer
	 * @return 	false on success
	 * 			true if there is already a string being sent
	 */
	bool write(const char *buffer, uint16_t length) {
		bool ret = _txbuffer.append(buffer,length);
		if (ret == _txbuffer.success()) {
			return false;
		}
		runTxBuffers();
		ret = _txbuffer.append(buffer,length);
		if (ret == _txbuffer.success()) {
			return false;
		}
		return true;
	}

	/**
	 * Write a buffer asynchronously
	 * @param	buffer	the buffer
	 * @param	length	the length of the buffer
	 * @param	completeFunction	a function to call when the string has been written (the string is passed as a parameter)
	 * @return 	false on success
	 * 			true if there is already a string being sent
	 */
	bool write(const char *buffer, uint16_t length, void (*completeFunction)(const char *)) {
		bool ret = _txbuffer.append(buffer,length,(void (*)(const void*))completeFunction);
		if (ret == _txbuffer.success()) {
			return false;
		}
		runTxBuffers();
		ret = _txbuffer.append(buffer,length,(void (*)(const void*))completeFunction);
		if (ret == _txbuffer.success()) {
			return false;
		}
		return true;
	}

	/**
	 * Print a debug message
	 * @param	file		the filename
	 * @param	line		the line number
	 * @param	function	the function name
	 * @param	format		a printf format
	 * @param	...			the printf parms
	 * @return 	false on success
	 * 			true if the message was not sent
	 */
	bool debug(const char *file, int line, const char *function,
			PGM_P format, ...) {
		va_list	ap;
		va_start(ap, format);

		PGM_P prefix = PSTR("%s:%d\t%s():\t\t");

		int length = snprintf_P(NULL, 0, prefix,
				file, line, function);
		length += vsnprintf_P(NULL, 0, format, ap);
		length += 3; // "\r\n\0"

		char *buf;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			buf = (char *)malloc(length);
		}
		char *cur = buf;
		if (NULL != cur) {
			cur += snprintf_P(cur, length, prefix,
						file, line, function);
			int lengthRemaining = length - (cur - buf);
			cur += vsnprintf_P(cur, lengthRemaining, format, ap);
			*(cur++) = '\r';
			*(cur++) = '\n';
			*cur = '\0';

			if (write(buf, length, &device_tx_free)) {
				device_tx_free(buf);
				va_end(ap);
				return true;
			}

			va_end(ap);
			return false;
		}

		va_end(ap);
		return true;
	}

	/**
	 * Print a message
	 * @param	format		a printf format
	 * @param	...			the printf parms
	 * @return 	false on success
	 * 			true if there is already a string being sent
	 */
	bool printf(PGM_P format, ...) {
		bool ret;

		va_list	ap;
		va_start(ap, format);

		ret = printf(format,ap);

		va_end(ap);
		return ret;
	}

	/**
	 * Print a message
	 * @param	format		a printf format
	 * @param	...			the printf parms
	 * @return 	false on success
	 * 			true if there is already a string being sent
	 */
	bool printf(PGM_P format, va_list ap) {

		uint16_t length = vsnprintf_P(NULL, 0, format, ap);

		char *buf = (char *)malloc(length+1); // +1 for "\0"

		if (NULL != buf) {
			vsnprintf_P(buf, length+1, format, ap);

			if (write(buf, length, &device_tx_free)) {
				device_tx_free(buf);
				return true;
			}

			return false;
		}

		return true;
	}

//	/**
//	 * Print an integer
//	 * @param	value	the value to print
//	 * @return 	false on success
//	 * 			true if there is already a string being sent
//	 */
//	bool write(uint8_t value) {
//		char *buf = (char *)malloc(4);
//		if (NULL != buf) {
//			utoa((uint16_t)value, buf, 10);
//			if (write(buf, &mhv_device_tx_free)) {
//				mhv_device_tx_free(buf);
//				return true;
//			}
//			return false;
//		}
//		return true;
//	}
//
//	/**
//	 * Print an integer
//	 * @param	value	the value to print
//	 * @return 	false on success
//	 * 			true if there is already a string being sent
//	 */
//	bool write(uint16_t value) {
//		char *buf = (char *)malloc(6);
//		if (NULL != buf) {
//			utoa(value, buf, 10);
//			if (write(buf, &mhv_device_tx_free)) {
//				mhv_device_tx_free(buf);
//				return true;
//			}
//			return false;
//		}
//		return true;
//	}

	/**
	 * Print an integer
	 * @param	value	the value to print
	 * @return 	false on success
	 * 			true if there is already a string being sent
	 */
	bool write(uint32_t value) {
		//                        4294967296 + \0
		char *buf = (char *)malloc(11);
		if (NULL != buf) {
			ultoa(value, buf, 10);
			if (write(buf, &device_tx_free)) {
				device_tx_free(buf);
				return true;
			}
			return false;
		}
		return true;
	}
	bool write(char value) {
		bool ret = _txbuffer.append(value);
		if (ret == _txbuffer.success()) {
			return false; // success
		}
		runTxBuffers();
		ret = _txbuffer.append(value);
		if (ret == _txbuffer.success()) {
			return false; // success
		}
		return true; // failure
	}
};


template<uint8_t txCount>
class Device_TXImplementation : public Device_TX {
protected:
 public:
	IndirectingRingBufferImplementation<txCount> _txbuffer;

	/**
	 * Constructor
	 */
	Device_TXImplementation() :
		Device_TX(_txbuffer) {}

};

}
#endif
