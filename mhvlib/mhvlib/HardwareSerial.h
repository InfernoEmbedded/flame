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


#ifndef MHV_HardwareSerial_h
#define MHV_HardwareSerial_h

#include <inttypes.h>
#include <avr/interrupt.h>
#include <mhvlib/io.h>
#include <stdio.h>
#include <mhvlib/RingBuffer.h>
#include <avr/pgmspace.h>
#include <mhvlib/Device_TX.h>
#include <mhvlib/Device_RX.h>
#include <avr/sfr_defs.h>
#include <stdlib.h>
#include <string.h>


#define MHV_HARDWARESERIAL_ASSIGN_INTERRUPTS(mhvHardwareSerial, mhvHardwareSerialInterrupts) \
	_MHV_HARDWARESERIAL_ASSIGN_INTERRUPTS(mhvHardwareSerial, mhvHardwareSerialInterrupts)

#define _MHV_HARDWARESERIAL_ASSIGN_INTERRUPTS(mhvHardwareSerial, mhvRxVect, mhvTxVect) \
ISR(mhvRxVect) { \
	mhvHardwareSerial.rx(); \
} \
ISR(mhvTxVect) { \
	mhvHardwareSerial.tx(); \
}

/**
 * Create a new serial object
 * @param	_mhvObjectName	the variable name of the object
 * @param	_mhvRXBUFLEN	the maximum length of the line to be received
 * @param	_mhvTXBUFCOUNT	the maximum number of tx buffers to send asynchonously
 * @param	_mhvSERIAL		serial port parameters
 * @param	_mhvBAUD		the baud rate requested
 */
#define MHV_HARDWARESERIAL_CREATE(_mhvObjectName, _mhvRXBUFLEN, _mhvTXBUFCOUNT, _mhvSERIAL, _mhvBAUD) \
		MHV_HardwareSerial<_mhvSERIAL, _mhvBAUD, _mhvRXBUFLEN, _mhvTXBUFCOUNT> _mhvObjectName; \
		MHV_HARDWARESERIAL_ASSIGN_INTERRUPTS(_mhvObjectName, _mhvSERIAL ## _INTERRUPTS);

/**
 * Create now serial port driver
 * @tparam	usart			the serial port parameters
 * @tparam	baud			the baud rate to run at
 * @tparam	rxBufLength		the maximum number of characters to receive
 * @tparam	txBuffers		the number of send buffers
 * @post Interrupts should be assigned to the driver
 */
template <MHV_DECLARE_USART(usart), uint32_t baud, uint8_t rxBufLength, uint8_t txBuffers>
class MHV_HardwareSerial : public MHV_Device_TXImplementation<txBuffers>,
	public MHV_Device_RXImplementation<rxBufLength> {
private:
	volatile bool _echo;

protected:
	/**
	 * Start sending async data
	 */
	void runTxBuffers() {
		int c = MHV_Device_TX::nextCharacter();
		if (-1 == c) {
	// This should never happen
			return;
		}

		// Enable tx interrupt
		_MMIO_BYTE(usartControl) |= _BV(usartTxInterruptEnable);

		// If the UART isn't already sending data, start sending
		while (!(_MMIO_BYTE(usartStatus) & _BV(usartDataEmpty))) {}

		_MMIO_BYTE(usartIO) = (char)c;
	}

public:
	/**
	 * Constructor
	 */
	MHV_HardwareSerial() {
		_echo = false;
		MHV_Device_TX::_tx = NULL;

		setSpeed(baud);
	}

	/**
	 * RX interrupt handler
	 */
	void rx() {
		char c = _MMIO_BYTE(usartIO);
		MHV_Device_RX::_rxBuffer.append(c);

		if (_echo && (_MMIO_BYTE(usartStatus) & _BV(usartDataEmpty))) {
			_MMIO_BYTE(usartIO) = c;
		}
	}

	/**
	 * TX interrupt handler
	 */
	void tx() {
		int c = MHV_Device_TX::nextCharacter();

		if (-1 == c) {
			// Nothing more to send, disable the TX interrupt
			MHV_Device_TX::_tx = NULL;
			_MMIO_BYTE(usartControl) &= ~_BV(usartTxInterruptEnable);
			return;
		}

		_MMIO_BYTE(usartIO) = (char)c;
	}

	/**
	 * Configure the serial port for a specific baud rate
	 * @param	newBaud	the baud rate to set
	 */
	INLINE void setSpeed(unsigned long newBaud) {
	/* Use U2X if the requested baud rate is higher than (F_CPU/16),
	 * otherwise use whatever has the least error
	 */
		if (newBaud > F_CPU / 16 ||
				abs((int)(255-((F_CPU/(8*(((F_CPU/4/newBaud-1)/2)+1))*255)/newBaud))) < abs((int)(255-((F_CPU/(16*(((F_CPU/8/newBaud-1)/2)+1))*255)/newBaud)))) {
			_MMIO_BYTE(usartStatus) = _BV(usartU2X);
			_MMIO_BYTE(usartBaud) = (uint16_t)(F_CPU / 4 / newBaud - 1) / 2;
		} else {
			_MMIO_BYTE(usartStatus) = 0;
			_MMIO_BYTE(usartBaud) = (uint16_t)(F_CPU / 8 / newBaud - 1) / 2;
		}

		_MMIO_BYTE(usartControl) |= _BV(usartRxEnable) | _BV(usartTxEnable) | _BV(usartRxInterruptEnable);
	}

	/**
	 * Halt the serial port
	 */
	void end() {
		_MMIO_BYTE(usartControl) &= ~_BV(usartRxEnable) & ~_BV(usartTxEnable) & ~_BV(usartRxInterruptEnable) & ~_BV(usartTxInterruptEnable);
	}

	/**
	 * Enable echoing data received by us back to the sender (useful for terminal
	 * interaction
	 *
	 * @param	echoOn	true to enable echo
	 */
	void echo(bool echoOn) {
		_echo = echoOn;
	}

	/**
	 * Can we send something via busywrite
	 *
	 * @return true if we can send something
	 */
	bool canSendBusy() {
		return ((NULL == MHV_Device_TX::_tx) && (_MMIO_BYTE(usartStatus) & _BV(usartDataEmpty)));
	}

	/**
	 * Write a character
	 *
	 * @param	c	the character to write
	 */
	void busyWrite(char c) {
		while (!canSendBusy()) {};
		_MMIO_BYTE(usartControl) &= ~_BV(usartTxInterruptEnable);

		while (!(_MMIO_BYTE(usartStatus) & _BV(usartDataEmpty))) {}

		_MMIO_BYTE(usartIO) = c;
	}

	/**
	 * Write a null terminated progmem string
	 *
	 * @param	buffer	the string to write
	 */
	void busyWrite_P(PGM_P buffer) {
		const char *p;

		while (!canSendBusy()) {};
		_MMIO_BYTE(usartControl) &= ~_BV(usartTxInterruptEnable);

		p = buffer;
		char c = pgm_read_byte(p++);
		while (c != '\0') {
			while (!(_MMIO_BYTE(usartStatus) & _BV(usartDataEmpty))) {}

			_MMIO_BYTE(usartIO) = c;
			c = pgm_read_byte(p++);
		}
	}


	/**
	 * Write a null terminated string to the serial port
	 *
	 * @param	buffer	the string to write
	 */
	void busyWrite(const char *buffer) {
		const char *p;

		while (!canSendBusy()) {};
		_MMIO_BYTE(usartControl) &= ~_BV(usartTxInterruptEnable);

		for (p = buffer; *p != '\0';) {
			while (!(_MMIO_BYTE(usartStatus) & _BV(usartDataEmpty))) {}

			_MMIO_BYTE(usartIO) = *(p++);
		}
	}

	/**
	 * Write a buffer from PROGMEM
	 * @param	buffer	the buffer to write
	 * @param	length	the length of the buffer
	 */
	void busyWrite_P(PGM_P buffer, uint16_t length) {
		uint16_t i;

		while (!canSendBusy()) {};
		_MMIO_BYTE(usartControl) &= ~_BV(usartTxInterruptEnable);

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
	void busyWrite(const char *buffer, uint16_t length) {
		uint16_t i;

		while (!canSendBusy()) {};
		_MMIO_BYTE(usartControl) &= ~_BV(usartTxInterruptEnable);

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
	bool busy(void) {
		return !(_MMIO_BYTE(usartStatus) & _BV(usartDataEmpty));
	}
};


#endif
