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


#ifndef MHV_EPP_BufferedIO_h
#define MHV_EPP_BufferedIO_h

#include <inttypes.h>
#include <avr/interrupt.h>
#include <MHV_io.h>
#include <stdio.h>
#include <MHV_RingBuffer.h>
#include <avr/pgmspace.h>
#include <MHV_Device_TX.h>

/**
 * Create a new buffered EPP slave
 *
 * @param	_mhvObjectName	the variable name of the object
 * @param	_mhvRXBUFLEN	the maximum length of the line to be received
 * @param	_mhvTXBUFCOUNT	the maximum number of tx buffers to send asynchonously
 * @param	_mhvSERIAL		serial port parameters
 * @param	_mhvBAUD		the baud rate requested
 */
#define MHV_EPP_BUFFEREDIO_CREATE(_mhvObjectName, _mhvRXBUFLEN, _mhvTXBUFCOUNT, _mhvSERIAL, _mhvBAUD) \
		char _mhvObjectName ## _rxBuffer[_mhvRXBUFLEN + 1]; \
		char _mhvObjectName ## _txBuffer[_mhvTXBUFCOUNT * sizeof(MHV_TX_BUFFER) + 1]; \
		MHV_RingBuffer _mhvObjectName ## _rxBuf(_mhvObjectName ## _rxBuffer, sizeof(_mhvObjectName ## _rxBuffer)); \
		MHV_RingBuffer _mhvObjectName ## _txBuf(_mhvObjectName ## _txBuffer, sizeof(_mhvObjectName ## _txBuffer)); \
		MHV_EPP_BufferedIO _mhvObjectName(&_mhvObjectName ## _rxBuf, &_mhvObjectName ## _txBuf, _mhvSERIAL, _mhvBAUD); \
		_MHV_EPP_BufferedIO_ASSIGN_INTERRUPTS(_mhvObjectName, _mhvSERIAL ## _INTERRUPTS);

class MHV_EPP_BufferedIO : public MHV_Device_TX {
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
	MHV_EPP_BufferedIO(MHV_RingBuffer *rxBuffer, MHV_RingBuffer *txBuffer,
			MHV_EPP_MODE mode,
			volatile uint8_t *dataDir, volatile uint8_t *dataInput, volatile uint8_t *dataOutput,
			uint8_t dataBit, uint8_t dataPcInt,
			volatile uint8_t *controlDir, volatile uint8_t *controlInput, volatile uint8_t *controlOutput,
			uint8_t controlBit, uint8_t controlPcInt);

	void end();
	uint8_t available();
	int read();
	void flush();
	void rx();
	void tx();
	int asyncReadLine(char *buffer, uint8_t bufferLength);

};


#endif
