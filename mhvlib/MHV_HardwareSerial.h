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
#include <MHV_io.h>
#include <stdio.h>
#include <MHV_RingBuffer.h>
#include <avr/pgmspace.h>
#include <MHV_Device_TX.h>
#include <MHV_Device_RX.h>

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
		MHV_RX_BUFFER_CREATE(_mhvObjectName ## RX, _mhvRXBUFLEN); \
		MHV_TX_BUFFER_CREATE(_mhvObjectName ## TX, _mhvTXBUFCOUNT); \
		MHV_HardwareSerial _mhvObjectName(_mhvObjectName ## RX, _mhvObjectName ## RX, _mhvSERIAL, _mhvBAUD); \
		MHV_HARDWARESERIAL_ASSIGN_INTERRUPTS(_mhvObjectName, _mhvSERIAL ## _INTERRUPTS);

#define MHV_HARDWARESERIAL_DEBUG MHV_DEBUG

class MHV_HardwareSerial : public MHV_Device_TX, public MHV_Device_RX {
private:
	volatile uint16_t *_ubrr;
	volatile uint8_t *_ucsra;
	volatile uint8_t *_ucsrb;
	volatile uint8_t *_udr;
	uint8_t _rxen;
	uint8_t _txen;
	uint8_t _rxcie;
	uint8_t _txcie;
	uint8_t _udre;
	uint8_t _u2x;
	volatile bool _echo;

protected:
	void runTxBuffers();

public:
	MHV_HardwareSerial(MHV_RingBuffer &rxBuffer, MHV_RingBuffer &txBuffer, volatile uint16_t *ubrr,
			volatile uint8_t *ucsra, volatile uint8_t *ucsrb, volatile uint8_t *udr, uint8_t rxen,
			uint8_t txen, uint8_t rxcie, uint8_t txcie, uint8_t udre, uint8_t u2x,
			unsigned long baud);
	void setSpeed(unsigned long baud);
	void end();
	void busyWrite(char c);
	void busyWrite(const char *buffer);
	void busyWrite(const char *buffer, uint16_t length);
	void busyWrite_P(PGM_P buffer);
	void busyWrite_P(PGM_P buffer, uint16_t length);
	bool canSendBusy();
	void rx();
	void tx();
	void echo(bool echoOn);
	bool busy();

};


#endif
