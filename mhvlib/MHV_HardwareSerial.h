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
#include <stdio.h>
#include <MHV_RingBuffer.h>
#include <avr/pgmspace.h>
#include <MHV_Device_TX.h>

#define MHV_HARDWARESERIAL_ASSIGN_INTERRUPTS(mhvHardwareSerial, mhvHardwareSerialInterrupts) \
	_MHV_HARDWARESERIAL_ASSIGN_INTERRUPTS(mhvHardwareSerial, mhvHardwareSerialInterrupts)

#define _MHV_HARDWARESERIAL_ASSIGN_INTERRUPTS(mhvHardwareSerial, mhvRxVect, mhvTxVect) \
ISR(mhvRxVect) { \
	mhvHardwareSerial.rx(); \
} \
ISR(mhvTxVect) { \
	mhvHardwareSerial.tx(); \
}

#define MHV_HARDWARESERIAL_DEBUG(__dbg_serial, __dbg_format, __dbg_args...) \
do {\
	char __debug_buffer[80]; \
	while (!serial.canSendBusy()) {} \
	snprintf_P(__debug_buffer, sizeof(__debug_buffer), PSTR("%s:%d\t%s():\t\t"), __FILE__, __LINE__, __FUNCTION__); \
	__dbg_serial.busyWrite(__debug_buffer); \
	snprintf_P(__debug_buffer, sizeof(__debug_buffer), PSTR(__dbg_format), ## __dbg_args); \
	__dbg_serial.busyWrite(__debug_buffer); \
	__dbg_serial.busyWrite_P(PSTR("\r\n")); \
} while (0)

class MHV_HardwareSerial : public MHV_Device_TX {
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
	volatile bool _echo;

protected:
	void runTxBuffers();

public:
	MHV_HardwareSerial(MHV_RingBuffer *rxBuffer, MHV_RingBuffer *txBuffer, volatile uint16_t *ubrr,
			volatile uint8_t *ucsra, volatile uint8_t *ucsrb, volatile uint8_t *udr, uint8_t rxen,
			uint8_t txen, uint8_t rxcie, uint8_t txcie, uint8_t udre, uint8_t u2x,
			unsigned long baud);
	void setSpeed(unsigned long baud);
	void end();
	uint8_t available();
	int read();
	void flush();
	void busyWrite(char c);
	void busyWrite(const char *buffer);
	void busyWrite(const char *buffer, uint16_t length);
	void busyWrite_P(PGM_P buffer);
	void busyWrite_P(PGM_P buffer, uint16_t length);
	bool canSendBusy();
	void rx();
	void tx();
	int asyncReadLine(char *buffer, uint8_t bufferLength);
	int busyReadLine(char *buffer, uint8_t bufferLength);
	void echo(bool echoOn);
	bool busy();
};


#endif
