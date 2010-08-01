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


#include <stdio.h>
#include <avr/sfr_defs.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "MHV_HardwareSerial.h"

// Constructors
MHV_HardwareSerial::MHV_HardwareSerial(MHV_RingBuffer *rxBuffer, MHV_RingBuffer *txBuffer,
		volatile uint16_t *ubrr, volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
		volatile uint8_t *udr, uint8_t rxen, uint8_t txen, uint8_t rxcie,
		uint8_t txcie, uint8_t udre, uint8_t u2x, unsigned long baud) {
	_rxBuffer = rxBuffer;
	_txPointers = txBuffer;

	_echo = false;
	_ubrr = ubrr;
	_ucsra = ucsra;
	_ucsrb = ucsrb;
	_udr = udr;
	_rxen = rxen;
	_txen = txen;
	_rxcie = rxcie;
	_txcie = txcie;
	_udre = udre;
	_u2x = u2x;
	_tx = NULL;

	setSpeed(baud);
}

// RX interrupt handler
void MHV_HardwareSerial::rx() {
	char c = *_udr;
	_rxBuffer->append(c);

	if (_echo && ((*_ucsra) & (1 << _udre))) {
		*_udr = c;
	}
}

// Done with the current TX buffer
void MHV_HardwareSerial::txDone() {
//	if (NULL != _currentTx.completeFunction) {
//		_currentTx.completeFunction(_currentTx.data);
//	}

	if (_txPointers->consume(&_currentTx, sizeof(_currentTx))) {
// Nothing more to send, disable the TX interrupt
		_tx = NULL;
		*_ucsrb &= ~_BV( _txcie);
		return;
	}

	_tx = _currentTx.data;
	tx();
}

// TX interrupt handler
void MHV_HardwareSerial::tx() {
	char c;
	if (NULL == _tx) {
		return;
	}

	if (_currentTx.progmem) {
		c = pgm_read_byte(_tx);
	} else {
		c = *_tx;
	}

	if (_currentTx.isString) {
		// sending a null terminated string
		if ('\0' == c) {
			txDone();
			return;
		}
	} else {
		// sending a specific length buffer
		if (_tx == _currentTx.data + _currentTx.length) {
			txDone();
			return;
		}
	}
	*_udr = c;
	_tx++;
}

/* Start sending async data if we aren't already
 */
void MHV_HardwareSerial::asyncStart() {
	if (NULL != _tx) {
		return;
	}

	if (_txPointers->consume(&_currentTx, sizeof(_currentTx))) {
		return;
	}

	// Enable tx interrupt
	*_ucsrb |= _BV( _txcie);

	_tx = _currentTx.data;

	// If the UART isn't already sending data, start sending
	if (((*_ucsra) & (1 << _udre))) {
		tx();
	}
}


void MHV_HardwareSerial::setSpeed(unsigned long baud) {
/* Use U2X if the requested baud rate is higher than (F_CPU/16),
 * otherwise use whatever has the least error
 */
	if (baud > F_CPU / 16 ||
			abs((int)(255-((F_CPU/(8*(((F_CPU/4/baud-1)/2)+1))*255)/baud))) < abs((int)(255-((F_CPU/(16*(((F_CPU/8/baud-1)/2)+1))*255)/baud)))) {
		*_ucsra = _BV(_u2x);
		*_ubrr = (uint16_t)(F_CPU / 4 / baud - 1) / 2;
	} else {
		*_ucsra = 0;
		*_ubrr = (uint16_t)(F_CPU / 8 / baud - 1) / 2;
	}

	*_ucsrb |= _BV( _rxen) | _BV( _txen) | _BV( _rxcie);
}

/* If we have a line on the serial port, copy it into a buffer & null terminate, stripping CR/LF
 * returns 0 if we have successfully copied a line
 * returns -1 if there was no line available
 * returns -2 if the buffer was too small
 * returns -3 if we have reached the end of the ringbuffer with no line terminator
 */
int MHV_HardwareSerial::asyncReadLine(char *buffer, uint8_t bufferLength) {
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

/* If we have a line on the serial port, copy it into a buffer & null terminate, stripping CR/LF
 * Blocks until a line is available
 * returns 0 if we have successfully copied a line
 * returns -2 if the buffer was too small
 * returns -3 if we have reached the end of the ringbuffer with no line terminator
 */
int MHV_HardwareSerial::busyReadLine(char *buffer, uint8_t bufferLength) {
	int rc;
	while (-1 == (rc = asyncReadLine(buffer, bufferLength))) {}
	return rc;
}

int MHV_HardwareSerial::read(void) {
	return _rxBuffer->consume();
}

void MHV_HardwareSerial::flush() {
	_rxBuffer->flush();
}

void MHV_HardwareSerial::end() {
	*_ucsrb &= ~_BV( _rxen) & ~_BV( _txen) & ~_BV( _rxcie) & ~_BV( _txcie);
}

/* Enable echoing data received by us back to the sender (useful for terminal
 * interaction
 */
void MHV_HardwareSerial::echo(bool echoOn) {
	_echo = echoOn;
}

/* Are we ready to asynchronously send another buffer?
 */
bool MHV_HardwareSerial::canSend() {
	return !(_txPointers->full(sizeof(MHV_SERIAL_BUFFER)));
}

/* Can we send something via busywrite
 */
bool MHV_HardwareSerial::canSendBusy() {
	return ((NULL == _tx) && ((*_ucsra) & (1 << _udre)));
}

void MHV_HardwareSerial::busyWrite(char c) {
	while (!canSendBusy()) {};
	*_ucsrb &= ~_BV( _txcie);

	while (!((*_ucsra) & (1 << _udre))) {}

	*_udr = c;
}

/* Write a null terminated progmem string to the serial port
 */
void MHV_HardwareSerial::busyWrite_P(PGM_P buffer) {
	const char *p;

	while (!canSendBusy()) {};
	*_ucsrb &= ~_BV( _txcie);

	p = buffer;
	char c = pgm_read_byte(p++);
	while (c != '\0') {
		while (!((*_ucsra) & (1 << _udre))) {}

		*_udr = c;
		c = pgm_read_byte(p++);
	}
}


/* Write a null terminated string to the serial port
 */
void MHV_HardwareSerial::busyWrite(const char *buffer) {
	const char *p;

	while (!canSendBusy()) {};
	*_ucsrb &= ~_BV( _txcie);

	for (p = buffer; *p != '\0';) {
		while (!((*_ucsra) & (1 << _udre))) {}

		*_udr = *(p++);
	}
}

void MHV_HardwareSerial::busyWrite_P(PGM_P buffer, uint16_t length) {
	uint16_t i;

	while (!canSendBusy()) {};
	*_ucsrb &= ~_BV( _txcie);

	for (i = 0; i < length; i++) {
		/* Don't need to check return values as we have already checked up front
		 * and async writes can't be initiated until we're done
		 */
		busyWrite(pgm_read_byte(buffer + i));
	}
}

void MHV_HardwareSerial::busyWrite(const char *buffer, uint16_t length) {
	uint16_t i;

	while (!canSendBusy()) {};
	*_ucsrb &= ~_BV( _txcie);

	for (i = 0; i < length; i++) {
		/* Don't need to check return values as we have already checked up front
		 * and async writes can't be initiated until we're done
		 */
		busyWrite(buffer[i]);
	}
}

/* Check if the hardware is busy - note that this should not be used to
 * determine if you can actually write - use canSend instead
 */
bool MHV_HardwareSerial::busy(void) {
	return !((*_ucsra) & (1 << _udre));
}

/* Check if there is any received data available
 * @return the number of bytes available
 */
uint8_t MHV_HardwareSerial::available(void) {
	return _rxBuffer->length();
}

/* Write a progmem string asynchronously
 * @return false on success, true on failure
 */
bool MHV_HardwareSerial::asyncWrite_P(PGM_P buffer) {
	MHV_SERIAL_BUFFER buf;

	if (_txPointers->full(sizeof(buf))) {
		return true;
	}

	buf.data = buffer;
	buf.length = 0;
//	buf.completeFunction = NULL;
	buf.progmem = true;
	buf.isString = true;

	_txPointers->append(&buf, sizeof(buf));
	asyncStart();

	return false;
}

/* Write a string asynchronously
 * @return false on success, true on failure
 */
bool MHV_HardwareSerial::asyncWrite(const char *buffer) {
	MHV_SERIAL_BUFFER buf;

	if (_txPointers->full(sizeof(buf))) {
		return true;
	}

	buf.data = buffer;
	buf.length = 0;
//	buf.completeFunction = NULL;
	buf.progmem = false;
	buf.isString = true;

	_txPointers->append(&buf, sizeof(buf));
	asyncStart();

	return false;
}

/* Write a buffer asynchronously
 * Returns 	0 on success
 * 			1 if there is already a string being sent
 */
bool MHV_HardwareSerial::asyncWrite_P(PGM_P buffer, uint16_t length) {
	MHV_SERIAL_BUFFER buf;

	if (_txPointers->full(sizeof(buf))) {
		return true;
	}

	buf.data = buffer;
	buf.length = length;
//	buf.completeFunction = NULL;
	buf.progmem = true;
	buf.isString = false;

	_txPointers->append(&buf, sizeof(buf));
	asyncStart();

	return false;
}

/* Write a buffer asynchronously
 * Returns 	0 on success
 * 			1 if there is already a string being sent
 */
bool MHV_HardwareSerial::asyncWrite(const char *buffer, uint16_t length) {
	MHV_SERIAL_BUFFER buf;

	if (_txPointers->full(sizeof(buf))) {
		return true;
	}

	buf.data = buffer;
	buf.length = length;
//	buf.completeFunction = NULL;
	buf.progmem = false;
	buf.isString = false;

	_txPointers->append(&buf, sizeof(buf));
	asyncStart();

	return false;
}
