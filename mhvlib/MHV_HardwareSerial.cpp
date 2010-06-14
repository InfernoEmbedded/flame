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
MHV_HardwareSerial::MHV_HardwareSerial(MHV_RingBuffer *rxBuffer,
		volatile uint16_t *ubrr, volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
		volatile uint8_t *udr, uint8_t rxen, uint8_t txen, uint8_t rxcie,
		uint8_t txcie, uint8_t udre, uint8_t u2x, unsigned long baud) {
	_rxBuffer = rxBuffer;
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

	_txBuffer = NULL;

	setSpeed(baud);
}

#define DEBUG 0

#if DEBUG
bool rxToggle = false;
#endif

// RX interrupt handler
void MHV_HardwareSerial::rx() {
	char c = *_udr;
#if DEBUG
	if (rxToggle) {
		mhv_pinOn(MHV_ARDUINO_PIN_13);
	} else {
		mhv_pinOff(MHV_ARDUINO_PIN_13);
	}
	rxToggle = !rxToggle;
#endif
	_rxBuffer->append(c);
	if (_echo && ((*_ucsra) & (1 << _udre))) {
		*_udr = c;
	}
}

// TX interrupt handler
void MHV_HardwareSerial::tx() {
	char c;
	if (NULL == _txBuffer) {
		return;
	}

	if (_txBufferProgmem) {
		c = pgm_read_byte(_txBuffer);
	} else {
		c = *_txBuffer;
	}
	if (MHV_SENDING_STRING == _txBufferLength) {
		// sending a null terminated string
		if ('\0' == c) {
			_txBuffer = NULL;
			return;
		}
	} else {
		// sending a specific length buffer
		if (0 == _txBufferLength--) {
			_txBuffer = NULL;
			return;
		}
	}
	*_udr = c;
	_txBuffer++;
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
 */
int MHV_HardwareSerial::readLine(char *buffer, uint8_t bufferLength) {
	// Peek at the last character & see if its a newline
	int last = _rxBuffer->peekHead();

	if ('\r' != last && '\n' != last) {
		return -1;
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
	return 0;
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

void MHV_HardwareSerial::echo(bool echoOn) {
	_echo = echoOn;
}

/* Are we ready to asynchronously send another buffer?
 */
bool MHV_HardwareSerial::canSend() {
	return _txBuffer == NULL;
}

uint8_t MHV_HardwareSerial::busyWrite(char c) {
	if (!canSend()) {
		return 1;
	}
	*_ucsrb &= ~_BV( _txcie);

	while (!((*_ucsra) & (1 << _udre))) {}

	*_udr = c;
	return 0;
}

/* Write a null terminated progmem string to the serial port
 */
uint8_t MHV_HardwareSerial::busyWrite_P(const char *buffer) {
	const char *p;

	if (!canSend()) {
		return 1;
	}
	*_ucsrb &= ~_BV( _txcie);

	p = buffer;
	char c = pgm_read_byte(p++);
	while (c != '\0') {
		while (!((*_ucsra) & (1 << _udre))) {}

		*_udr = c;
		c = pgm_read_byte(p++);
	}
	return 0;
}


/* Write a null terminated string to the serial port
 */
uint8_t MHV_HardwareSerial::busyWrite(const char *buffer) {
	const char *p;

	if (!canSend()) {
		return 1;
	}
	*_ucsrb &= ~_BV( _txcie);

	for (p = buffer; *p != '\0';) {
		while (!((*_ucsra) & (1 << _udre))) {}

		*_udr = *(p++);
	}
	return 0;
}

uint8_t MHV_HardwareSerial::busyWrite_P(const char *buffer, uint16_t length) {
	uint16_t i;

	if (!canSend()) {
		return 1;
	}

	for (i = 0; i < length; i++) {
		/* Don't need to check return values as we have already checked up front
		 * and async writes can't be initiated until we're done
		 */
		busyWrite(pgm_read_byte(buffer + i));
	}
	return 0;
}

uint8_t MHV_HardwareSerial::busyWrite(const char *buffer, uint16_t length) {
	uint16_t i;

	if (!canSend()) {
		return 1;
	}
	*_ucsrb &= ~_BV( _txcie);

	for (i = 0; i < length; i++) {
		/* Don't need to check return values as we have already checked up front
		 * and async writes can't be initiated until we're done
		 */
		busyWrite(buffer[i]);
	}
	return 0;
}

/* Are we ready to asynchronously send another buffer?
 */
bool MHV_HardwareSerial::busy(void) {
	return !((*_ucsra) & (1 << _udre));
}

uint8_t MHV_HardwareSerial::available(void) {
	return _rxBuffer->length();
}

/* Write a progmem string asynchronously
 * Returns 	0 on success
 * 			1 if there is already a string being sent
 */
uint8_t MHV_HardwareSerial::asyncWrite_P(const char *buffer) {
	if (!canSend()) {
		return 1;
	}
	_txBufferLength = MHV_SENDING_STRING;
	_txBufferProgmem = true;
	*_ucsrb |= _BV( _txcie);

	// Start writing
	char c = pgm_read_byte(buffer);
	if ('\0' == c) {
		_txBuffer = NULL;
		return 0;
	} else if (((*_ucsra) & (1 << _udre))) {
		_txBuffer = buffer + 1;
		*_udr = c;
	} else {
		// Already busy sending a byte
		_txBuffer = buffer;
	}

	return 0;
}

/* Write a string asynchronously
 * Returns 	0 on success
 * 			1 if there is already a string being sent
 */
uint8_t MHV_HardwareSerial::asyncWrite(const char *buffer) {
	if (!canSend()) {
		return 1;
	}
	*_ucsrb |= _BV( _txcie);

	_txBufferLength = MHV_SENDING_STRING;
	_txBufferProgmem = false;

	// Start writing
	if (buffer[0] == '\0') {
		_txBuffer = NULL;
		return 0;
	} else if (((*_ucsra) & (1 << _udre))) {
		_txBuffer = buffer + 1;
		*_udr = buffer[0];
	} else {
		// Already busy sending a byte
		_txBuffer = buffer;
	}

	return 0;
}

/* Write a buffer asynchronously
 * Returns 	0 on success
 * 			1 if there is already a string being sent
 */
uint8_t MHV_HardwareSerial::asyncWrite_P(const char *buffer, uint16_t length) {
	if (!canSend()) {
		return 1;
	}
	*_ucsrb |= _BV( _txcie);

	_txBufferProgmem = true;

	// Start writing
	if (length == 0) {
		_txBuffer = NULL;
		return 0;
	}  else if (((*_ucsra) & (1 << _udre))) {
		_txBuffer = buffer + 1;
		_txBufferLength = length - 1;
		*_udr = pgm_read_byte(buffer);
	} else {
		// Already busy sending a byte
		_txBuffer = buffer;
		_txBufferLength = length;
	}

	return 0;
}

/* Write a buffer asynchronously
 * Returns 	0 on success
 * 			1 if there is already a string being sent
 */
uint8_t MHV_HardwareSerial::asyncWrite(const char *buffer, uint16_t length) {
	if (!canSend()) {
		return 1;
	}
	*_ucsrb |= _BV( _txcie);

	_txBufferProgmem = false;

	// Start writing
	if (length == 0) {
		_txBuffer = NULL;
		return 0;
	} else if (((*_ucsra) & (1 << _udre))) {
		_txBuffer = buffer + 1;
		_txBufferLength = length - 1;
		*_udr = buffer[0];
	} else {
		// Already busy sending a byte
		_txBuffer = buffer;
		_txBufferLength = length;
	}
	return 0;
}
