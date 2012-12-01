/* Copyright (c) 2012, Make, Hack, Void Inc
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


#ifndef MHV_EEPROM_H_
#define MHV_EEPROM_H_

#include <mhvlib/io.h>
#include <mhvlib/Lock.h>
#include <inttypes.h>

#ifdef EE_READY_vect
#define MHV_EEPROM_ASSIGN_INTERRUPTS(_mhvEeprom) \
ISR(EE_READY_vect) { \
	_mhvEeprom.writeInterrupt(); \
}
#elif defined EEPROM_READY_vect
ISR(EEPROM_READY_vect) { \
	_mhvEeprom.writeInterrupt(); \
}
#endif

#define MHV_EEPROM_CREATE(_mhvObjectName) \
		EEPROM _mhvObjectName; \
		MHV_EEPROM_ASSIGN_INTERRUPTS(_mhvObjectName);

#define MHV_EEPROM_BUSY	-1

#define MHV_ACCELEROMETER_CAL_EEPROM_ADDRESS	1003
// 6 bytes offsets + 12 bytes scaling + 2 bytes checksum
#define MHVBOARD_TIMEOUT_EEPROM_ADDRESS			1023

namespace mhvlib {

class EEPROM;

class EEPROMListener {
public:
	/**
	 * Called when an EEPROM write is finished
	 * @param eeprom		the EEPROM access instance
	 * @param address		the EEPROM address that the data was written to
	 * @param buffer		the pointer that the data was wriiten from
	 */
	virtual void eepromDone(EEPROM *eeprom, uint16_t address, void *buffer) =0;
};

class EEPROM {
private:
	Lock			_lock;
	uint16_t		_writeAddress;
	uint8_t			*_writeBuffer;
	uint16_t		_bytesWritten;
	uint16_t		_bytesToWrite;
	EEPROMListener	*_done;

public:
	EEPROM();
	int16_t read(uint16_t address);
	uint8_t busyRead(uint16_t address);
	int8_t read(void *buffer, uint16_t address, uint16_t length);
	void busyRead(void *buffer, uint16_t address, uint16_t length);
	int8_t write(uint16_t address, uint8_t data);
	int8_t busyWrite(void *buffer, uint16_t address, uint16_t length);
	int8_t write(void *buffer, uint16_t address, uint16_t length,
			EEPROMListener *done);
	void writeInterrupt();
	bool isBusy();
	uint16_t crc(uint16_t address, uint16_t length);
};

}

#endif /* MHV_EEPROM_H_ */
