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


#ifndef MHV_TXBUFFER_H
#define MHV_TXBUFFER_H

#define MHVLIB_NEED_PURE_VIRTUAL

#include <inttypes.h>
#include <avr/interrupt.h>
#include <MHV_io.h>
#include <stdio.h>
#include <MHV_RingBuffer.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

#define MHV_DEBUG(__dbg_tx, __dbg_format, __dbg_args...) \
do {\
	__dbg_tx.debug(__FILE__, __LINE__, __FUNCTION__, PSTR(__dbg_format), ## __dbg_args); \
} while (0)

struct mhv_tx_buffer {
	const char	*data;
	uint16_t	length;
	void		(*completeFunction)(const char *);
	bool		progmem;
	bool		isString;
};
typedef struct mhv_tx_buffer MHV_TX_BUFFER;

void mhv_device_tx_free(const char *buf);

/**
 * A device that can transmit data
 * @tparam	txPointers	the number of available output buffers for non-blocking I/O
 */
class MHV_Device_TX {
protected:
	MHV_TX_BUFFER				_currentTx;
	const char					*_tx;
	MHV_RingBuffer				&_txPointers;

	/**
	 * Constructor
	 * @param	txPointers	a ringbuffer to store TX elements in
	 */
	MHV_Device_TX(MHV_RingBuffer &txPointers) :
			_txPointers(txPointers) {
		_currentTx.data = NULL;
		_currentTx.length = 0;
		_currentTx.completeFunction = NULL;
		_currentTx.progmem = false;
		_currentTx.isString = false;
	}

	virtual void runTxBuffers()=0;

	/**
	 * Called when a buffer has been processed
	 * @return true if there is another buffer to process
	 */
	bool moreTX() {
	// Call the completeFunction, if one is provided - allows the user to free memory, etc
		if (NULL != _currentTx.completeFunction) {
			_currentTx.completeFunction(_currentTx.data);
		}

		if (_txPointers.consume(&_currentTx, sizeof(_currentTx))) {
			_tx = NULL;
			return false;
		}

		_tx = _currentTx.data;

		return true;
	}

	/**
	 * Called by children to get a character to transmit
	 * @return the character, or -1 if there is nothing left
	 */
	int nextCharacter() {
		char c = '\0';

		if (NULL != _tx) {
			if (_currentTx.progmem) {
				c = pgm_read_byte(_tx);
			} else {
				c = *_tx;
			}
		}

		if ((_currentTx.isString && ('\0' == c)) ||
				(!_currentTx.isString && (_tx == _currentTx.data + _currentTx.length))) {
			if (!moreTX()) {
				_tx = NULL;
				_currentTx.data = NULL;
				_currentTx.length = 0;
				_currentTx.completeFunction = NULL;
				_currentTx.progmem = false;
				_currentTx.isString = false;
				return -1;
			}

			return nextCharacter();
		}

		_tx++;
		return (int)c;
	}

	//	virtual ~MHV_Device_TX();

public:
	/**
	 * Can we accept another buffer?
	 */
	bool canWrite() {
		return !(_txPointers.full(sizeof(MHV_TX_BUFFER)));
	}

	/**
	 * Write a progmem string asynchronously
	 * @param	buffer	the progmem string
	 * @return 	false on success
	 * 			true if there is already a string being sent
	 */
	bool write_P(PGM_P buffer) {
		MHV_TX_BUFFER buf;

		if (_txPointers.full(sizeof(buf))) {
			return true;
		}

		buf.data = buffer;
		buf.length = 0;
		buf.completeFunction = NULL;
		buf.progmem = true;
		buf.isString = true;

		_txPointers.append(&buf, sizeof(buf));
		if (!_tx) {
			runTxBuffers();
		}

		return false;
	}

	/**
	 * Write a string asynchronously
	 * @param	buffer	the string
	 * @return 	false on success
	 * 			true if there is already a string being sent
	 */
	bool write(const char *buffer) {
		MHV_TX_BUFFER buf;

		if (_txPointers.full(sizeof(buf))) {
			return true;
		}

		buf.data = buffer;
		buf.length = 0;
		buf.completeFunction = NULL;
		buf.progmem = false;
		buf.isString = true;

		_txPointers.append(&buf, sizeof(buf));
		if (!_tx) {
			runTxBuffers();
		}

		return false;
	}

	/**
	 * Write a string asynchronously
	 * @param	buffer				the string
	 * @param	completeFunction	a function to call when the string has been written (the string is passed as a parameter)
	 * @return 	false on success
	 * 			true if there is already a string being sent
	 */
	bool write(const char *buffer, void (*completeFunction)(const char *)) {
		MHV_TX_BUFFER buf;

		if (_txPointers.full(sizeof(buf))) {
			return true;
		}

		buf.data = buffer;
		buf.length = 0;
		buf.completeFunction = completeFunction;
		buf.progmem = false;
		buf.isString = true;

		_txPointers.append(&buf, sizeof(buf));
		if (!_tx) {
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
		MHV_TX_BUFFER buf;

		if (_txPointers.full(sizeof(buf))) {
			return true;
		}

		buf.data = buffer;
		buf.length = length;
		buf.completeFunction = NULL;
		buf.progmem = true;
		buf.isString = false;

		_txPointers.append(&buf, sizeof(buf));
		if (!_tx) {
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
	bool write(const char *buffer, uint16_t length) {
		MHV_TX_BUFFER buf;

		if (_txPointers.full(sizeof(buf))) {
			return true;
		}

		buf.data = buffer;
		buf.length = length;
		buf.completeFunction = NULL;
		buf.progmem = false;
		buf.isString = false;

		_txPointers.append(&buf, sizeof(buf));
		if (!_tx) {
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
		MHV_TX_BUFFER buf;

		if (_txPointers.full(sizeof(buf))) {
			return true;
		}

		buf.data = buffer;
		buf.length = length;
		buf.completeFunction = completeFunction;
		buf.progmem = false;
		buf.isString = false;

		_txPointers.append(&buf, sizeof(buf));
		if (!_tx) {
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

		int length = snprintf_P(NULL, 0, PSTR("%s:%d\t%s():\t\t"),
				file, line, function);
		length += vsnprintf_P(NULL, 0, format, ap);
		length += 3; // "\r\n\0"

		char *buf = (char *)malloc(length);
		char *cur = buf;
		if (NULL != cur) {
			cur += snprintf_P(cur, length, PSTR("%s:%d\t%s():\t\t"),
						file, line, function);
			length -= cur - buf;
			cur += vsnprintf_P(cur, length, format, ap);
			*(cur++) = '\r';
			*(cur++) = '\n';
			*cur = '\0';

			if (write(buf, length, &mhv_device_tx_free)) {
				mhv_device_tx_free(buf);
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

		char *buf = (char *)malloc(length);
		if (NULL != buf) {
			vsnprintf_P(buf, length, format, ap);

			if (write(buf, length, &mhv_device_tx_free)) {
				mhv_device_tx_free(buf);
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
		char *buf = (char *)malloc(11);
		if (NULL != buf) {
			ultoa(value, buf, 10);
			if (write(buf, &mhv_device_tx_free)) {
				mhv_device_tx_free(buf);
				return true;
			}
			return false;
		}
		return true;
	}
};


template<uint8_t txCount>
class MHV_Device_TXImplementation : public MHV_Device_TX {
protected:
	MHV_RingBufferImplementation<txCount * sizeof(MHV_TX_BUFFER)>
					_myTxPointers;

	/**
	 * Constructor
	 */
	MHV_Device_TXImplementation() :
		MHV_Device_TX(_myTxPointers) {}

};

#endif
