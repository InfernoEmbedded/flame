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


#include <stdio.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <MHV_Device_RX.h>

/**
 * Constructor
 * @param	rxBuffer	a buffer to read into
 */
MHV_Device_RX::MHV_Device_RX(MHV_RingBuffer *rxBuffer) {
	_rxBuffer = rxBuffer;
}

/**
 * If we have a line, copy it into a buffer & null terminate, stripping CR/LF
 * returns 0 if we have successfully copied a line
 * returns -1 if there was no line available
 * returns -2 if the buffer was too small
 * returns -3 if we have reached the end of the ringbuffer with no line terminator
 */
int MHV_Device_RX::asyncReadLine(char *buffer, uint8_t bufferLength) {
	// Peek at the last character & see if its a newline
	int last = _rxBuffer->peekHead();

	bool isFull = _rxBuffer->full();
	if (!isFull) {
		if ('\r' != last && '\n' != last) {
			return -1;
		}
	}

	uint8_t i = 0;
	int byte;
	while (-1 != (byte = _rxBuffer->consume())) {
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
int MHV_Device_RX::busyReadLine(char *buffer, uint8_t bufferLength) {
	int rc;
	while (-1 == (rc = asyncReadLine(buffer, bufferLength))) {}
	return rc;
}

/**
 * Read a byte from the receive buffer
 * @return the byte, or -1 if there is nothing to read
 */
int MHV_Device_RX::read(void) {
	return _rxBuffer->consume();
}

/**
 * Discard remaining data in the receive buffer
 */
void MHV_Device_RX::flush() {
	_rxBuffer->flush();
}
