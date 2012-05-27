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


#ifndef MHV_RXBUFFER_H
#define MHV_RXBUFFER_H

#include <inttypes.h>
#include <avr/interrupt.h>
#include <mhvlib/io.h>
#include <stdio.h>
#include <mhvlib/RingBuffer.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>

#define MHV_RX_BUFFER_CREATE(_mhvRxName, _mhvRxCharacterCount) \
	char _mhvRxName ## Buf[_mhvRxCharacterCount + 1]; \
	MHV_RingBuffer _mhvRxName(_mhvRxName ## Buf, _mhvRxCharacterCount + 1);

namespace mhvlib_bsd {

class Device_RX;

/**
 * A listener that will be called when a line is ready or the buffer is full
 */
class RXListener {
public:
	virtual void rxReady(Device_RX *rx) =0;
};


/**
 * A device that can receive data
 */
class Device_RX {
protected:
	RingBuffer		&_rxBuffer;
	RXListener		*_listener;

	/**
	 * Constructor
	 * @param	buffer	A buffer to store received data
	 */
	Device_RX(RingBuffer &buffer) :
			_rxBuffer(buffer),
			_listener(NULL) {}

public:
	/**
	 * If we have a line, copy it into a buffer & null terminate, stripping CR/LF
	 * returns 0 if we have successfully copied a line
	 * returns -1 if there was no line available
	 * returns -2 if the buffer was too small
	 * returns -3 if we have reached the end of the ringbuffer with no line terminator
	 */
	int asyncReadLine(char *buffer, uint8_t bufferLength) {
		// Peek at the last character & see if its a newline
		int last = _rxBuffer.peekHead();

		bool isFull = _rxBuffer.full();
		if (!isFull) {
			if ('\r' != last && '\n' != last) {
				return -1;
			}
		}

		uint8_t i = 0;
		int byte;
		while (-1 != (byte = _rxBuffer.consume())) {
			if (i == bufferLength - 1) {
				buffer[i] = '\0';
				return -2;
			}
			if ('\r' == byte || '\n' == byte) {
				break;
			}
			buffer[i++] = (char)byte;
		}
		buffer[i] = '\0';

		if (isFull) {
			return -3;
		}
		return 0;
	}

	/**
	 * If we have a line, copy it into a buffer & null terminate, stripping CR/LF
	 * Blocks until a line is available
	 * @return 0 if we have successfully copied a line
	 * @return -2 if the buffer was too small
	 * @return -3 if we have reached the end of the ringbuffer with no line terminator
	 */
	int busyReadLine(char *buffer, uint8_t bufferLength) {
		int rc;
		while (-1 == (rc = asyncReadLine(buffer, bufferLength))) {}
		return rc;
	}

	/**
	 * Read a byte from the receive buffer
	 * @return the byte, or -1 if there is nothing to read
	 */
	int read(void) {
		return _rxBuffer.consume();
	}

	/**
	 * Discard remaining data in the receive buffer
	 */
	void flush() {
		_rxBuffer.flush();
	}

	/**
	 * Check if a line is ready, or the ringbuffer is full
	 * @return true if either of the situations occurs
	 */
	bool ready() {
		if (_rxBuffer.full()) {
			return true;
		}

		// Peek at the last character & see if its a newline
		int last = _rxBuffer.peekHead();
		if ('\r' == last || '\n' == last) {
			return true;
		}

		return false;
	}

	/**
	 * Register interest for lines/overflows from an RX device
	 * @param	listener	an MHV_RXListener to notify that the device is ready
	 */
	void registerListener(RXListener &listener) {
		_listener = &listener;
	}

	/**
	 * Deregister interest for lines/overflows from an RX device
	 */
	void deregisterListener() {
		_listener = NULL;
	}


	/**
	 * Call from the main loop to handle any events
	 */
	void handleEvents() {
			if (ready()) {
			_listener->rxReady(this);
		}
	}
};

template<uint8_t bufferLength>
class Device_RXImplementation : public Device_RX {
protected:
	RingBufferImplementation<bufferLength>
					_myBuffer;

	/**
	 * Constructor
	 */
	Device_RXImplementation() :
		Device_RX(_myBuffer) {}
};

}

#endif
