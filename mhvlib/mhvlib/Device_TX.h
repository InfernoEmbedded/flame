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
#include <mhvlib/RingBuffer.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

#include <util/delay.h>

#define MHV_DEBUG(__dbg_tx, __dbg_format, __dbg_args...) \
do {\
	__dbg_tx.debug(__FILE__, __LINE__, __FUNCTION__, PSTR(__dbg_format), ## __dbg_args); \
} while (0)

namespace mhvlib {

enum AddressType {
	NORMAL,
#if 0 && defined __FLASH
	FLASH,
#endif
#if 0 && defined __MEMX
	MEMX,
#endif
	PROG_MEM
};

class TXBuffer {
protected:
	volatile void			*_data;
	volatile uint16_t 		_offset;
	volatile uint16_t		_length;
	void					(*_completeFunction)(const char *);
	volatile AddressType	_type;
	volatile bool			_isString;

public:
	/**
	 * Create an empty buffer
	 * @param string	the string to stick in the buffer
	 */
	TXBuffer() :
		_data(NULL),
		_offset(0),
		_length(0),
		_completeFunction(NULL),
		_type(NORMAL),
		_isString(false) {}

	/**
	 * Create a buffer holding a string
	 * @param string	the string to stick in the buffer
	 */
	TXBuffer(const char *string) :
		_data((volatile char *)string),
		_offset(0),
		_length(0),
		_completeFunction(NULL),
		_type(NORMAL),
		_isString(true) {}

	/**
	 * Create a buffer holding a string
	 * @param string			the string to stick in the buffer
	 * @param completeFunction	a function to call when we are done with the string, will be passed the string
	 */
	TXBuffer(const char *string, void (*completeFunction)(const char *)) :
		_data((volatile char *)string),
		_offset(0),
		_length(0),
		_completeFunction(completeFunction),
		_type(NORMAL),
		_isString(true) {}

	/**
	 * Create a buffer holding a progmem string
	 * @param string	the string to stick in the buffer
	 * @param ignored	provided only to distinguish against a memory string
	 */
	TXBuffer(PGM_P string, bool ignored UNUSED) :
		_data((volatile char *)string),
		_offset(0),
		_length(0),
		_completeFunction(NULL),
		_type(PROG_MEM),
		_isString(true) {}

#ifdef UNSUPPORTED
#ifdef __FLASH
	/**
	 * Create a buffer holding a flash string
	 * @param string
	 */
	TXBuffer(const __flash char *string) :
		_data(string),
		_offset(0),
		_length(0),
		_completeFunction(NULL),
		_type(FLASH),
		_isString(true) {}
#endif

#ifdef __MEMX
	/**
	 * Create a buffer holding a memx string
	 * @param string
	 */
	TXBuffer(const __memx char *string) :
		_data(string),
		_offset(0),
		_length(0),
		_completeFunction(NULL),
		_type(MEMX),
		_isString(true) {}
#endif
#endif

	/**
	 * Create a buffer holding a block of memory
	 * @param buffer	the buffer
	 * @param length	the length of the buffer
	 */
	TXBuffer(const char *buffer, uint16_t length) :
		_data((volatile char *)buffer),
		_offset(0),
		_length(length),
		_completeFunction(NULL),
		_type(NORMAL),
		_isString(false) {}

	/**
	 * Create a buffer holding a block of memory
	 * @param buffer	the buffer
	 * @param length	the length of the buffer
	 * @param completeFunction	a function to call when we are done with the string, will be passed the buffer
	 */
	TXBuffer(const char *buffer, uint16_t length, void (*completeFunction)(const char *)) :
		_data((volatile char *)buffer),
		_offset(0),
		_length(length),
		_completeFunction(completeFunction),
		_type(NORMAL),
		_isString(false) {}


	/**
	 * Create a buffer holding a progmem block of memory
	 * @param buffer	the buffer
	 * @param length	the length of the buffer
	 * @param ignored	provided only to distinguish against a memory string
	 */
	TXBuffer(PGM_P buffer, uint16_t length, bool ignored UNUSED) :
		_data((volatile char *)buffer),
		_offset(0),
		_length(length),
		_completeFunction(NULL),
		_type(PROG_MEM),
		_isString(false) {}

#ifdef UNSUPPORTED
#ifdef __FLASH
	/**
	 * Create a buffer holding a flash block of memory
	 * @param buffer	the buffer
	 * @param length	the length of the buffer
	 */
	TXBuffer(const __flash char *buffer buffer, uint16_t length) :
		_data(buffer),
		_offset(0),
		_length(length),
		_completeFunction(NULL),
		_type(FLASH),
		_isString(false) {}
#endif

#ifdef __MEMX
	/**
	 * Create a buffer holding a flash block of memory
	 * @param buffer	the buffer
	 * @param length	the length of the buffer
	 */
	TXBuffer(const __memx char *buffer buffer, uint16_t length) :
		_data(buffer),
		_offset(0),
		_length(length),
		_completeFunction(NULL),
		_type(MEMX),
		_isString(false) {}
#endif
#endif


	/**
	 * Get the position in the current buffer
	 * @return the current position
	 */
	uint16_t getPosition() {
		return _offset;
	}


	/**
	 * Seek the position in the current buffer
	 * @param	position	the desired position
	 */
	void seek(uint16_t position) {
		if (position > _length - 1) {
			return;
		}

		_offset = position;
	}


	/**
	 * Get the character at an offset
	 * @param	offset	the offset
	 * @return the character at the offset, or -1 if there is none
	 */
	int peek(uint16_t offset) {
		char c = '\0';

		if (!_isString && offset >= (_length)) {
			return -1;
		}

		switch (_type) {
			case NORMAL:
				c = *((const char *)_data + offset);
				break;
			case PROG_MEM:
				c = pgm_read_byte((PGM_P)_data + offset);
				break;
#ifdef UNSUPPORTED
#ifdef __FLASH
			case FLASH:
				const __flash char *flash = _data + offset;
				c = *flash;
				break;
#endif
#ifdef __MEMX
			case MEMX:
				const __memx char *memx = _data + offset;
				c = *memx;
				break;
#endif
#endif
		}

		if ((_isString && '\0' == c)) {
			return -1;
		}

		return (int)c;

	}

	/**
	 * Get a character to transmit
	 * @return the character, or -1 if there is nothing left
	 */
	int nextCharacter() {
		return peek(_offset++);
	}


	/**
	 * Called when the buffer is no longer needed
	 */
	void discard() {
		if (NULL != _completeFunction) {
			_completeFunction((const char *)_data);
		}

		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			_offset = 0;
			_length = 0;
			_completeFunction = NULL;
			_isString = false;
		}
	}

	/**
	 * Does this buffer have more characters?
	 * @return true if there are more characters
	 */
	bool hasMore() {
		if (NULL == _data) {
			return false;
		}
		return -1 != peek(_offset);
	}

	/**
	 * Dump the current state into a buffer for debugging
	 * @param	buf			the buffer to write to
	 * @param	bufLength	the space available in the buffer
	 * @param	func		the name of the caller
	 */
	void dumpState(char *buf, uint8_t bufLength, const char *func) {
		int c = peek(_offset);
		snprintf(buf, bufLength, "\r\n%s: Data=%p offset=%i length=%i completeFunc=%p type=%s isString=%i char=%d  (%c)\r\n",
				func, _data, _offset, _length, _completeFunction,
				(_type == PROG_MEM ? "PROG_MEM" :
						(_type == NORMAL ? "NORMAL" : "UNKNOWN")),
				_isString, c, ((c >= 32 && c < 127) ? c : '?'));
	}
};

void device_tx_free(const char *buf);

/**
 * A device that can transmit data
 * @tparam	txPointers	the number of available output buffers for non-blocking I/O
 */
class Device_TX {
protected:
	TXBuffer		_currentTx;
	RingBuffer		&_txPointers;

	/**
	 * Constructor
	 * @param	txPointers	a ringbuffer to store TX elements in
	 */
	Device_TX(RingBuffer &txPointers) :
			_txPointers(txPointers) {}

	virtual void runTxBuffers()=0;

	/**
	 * Called when a buffer has been processed
	 * @return true if there is another buffer to process
	 */
	bool moreTX() {
		_currentTx.discard();

		if (_txPointers.consume(&_currentTx, sizeof(_currentTx))) {
// no more buffers
			return false;
		}

		return true;
	}

	/**
	 * Get the next character
	 * @return the next character, or -1 if there is no more
	 */
	int nextCharacter() {
		int c = _currentTx.nextCharacter();
		while (-1 == c) {
			if (!moreTX()) {
				return -1;
			}

			c = _currentTx.nextCharacter();
		}

		return c;
	}

	//	virtual ~MHV_Device_TX();

public:
	/**
	 * Can we accept another buffer?
	 */
	bool canWrite() {
		return !(_txPointers.full(sizeof(TXBuffer)));
	}

	/**
	 * Write a progmem string asynchronously
	 * @param	string	the progmem string
	 * @return 	false on success
	 * 			true if there is already a string being sent
	 */
	bool write_P(PGM_P string) {
		TXBuffer buf(string, false);

		if (_txPointers.full(sizeof(buf))) {
			return true;
		}

		_txPointers.append(&buf, sizeof(buf));

		if (!_currentTx.hasMore()) {
			runTxBuffers();
		}

		return false;
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
		TXBuffer buf(string);

		if (_txPointers.full(sizeof(buf))) {
			return true;
		}

		_txPointers.append(&buf, sizeof(buf));

		if (!_currentTx.hasMore()) {
			runTxBuffers();
		}

		return false;
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
		TXBuffer buf(string);

		if (_txPointers.full(sizeof(buf))) {
			return true;
		}

		_txPointers.append(&buf, sizeof(buf));

		if (!_currentTx.hasMore()) {
			runTxBuffers();
		}

		return false;
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
		TXBuffer buf(string);

		if (_txPointers.full(sizeof(buf))) {
			return true;
		}

		_txPointers.append(&buf, sizeof(buf));

		if (!_currentTx.hasMore()) {
			runTxBuffers();
		}

		return false;
	}

	/**
	 * Write a string asynchronously
	 * @param	string				the string
	 * @param	completeFunction	a function to call when the string has been written (the string is passed as a parameter)
	 * @return 	false on success
	 * 			true if there is already a string being sent
	 */
	bool write(const char *string, void (*completeFunction)(const char *)) {
		TXBuffer buf(string, completeFunction);

		if (_txPointers.full(sizeof(buf))) {
			return true;
		}

		_txPointers.append(&buf, sizeof(buf));

		if (!_currentTx.hasMore()) {
			runTxBuffers();
		}

		return false;
	}

	/**
	 * Write a buffer asynchronously
	 * @param	buffer	the buffer
	 * @param	length	the length of the buffer
	 * @return 	false on success
	 * 			true if there is already a string being sent
	 */
	bool write_P(PGM_P buffer, uint16_t length) {
		TXBuffer buf(buffer, length, false);

		if (_txPointers.full(sizeof(buf))) {
			return true;
		}

		_txPointers.append(&buf, sizeof(buf));

		if (!_currentTx.hasMore()) {
			runTxBuffers();
		}

		return false;
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
		TXBuffer buf(buffer, length);

		if (_txPointers.full(sizeof(buf))) {
			return true;
		}

		_txPointers.append(&buf, sizeof(buf));

		if (!_currentTx.hasMore()) {
			runTxBuffers();
		}

		return false;
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
		TXBuffer buf(buffer, length);

		if (_txPointers.full(sizeof(buf))) {
			return true;
		}

		_txPointers.append(&buf, sizeof(buf));

		if (!_currentTx.hasMore()) {
			runTxBuffers();
		}

		return false;
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
		TXBuffer buf(buffer, length);

		if (_txPointers.full(sizeof(buf))) {
			return true;
		}

		_txPointers.append(&buf, sizeof(buf));

		if (!_currentTx.hasMore()) {
			runTxBuffers();
		}

		return false;
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
		TXBuffer buf(buffer, length, completeFunction);

		if (_txPointers.full(sizeof(buf))) {
			return true;
		}

		_txPointers.append(&buf, sizeof(buf));

		if (!_currentTx.hasMore()) {
			runTxBuffers();
		}

		return false;
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
		va_list	ap;
		va_start(ap, format);

		int length = vsnprintf_P(NULL, 0, format, ap);
		length++; // "\0"

		char *buf;
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			buf = (char *)malloc(length);
		}

		if (NULL != buf) {
			vsnprintf_P(buf, length, format, ap);

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
};


template<uint8_t txCount>
class Device_TXImplementation : public Device_TX {
protected:
	RingBufferImplementation<txCount * sizeof(TXBuffer)>
					_myTxPointers;

	/**
	 * Constructor
	 */
	Device_TXImplementation() :
		Device_TX(_myTxPointers) {}

};

}
#endif
