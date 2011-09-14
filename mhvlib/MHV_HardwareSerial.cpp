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
#include <avr/sfr_defs.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "MHV_HardwareSerial.h"

/**
 * Constructor
 * @param	rxBuffer	the receive buffer
 * @param	txBuffer	the transmit buffer
 * @param	serial		the parameter list for the serial port
 * @param	baud		the baud rate
 */
MHV_HardwareSerial::MHV_HardwareSerial(MHV_RingBuffer *rxBuffer, MHV_RingBuffer *txBuffer,
		volatile uint16_t *ubrr, volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
		volatile uint8_t *udr, uint8_t rxen, uint8_t txen, uint8_t rxcie,
		uint8_t txcie, uint8_t udre, uint8_t u2x, unsigned long baud) :
		MHV_Device_TX(txBuffer), MHV_Device_RX(rxBuffer) {
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

/**
 * RX interrupt handler
 */
void MHV_HardwareSerial::rx() {
	char c = *_udr;
	_rxBuffer->append(c);

	if (_echo && ((*_ucsra) & (1 << _udre))) {
		*_udr = c;
	}
}

/**
 * TX interrupt handler
 */
void MHV_HardwareSerial::tx() {
	int c = nextCharacter();

	if (-1 == c) {
		// Nothing more to send, disable the TX interrupt
		_tx = NULL;
		*_ucsrb &= ~_BV( _txcie);
		return;
	}

	*_udr = (char)c;
}

/**
 * Start sending async data
 */
void MHV_HardwareSerial::runTxBuffers() {
	int c = nextCharacter();
	if (-1 == c) {
// This should never happen
		return;
	}

	// Enable tx interrupt
	*_ucsrb |= _BV( _txcie);

	// If the UART isn't already sending data, start sending
	while (!((*_ucsra) & (1 << _udre))) {}

	*_udr = (char)c;
}

/**
 * Configure the serial port for a specific baud rate
 * @param	baud	the baud rate to set
 */
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

/**
 * Halt the serial port
 */
void MHV_HardwareSerial::end() {
	*_ucsrb &= ~_BV( _rxen) & ~_BV( _txen) & ~_BV( _rxcie) & ~_BV( _txcie);
}

/**
 * Enable echoing data received by us back to the sender (useful for terminal
 * interaction
 *
 * @param	echoOn	true to enable echo
 */
void MHV_HardwareSerial::echo(bool echoOn) {
	_echo = echoOn;
}

/**
 * Can we send something via busywrite
 *
 * @return true if we can send something
 */
bool MHV_HardwareSerial::canSendBusy() {
	return ((NULL == _tx) && ((*_ucsra) & (1 << _udre)));
}

/**
 * Write a character
 *
 * @param	c	the character to write
 */
void MHV_HardwareSerial::busyWrite(char c) {
	while (!canSendBusy()) {};
	*_ucsrb &= ~_BV( _txcie);

	while (!((*_ucsra) & (1 << _udre))) {}

	*_udr = c;
}

/**
 * Write a null terminated progmem string
 *
 * @param	buffer	the string to write
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


/**
 * Write a null terminated string to the serial port
 *
 * @param	buffer	the string to write
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

/**
 * Write a buffer from PROGMEM
 * @param	buffer	the buffer to write
 * @param	length	the length of the buffer
 */
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

/**
 * Write a buffer
 * @param	buffer	the buffer to write
 * @param	length	the length of the buffer
 */
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

/**
 * Check if the hardware is busy - note that this should not be used to
 * determine if you can actually write - use canSend instead
 *
 * @return true if the hardware is busy
 */
bool MHV_HardwareSerial::busy(void) {
	return !((*_ucsra) & (1 << _udre));
}

/**
 * Print a debug message
 * @param	file		the filename
 * @param	line		the line number
 * @param	function	the function name
 * @param	format		a printf format
 * @param	parms		the printf parms
 */
void MHV_HardwareSerial::debug(const char *file, int line, const char *function,
		PGM_P format, ...) {
	char	debugBuffer[80];
	va_list	ap;
	va_start(ap, format);

	while (!canSendBusy()) {}

	snprintf_P(debugBuffer, sizeof(debugBuffer), PSTR("%s:%d\t%s():\t\t"),
			file, line, function);
	busyWrite(debugBuffer);

	vsnprintf_P(debugBuffer, sizeof(debugBuffer), format, ap);
	busyWrite(debugBuffer);
	busyWrite_P(PSTR("\r\n"));
}
