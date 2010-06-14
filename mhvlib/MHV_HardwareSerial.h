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


#ifndef MHV_HardwareSerial_h
#define MHV_HardwareSerial_h

#include <inttypes.h>
#include <avr/interrupt.h>
#include <MHV_io.h>
#include <MHV_RingBuffer.h>

#define MHV_HARDWARESERIAL_ASSIGN_INTERRUPTS(mhvHardwareSerial, mhvHardwareSerialInterrupts) \
	_MHV_HARDWARESERIAL_ASSIGN_INTERRUPTS(mhvHardwareSerial, mhvHardwareSerialInterrupts)

#define _MHV_HARDWARESERIAL_ASSIGN_INTERRUPTS(mhvHardwareSerial, mhvRxVect, mhvTxVect) \
ISR(mhvRxVect) { \
	mhvHardwareSerial.rx(); \
} \
ISR(mhvTxVect) { \
	mhvHardwareSerial.tx(); \
}

class MHV_HardwareSerial {
private:
	MHV_RingBuffer *_rxBuffer;
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
	const char *_txBuffer;
	int16_t _txBufferLength;
	void (_txCompleteCallback)(char *);
	volatile bool _txBufferProgmem;
	volatile bool _echo;

public:
	MHV_HardwareSerial(MHV_RingBuffer *rxBuffer, volatile uint16_t *ubrr,
			volatile uint8_t *ucsra, volatile uint8_t *ucsrb, volatile uint8_t *udr, uint8_t rxen,
			uint8_t txen, uint8_t rxcie, uint8_t txcie, uint8_t udre, uint8_t u2x,
			unsigned long baud);
	void setSpeed(unsigned long baud);
	void end(void);
	uint8_t available(void);
	int read(void);
	void flush(void);
	uint8_t busyWrite(char c);
	uint8_t busyWrite(const char *buffer);
	uint8_t busyWrite(const char *buffer, uint16_t length);
	uint8_t busyWrite_P(const char *buffer);
	uint8_t busyWrite_P(const char *buffer, uint16_t length);
	bool canSend(void);
	uint8_t asyncWrite(const char *buffer);
	uint8_t asyncWrite_P(const char *buffer);
	uint8_t asyncWrite(const char *buffer, uint16_t length);
	uint8_t asyncWrite_P(const char *buffer, uint16_t length);
	void rx(void);
	void tx(void);
	int readLine(char *buffer, uint8_t bufferLength);
	void echo(bool echoOn);
	bool busy(void);
};

#define MHV_SENDING_STRING -1


#endif
