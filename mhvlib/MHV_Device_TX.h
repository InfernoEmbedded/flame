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


#ifndef MHV_TXBUFFER_H
#define MHV_TXBUFFER_H

#define MHVLIB_NEED_PURE_VIRTUAL

#include <inttypes.h>
#include <avr/interrupt.h>
#include <MHV_io.h>
#include <stdio.h>
#include <MHV_RingBuffer.h>
#include <avr/pgmspace.h>

#define MHV_TX_BUFFER_CREATE(_mhvTxName, _mhvTxElementCount) \
	char _mhvTxName ## Buf[_mhvTxElementCount * sizeof(MHV_TX_BUFFER) + 1]; \
	MHV_RingBuffer _mhvTxName(_mhvTxName ## Buf, _mhvTxElementCount * sizeof(MHV_TX_BUFFER) + 1);

#define MHV_DEBUG(__dbg_tx, __dbg_format, __dbg_args...) \
do {\
	__dbg_tx.debug(__FILE__, __LINE__, __FUNCTION__, PSTR(__dbg_format), ## __dbg_args); \
} while (0)

struct mhv_tx_buffer {
	const char	*data;
	uint16_t	length;
	void		(*completeFunction)(const char *);
	bool		progmem;
	bool		isString;
};
typedef struct mhv_tx_buffer MHV_TX_BUFFER;

class MHV_Device_TX {
protected:
	MHV_TX_BUFFER	_currentTx;
	MHV_RingBuffer	*_txPointers;
	const char		*_tx;

	MHV_Device_TX(MHV_RingBuffer *txPointers);
	virtual void runTxBuffers()=0;
	bool moreTX();
	int nextCharacter();

public:
	bool canWrite();
	bool write(const char *buffer);
	bool write(const char *buffer, uint16_t length);
	bool write(const char *buffer, void (*completeFunction)(const char *));
	bool write(const char *buffer, uint16_t length, void (*completeFunction)(const char *));
	bool write_P(PGM_P buffer);
	bool write_P(PGM_P buffer, uint16_t length);
	bool  debug(const char *file, int line, const char *function,
			PGM_P format, ...);
	bool printf(PGM_P format, ...);
	bool write(uint8_t value);
	bool write(uint16_t value);
	bool write(uint32_t value);
	bool write(int8_t value);
	bool write(int16_t value);
	bool write(int32_t value);
};



#endif
