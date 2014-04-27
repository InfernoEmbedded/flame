/*
 * Copyright (c) 2014, Inferno Embedded
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


#ifndef FLAME_HardwareSerial_h
#define FLAME_HardwareSerial_h

#include <inttypes.h>
#include <avr/interrupt.h>
#include <flame/io.h>
#include <stdio.h>
#include <flame/RingBuffer.h>
#include <avr/pgmspace.h>
#include <flame/Device_TX.h>
#include <flame/Device_RX.h>
#include <avr/sfr_defs.h>
#include <stdlib.h>
#include <string.h>

#define FLAME_DEBUG_TX	0

#define FLAME_HARDWARESERIAL_ASSIGN_INTERRUPTS(flameHardwareSerial, flameHardwareSerialInterrupts) \
	_FLAME_HARDWARESERIAL_ASSIGN_INTERRUPTS(flameHardwareSerial, flameHardwareSerialInterrupts)

#define _FLAME_HARDWARESERIAL_ASSIGN_INTERRUPTS(flameHardwareSerial, flameRxVect, flameTxVect) \
ISR(flameRxVect) { \
	flameHardwareSerial.rx(); \
} \
ISR(flameTxVect) { \
	flameHardwareSerial.tx(); \
}

/**
 * Create a new serial object
 * @param	_flameObjectName	the variable name of the object
 * @param	_flameRXBUFLEN	the maximum length of the line to be received
 * @param	_flameTXBUFCOUNT	the maximum number of tx buffers to send asynchonously
 * @param	_flameSERIAL		serial port parameters
 * @param	_flameBAUD		the baud rate requested
 */
#define FLAME_HARDWARESERIAL_CREATE(_flameObjectName, _flameRXBUFLEN, _flameTXBUFCOUNT, _flameSERIAL, _flameBAUD) \
		HardwareSerial<_flameSERIAL, _flameBAUD, _flameRXBUFLEN, _flameTXBUFCOUNT> _flameObjectName __attribute__ ((visibility ("default"))); \
		FLAME_HARDWARESERIAL_ASSIGN_INTERRUPTS(_flameObjectName, _flameSERIAL ## _INTERRUPTS);

/**
 * Import an external serial object
 * @param	_flameObjectName	the variable name of the object
 * @param	_flameRXBUFLEN	the maximum length of the line to be received
 * @param	_flameTXBUFCOUNT	the maximum number of tx buffers to send asynchonously
 * @param	_flameSERIAL		serial port parameters
 * @param	_flameBAUD		the baud rate requested
 */
#define FLAME_HARDWARESERIAL_IMPORT(_flameObjectName, _flameRXBUFLEN, _flameTXBUFCOUNT, _flameSERIAL, _flameBAUD) \
		extern flame::HardwareSerial<_flameSERIAL, _flameBAUD, _flameRXBUFLEN, _flameTXBUFCOUNT> _flameObjectName;

namespace flame {

enum class SerialParity : uint8_t {
	NONE = 0,
	EVEN = 2,
	ODD = 3
};

INLINE uint8_t operator<<(SerialParity parity, uint8_t bits) {
	return (uint8_t)parity << bits;
}

enum class SerialMode : uint8_t {
	ASYNC = 0,
	SYNC = 1,
	MASTER_SPI = 3
};

INLINE uint8_t operator<<(SerialMode mode, uint8_t bits) {
	return (uint8_t)mode << bits;
}

/**
 * Create now serial port driver
 * @tparam	usart			the serial port parameters
 * @tparam	baud			the baud rate to run at
 * @tparam	rxBufLength		the maximum number of characters to receive
 * @tparam	txBuffers		the number of send buffers
 * @post Interrupts should be assigned to the driver
 */
template <FLAME_DECLARE_USART(usart), uint32_t baud, uint8_t rxBufLength, uint8_t txBuffers>
class HardwareSerial : public Device_TXImplementation<txBuffers>,
	public Device_RXImplementation<rxBufLength> {
private:
	volatile bool _echo;

protected:
	/**
	 * Start sending async data
	 */
	void runTxBuffers() {
#if FLAME_DEBUG_TX
		_MMIO_BYTE(usartControlB) &= ~_BV(usartTxInterruptEnable);
		dumpTXBufferState(__func__);
		//_MMIO_BYTE(usartControlB) |= _BV(usartTxInterruptEnable);
#endif

		int c = Device_TX::nextCharacter();

#if FLAME_DEBUG_TX
//		_MMIO_BYTE(usartControlB) &= ~_BV(usartTxInterruptEnable);
		dumpTXBufferState(__func__);
		_MMIO_BYTE(usartControlB) |= _BV(usartTxInterruptEnable);

		// Wait until the send is done
		while (!(_MMIO_BYTE(usartStatus) & _BV(usartDataEmpty))) {}
#endif


		if (-1 == c) {
	// This should never happen
			return;
		}

		// If the UART isn't already sending data, start sending
		while (!(_MMIO_BYTE(usartStatus) & _BV(usartDataEmpty))) {}

		_MMIO_BYTE(usartIO) = (char)c;

		// Enable tx interrupt
		_MMIO_BYTE(usartControlB) |= _BV(usartTxInterruptEnable);
	}

public:
	/**
	 * Constructor
	 */
	HardwareSerial() {
		_echo = false;

		configure(SerialMode::ASYNC, 8, SerialParity::NONE, 1, 0);
		setSpeed(baud);
	}

	/**
	 * Constructor
	 * @param	mode		the mode to run the serial port in
	 * @param	bits		the number of data bits
	 * @param	parity		the type of parity
	 * @param	stopBits	the number of stop bits
	 * @param	polarity	used only for synchronous mode
	 * 						when 0:	Change TX on rising,  sample RX on falling
	 * 						when 1:	Change TX on falling, sample RX on rising
	 */
	HardwareSerial(SerialMode mode, uint8_t bits, SerialParity parity, uint8_t stopBits, uint8_t polarity) {
		_echo = false;

		configure(mode, bits, parity, stopBits, polarity);
		setSpeed(baud);
	}

	/**
	 * Set data characteristics
	 * @param	mode		the mode to run the serial port in
	 * @param	bits		the number of data bits
	 * @param	parity		the type of parity
	 * @param	stopBits	the number of stop bits
	 * @param	polarity	used only for synchronous mode
	 * 						when 0:	Change TX on rising,  sample RX on falling
	 * 						when 1:	Change TX on falling, sample RX on rising
	 */
	void configure(SerialMode mode, uint8_t bits, SerialParity parity, uint8_t stopBits, uint8_t polarity) {
		stopBits -= 1;

		uint8_t frameSizeMask;

		switch (bits) {
		case 5:
			frameSizeMask = 0;
			break;
		case 6:
			frameSizeMask = 1;
			break;
		case 7:
			frameSizeMask = 2;
			break;
		case 8:
		default:
			frameSizeMask = 3;
			break;

		case 9:
			frameSizeMask = 3;
			_MMIO_BYTE(usartControlB) &= 1 << 2;
			break;

		}

		_MMIO_BYTE(usartControlC) = (mode << 6) | (parity << 4) | (frameSizeMask << 1) | polarity;
	}

	/**
	 * RX interrupt handler
	 */
	void rx() {
		char c = _MMIO_BYTE(usartIO);
		Device_RX::_rxBuffer.append(c);

		if (_echo && (_MMIO_BYTE(usartStatus) & _BV(usartDataEmpty))) {
			_MMIO_BYTE(usartIO) = c;
		}
	}

	/**
	 * Dump the state of the TX buffer
	 * @param	func	the name of the caller
	 */
	void dumpTXBufferState(const char *func) {
		char buf[100];
		Device_TX::_currentTx.dumpState(buf, sizeof(buf), func);
		busyWrite(buf);
	}

	/**
	 * TX interrupt handler
	 */
	void tx() {
#if FLAME_DEBUG_TX
		_MMIO_BYTE(usartControlB) &= ~_BV(usartTxInterruptEnable);
		dumpTXBufferState(__func__);
		//_MMIO_BYTE(usartControlB) |= _BV(usartTxInterruptEnable);
#endif

		int c = Device_TX::nextCharacter();

#if FLAME_DEBUG_TX
//		_MMIO_BYTE(usartControlB) &= ~_BV(usartTxInterruptEnable);
		dumpTXBufferState(__func__);
		_MMIO_BYTE(usartControlB) |= _BV(usartTxInterruptEnable);
#endif


		if (-1 == c) {
			// Nothing more to send, disable the TX interrupt
			_MMIO_BYTE(usartControlB) &= ~_BV(usartTxInterruptEnable);

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

		// Enable the port
		_MMIO_BYTE(usartControlB) |= _BV(usartRxEnable) | _BV(usartTxEnable) | _BV(usartRxInterruptEnable);
	}

	/**
	 * Halt the serial port
	 */
	void end() {
		_MMIO_BYTE(usartControlB) &= ~_BV(usartRxEnable) & ~_BV(usartTxEnable) & ~_BV(usartRxInterruptEnable) & ~_BV(usartTxInterruptEnable);
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
		return ((!Device_TX::_currentTx.hasMore()) && (_MMIO_BYTE(usartStatus) & _BV(usartDataEmpty)));
	}

	/**
	 * Write a character
	 *
	 * @param	c	the character to write
	 */
	void busyWrite(char c) {
		while (!canSendBusy()) {};
		_MMIO_BYTE(usartControlB) &= ~_BV(usartTxInterruptEnable);

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
		_MMIO_BYTE(usartControlB) &= ~_BV(usartTxInterruptEnable);

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
		_MMIO_BYTE(usartControlB) &= ~_BV(usartTxInterruptEnable);

		for (p = buffer; *p != '\0';) {
			while (!(_MMIO_BYTE(usartStatus) & _BV(usartDataEmpty))) {}

			_MMIO_BYTE(usartIO) = *(p++);
		}
	}

#ifdef UNSUPPORTED
#ifdef __FLASH
	/**
	 * Write a null terminated flash string to the serial port
	 *
	 * @param	buffer	the string to write
	 */
	void busyWrite(const __flash char *buffer) {
		const __flash char *p;

		while (!canSendBusy()) {};
		_MMIO_BYTE(usartControlB) &= ~_BV(usartTxInterruptEnable);

		for (p = buffer; *p != '\0';) {
			while (!(_MMIO_BYTE(usartStatus) & _BV(usartDataEmpty))) {}

			_MMIO_BYTE(usartIO) = *(p++);
		}
	}

	/**
	 * Write a buffer from flash
	 * @param	buffer	the buffer to write
	 * @param	length	the length of the buffer
	 */
	void busyWrite_P(const __flash char *buffer, uint16_t length) {
		uint16_t i;

		while (!canSendBusy()) {};
		_MMIO_BYTE(usartControlB) &= ~_BV(usartTxInterruptEnable);

		for (i = 0; i < length; i++) {
			/* Don't need to check return values as we have already checked up front
			 * and async writes can't be initiated until we're done
			 */
			busyWrite(buffer[i]));
		}
	}
#endif

#ifdef __MEMX
	/**
	 * Write a null terminated flash string to the serial port
	 *
	 * @param	buffer	the string to write
	 */
	void busyWrite(const __memx char *buffer) {
		const __memx char *p;

		while (!canSendBusy()) {};
		_MMIO_BYTE(usartControlB) &= ~_BV(usartTxInterruptEnable);

		for (p = buffer; *p != '\0';) {
			while (!(_MMIO_BYTE(usartStatus) & _BV(usartDataEmpty))) {}

			_MMIO_BYTE(usartIO) = *(p++);
		}
	}

	/**
	 * Write a buffer from memx
	 * @param	buffer	the buffer to write
	 * @param	length	the length of the buffer
	 */
	void busyWrite_P(const __memx char *buffer, uint16_t length) {
		uint16_t i;

		while (!canSendBusy()) {};
		_MMIO_BYTE(usartControlB) &= ~_BV(usartTxInterruptEnable);

		for (i = 0; i < length; i++) {
			/* Don't need to check return values as we have already checked up front
			 * and async writes can't be initiated until we're done
			 */
			busyWrite(buffer[i]));
		}
	}

#endif
#endif

	/**
	 * Write a buffer from PROGMEM
	 * @param	buffer	the buffer to write
	 * @param	length	the length of the buffer
	 */
	void busyWrite_P(PGM_P buffer, uint16_t length) {
		uint16_t i;

		while (!canSendBusy()) {};
		_MMIO_BYTE(usartControlB) &= ~_BV(usartTxInterruptEnable);

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
		_MMIO_BYTE(usartControlB) &= ~_BV(usartTxInterruptEnable);

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
}

#endif
