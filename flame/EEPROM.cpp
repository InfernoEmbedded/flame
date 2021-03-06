/* Copyright (c) 2014, Inferno Embedded
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of the Inferno Embedded nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL INFERNO EMBEDDED BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stddef.h>
#include <flame/EEPROM.h>
#include <util/crc16.h>

namespace flame {


/**
 * Create a new EEPROM access class
 */
EEPROM::EEPROM() :
	_writeAddress(0),
	_writeBuffer(NULL),
	_bytesWritten(0),
	_bytesToWrite(0),
	_done(NULL) {}

/**
 * Read a byte from EEPROM
 * @param	address	the address to read from
 * @return the byte, or FLAME_EEPROM_BUSY if the EEPROM is busy
 */
int16_t EEPROM::read(uint16_t address) {
	if ((EECR & _BV(EEPE)) || !_lock.obtain()) {
		return FLAME_EEPROM_BUSY;
	}

	EEAR = address;
	EECR |= _BV(EERE);

	int16_t val = (int16_t)EEDR;
	_lock.release();

	return val;
}

/**
 * Read a byte from EEPROM, waiting until the EEPROM is available
 * @param	address	the address to read from
 * @return the byte
 */
uint8_t EEPROM::busyRead(uint16_t address) {
	int16_t ret;

	while (FLAME_EEPROM_BUSY == (ret = read(address))) {}
	return ret;
}

/**
 * Read a buffer from EEPROM
 * @param	buffer	the buffer to populate
 * @param	address	the address to read from
 * @param	length	the number of bytes to read
 */
int8_t EEPROM::read(void *buffer, uint16_t address, uint16_t length) {
	if ((EECR & _BV(EEPE)) || !_lock.obtain()) {
		return FLAME_EEPROM_BUSY;
	}

	uint8_t *buf = (uint8_t *)buffer;

	for (uint16_t i = 0; i < length; i++) {
		EEAR = address + i;
		EECR |= _BV(EERE);
		buf[i] = EEDR;
	}

	_lock.release();

	return 0;
}

/**
 * Read a buffer from EEPROM, waiting until the EEPROM is available
 * @param	buffer	the buffer to populate
 * @param	address	the address to read from
 * @param	length	the number of bytes to read
 */
void EEPROM::busyRead(void *buffer, uint16_t address, uint16_t length) {
	while (FLAME_EEPROM_BUSY == (read(buffer, address, length))) {}
}

/**
 * Write a byte to EEPROM
 * @param	address	the address to write to
 * @param	data	the byte to write
 * @return FLAME_EEPROM_BUSY if the EEPROM is busy
 */
int8_t EEPROM::write(uint16_t address, uint8_t data) {
	if ((EECR & _BV(EEPE)) || !_lock.obtain()) {
		return FLAME_EEPROM_BUSY;
	}

	EEAR = address;
	EEDR = data;
	EECR = _BV(EEMPE);
	EECR = _BV(EEMPE) | _BV(EEPE);

	_lock.release();

	return 0;
}

/**
 * Write a buffer to EEPROM
 * @param	buffer	the buffer to read from
 * @param	address	the address to write to
 * @param	length	the number of bytes to write
 */
int8_t EEPROM::busyWrite(void *buffer, uint16_t address, uint16_t length) {
	uint8_t *buf = (uint8_t *)buffer;

	if ((EECR & _BV(EEPE)) || !_lock.obtain()) {
		return FLAME_EEPROM_BUSY;
	}

	for (uint16_t i = 0; i < length; i++) {
		EEAR = address + i;
		EEDR = buf[i];
		EECR = _BV(EEMPE);
		EECR = _BV(EEMPE) | _BV(EEPE);

		while (EECR & _BV(EEPE)) {}
	}

	_lock.release();

	return 0;
}


/**
 * Write a buffer to EEPROM
 * @param	buffer				the buffer to read from
 * @param	address				the address to write to
 * @param	length				the number of bytes to write (must be greater than 0)
 * @param	done				A listener to notify when the buffer has been written (can be NULL)
 */
int8_t EEPROM::write(void *buffer, uint16_t address, uint16_t length,
		EEPROMListener *done) {
	if ((EECR & _BV(EEPE)) || !_lock.obtain()) {
		return FLAME_EEPROM_BUSY;
	}

	_writeAddress = address;
	_writeBuffer = (uint8_t *)buffer;
	_bytesWritten = 0;
	_bytesToWrite = length;
	_done = done;

	// Start the first write
	EEAR = address;
	EEDR = ((uint8_t *)buffer)[0];
	EECR = _BV(EEMPE);

	// Enable the ready interrupt, further writes will continue in the interrupt handler
	EECR = _BV(EEMPE) | _BV(EEPE) | _BV(EERIE);

	return 0;
}

/**
 * Interrupt handler for async writes
 */
void EEPROM::writeInterrupt() {
	if (++_bytesWritten == _bytesToWrite) {
		// Done
		EECR = 0;
		_lock.release();

		if (_done) {
			_done->eepromDone(*this, _writeAddress, _writeBuffer);
		}

		return;
	}

	EEAR = _writeAddress + _bytesWritten;
	EEDR = _writeBuffer[_bytesWritten];
	EECR = _BV(EEMPE);
	// Enable the ready interrupt, further writes will continue in the interrupt handler
	EECR = _BV(EEMPE) | _BV(EEPE) | _BV(EERIE);
}

/**
 * Check if the EEPROM hardware is busy
 * @return true if the EEPROM hardware is busy
 */
bool EEPROM::isBusy() {
	if ((EECR & _BV(EEPE)) || _lock.check()) {
		return FLAME_EEPROM_BUSY;
	}

	return false;
}

/**
 * Calculate the CRC of a block of EEPROM
 * @param address	the address of the block
 * @param length	the length of the block
 * @return the CRC
 */
uint16_t EEPROM::crc(uint16_t address, uint16_t length) {
	uint16_t crc = 0;

	for (uint16_t i = 0; i < length; i++) {
		crc = _crc16_update(crc, busyRead(address + i));
	}

	return crc;
}

}
