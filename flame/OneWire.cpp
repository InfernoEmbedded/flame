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
 *
 * OneWire has been maintained by Paul Stoffregen (paul@pjrc.com) since
 * January 2010.  At the time, it was in need of many bug fixes, but had
 * been abandoned the original author (Jim Studt).
 *
 * Copyright (c) 2007, Jim Studt  (original old version - many contributors since)
 *
 * Jim Studt's original library was modified by Josh Larios.
 * Tom Pollard, pollard@alum.mit.edu, contributed around May 20, 2008
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * (copyleft) 2006 by Derek Yerger - Free to distribute freely.
 *
 * The CRC code was excerpted and inspired by the Dallas Semiconductor
 * sample code bearing this copyright.
 * ---------------------------------------------------------------------------
 * Copyright (C) 2000 Dallas Semiconductor Corporation, All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL DALLAS SEMICONDUCTOR BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Dallas Semiconductor
 * shall not be used except as stated in the Dallas Semiconductor
 * Branding Policy.
 *--------------------------------------------------------------------------
 */

#include <flame/OneWire.h>
#include <stdlib.h>
#include <string.h>

namespace flame {

/**
 * Constructor for the OneWire bus driver
 * The 1-Wire bus communicates on a single GPIO, and requires the signal be pulled to
 * VCC with a 4.7k resistor. Buses with many parasitically powered devices may need this dropped to 1k.
 *
 * @param dataPin	the pin the bus is driven by
 */
OneWire::OneWire(Pin &dataPin) :
		_dataPin(dataPin) {
	_dataPin.setInputPullup();
#if ONEWIRE_SEARCH
	resetSearch();
#endif
}

/**
 * Write a bit
 * @post the bus is powered
 * @param value	true to write a 1, 0 otherwise
 */
void OneWire::writeBit(bool value) {
	if (value) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			_dataPin.off();
			_dataPin.setOutput();
			_delay_us(10);
			_dataPin.on();
		}
		_delay_us(55);
	} else {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			_dataPin.off();
			_dataPin.setOutput();
			_delay_us(65);
			_dataPin.on();
		}
		_delay_us(5);
	}
}

/**
 * Read a bit
 * @post the bus is powered
 * @return true if the bit was 1, 0 otherwise
 */
bool OneWire::readBit() {
	bool data = false;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		_dataPin.setOutput();
		_dataPin.off();
		_delay_us(3);
		_dataPin.setInput();	// let pin float, pull up will raise
		_delay_us(10);
		data = _dataPin.read();
	}
	_delay_us(53);

	return data;
}

/**
 * Depower the bus
 * You only need to do this if you used the 'power' flag to write() or used a writeBit() call
 * and aren't about to do another read or write. You would rather not leave this powered if you don't have to, just in case
 * someone shorts your bus.
 */
void OneWire::depower() {
	_dataPin.setInput();
}

/**
 * Perform the onewire reset function.  We will wait up to 250uS for the bus to come high,
 * if it doesn't then it is broken or shorted and we return false
 * @return true if a device asserted a presence pulse, false otherwise
 */
bool OneWire::reset() {
	bool devicePresent = false;
	uint8_t retries = 125;

	_dataPin.setInput();
	// wait until the wire is high... just in case
	do {
		if (--retries == 0)
			return false;
		_delay_us(2);
	} while (!_dataPin.read());

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		_dataPin.off();
		_dataPin.setOutput();
	}
	_delay_us(480);
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		_dataPin.setInput();	// allow it to float
		_delay_us(70);
		devicePresent = !_dataPin.read();
	}
	_delay_us(410);

	return devicePresent;
}

/**
 * Write a byte.
 *
 * The writing code uses the active drivers to raise the pin high, if you need power after the write
 * (e.g. DS18S20 in parasite power mode) then set 'power' to 1, otherwise the pin will go tri-state at the end of
 * the write to avoid heating in a short or other mishap.
 * @param value	the byte to write
 * @param power	true to leave the bus powered
 * @post if the bus was left powered, it can later be depowered with depower()
 */
void OneWire::write(uint8_t value, bool power) {
	uint8_t bitMask;

	for (bitMask = 0x01; bitMask; bitMask <<= 1) {
		writeBit((bitMask & value) ? true : false);
	}
	if (!power) {
		depower();
	}
}

/**
 * Write a buffer
 *
 * @see write
 * @param buf		the buffer to write
 * @param length	the length of the buffer
 * @param power	true to leave the bus powered
 * @post if the bus was left powered, it can later be depowered with depower()
 */
void OneWire::write(const uint8_t *buf, uint16_t length, bool power) {
	for (uint16_t i = 0; i < length; i++) {
		write(buf[i], true);
	}

	if (!power) {
		depower();
	}
}

/**
 * Read a byte
 * @return the byte read
 */
uint8_t OneWire::read() {
	uint8_t bitMask;
	uint8_t value = 0x00;

	for (bitMask = 0x01; bitMask; bitMask <<= 1) {
		if (OneWire::readBit()) {
			value |= bitMask;
		}
	}
	return value;
}

/**
 * Read into a buffer
 * @param buf		the buffer to read into
 * @param length	the length of the buffer
 */
void OneWire::read(uint8_t *buf, uint16_t length) {
	for (uint16_t i = 0; i < length; i++) {
		buf[i] = read();
	}
}

/**
 * Issue a 1-Wire rom select command
 * @pre reset() has been called
 */
void OneWire::select(OneWireAddress *address) {
	write(0x55);           // Choose ROM
	for (uint8_t i = 0; i < ONEWIRE_ADDRESS_BYTES; i++) {
		write(address->bytes[i]);
	}
}

/**
 * Issue a 1-Wire rom skip command, to address all on bus.
 */
void OneWire::skip() {
	write(0xCC);           // Skip ROM
}

/**
 * Clear the search state so that if will start from the beginning again.
 *
 * You need to use this function to start a search again from the beginning.
 * You do not need to do it for the first search, though you could.
 * @param familyCode	(optional) the family code of the device to search for, leave blank for all devices
 */
void OneWire::resetSearch(uint8_t familyCode) {
	_lastDiscrepancy = 0;
	_lastDeviceFlag = false;
	_lastFamilyDiscrepancy = 0;
	_romNumber[0] = familyCode;
	for (uint8_t i = 1; i < ONEWIRE_ADDRESS_BYTES; i++) {
		_romNumber[i] = 0;
	}
}

/**
 * Count the number of devices on the bus
 * @param familyCode	(optional) the family code of the devices to count, leave blank for all devices
 * @return the number of devices found
 */
uint8_t OneWire::count(uint8_t familyCode) {
	uint8_t found = 0;
	ONEWIRE_ADDRESS address;

	resetSearch(familyCode);
	while (search(&address)) {
		found++;
	}

	return found;
}

/**
 * Perform a search of the 1 Wire bus. The order is deterministic. You will always get
 * the same devices in the same order.
 * @param newAddress	a ONEWIRE_ADDRESS to populate with the address of the found device
 * @param retries		the number of times to retry on CRC error
 * @return true if a device was found, false otherwise
 * @see resetSearch to start a new search
 * @see targetSearch to restrict the search to a particular device type
 */
bool OneWire::search(ONEWIRE_ADDRESS *newAddress, uint8_t retries) {
	uint8_t idBitNumber = 1;
	uint8_t lastZero = 0;
	uint8_t romByteNumber = 0;
	bool searchResult = false;
	uint8_t idBit, cmpIdBit;
	uint8_t romByteMask = 1;
	bool searchDirection;

// if the last call was not the last one
	if (!_lastDeviceFlag) {
		// 1-Wire reset
		if (!reset()) {
			// reset the search
			_lastDiscrepancy = 0;
			_lastDeviceFlag = false;
			_lastFamilyDiscrepancy = 0;
			return false;
		}

		while (retries--) {
			// issue the search command
			write(0xF0);

			// loop to do the search
			do {
				// read a bit and its complement
				idBit = readBit();
				cmpIdBit = readBit();

				// check for no devices on 1-wire
				if ((idBit == 1) && (cmpIdBit == 1)) {
					break;
				} else {
					// all devices coupled have 0 or 1
					if (idBit != cmpIdBit)
						searchDirection = idBit;  // bit write value for search
					else {
						// if this discrepancy if before the Last Discrepancy
						// on a previous next then pick the same as last time
						if (idBitNumber < _lastDiscrepancy)
							searchDirection = ((_romNumber[romByteNumber] & romByteMask) > 0);
						else
							// if equal to last pick 1, if not then pick 0
							searchDirection = (idBitNumber == _lastDiscrepancy);

						// if 0 was picked then record its position in LastZero
						if (!searchDirection) {
							lastZero = idBitNumber;

							// check for Last discrepancy in family
							if (lastZero < 9)
								_lastFamilyDiscrepancy = lastZero;
						}
					}

					// set or clear the bit in the ROM byte rom_byte_number
					// with mask rom_byte_mask
					if (searchDirection) {
						_romNumber[romByteNumber] |= romByteMask;
					} else {
						_romNumber[romByteNumber] &= ~romByteMask;
					}

					// serial number search direction write bit
					writeBit(searchDirection);

					// increment the byte counter id_bit_number
					// and shift the mask rom_byte_mask
					idBitNumber++;
					romByteMask <<= 1;

					// if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
					if (romByteMask == 0) {
						romByteNumber++;
						romByteMask = 1;
					}
				}
			} while (romByteNumber < ONEWIRE_ADDRESS_BYTES);  // loop until through all ROM bytes 0-7

			if (_romNumber[ONEWIRE_CRC_BYTE] == crc8(_romNumber, 7)) {
				// Good CRC
				break;
			} else if (0 == retries) {
				// final attempt was bad
				return false;
			}
		}

		// if the search was successful then
		if (!(idBitNumber < 65)) {
			// search successful so set LastDiscrepancy,LastDeviceFlag,search_result
			_lastDiscrepancy = lastZero;

			// check for last device
			if (_lastDiscrepancy == 0)
				_lastDeviceFlag = true;

			searchResult = true;
		}
	}

// if no device found then reset counters so next 'search' will be like a first
	if (!searchResult || !_romNumber[0]) {
		_lastDiscrepancy = 0;
		_lastDeviceFlag = false;
		_lastFamilyDiscrepancy = 0;
		searchResult = false;
	}
	for (int i = 0; i < ONEWIRE_ADDRESS_BYTES; i++) {
		newAddress->bytes[i] = _romNumber[i];
	}
	return searchResult;
}

// The 1-Wire CRC scheme is described in Maxim Application Note 27:
// "Understanding and Using Cyclic Redundancy Checks with Maxim iButton Products"
//

#if ONEWIRE_CRC8_TABLE
// This table comes from Dallas sample code where it is freely reusable,
// though Copyright (C) 2000 Dallas Semiconductor Corporation
static const uint8_t PROGMEM dscrc_table[] = {0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
	157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220, 35, 125, 159, 193, 66, 28, 254, 160,
	225, 191, 93, 3, 128, 222, 60, 98, 190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255, 70,
	24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7, 219, 133, 103, 57, 186, 228, 6, 88, 25, 71,
	165, 251, 120, 38, 196, 154, 101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36, 248, 166,
	68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185, 140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242,
	172, 47, 113, 147, 205, 17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80, 175, 241, 19,
	77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238, 50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76,
	18, 145, 207, 45, 115, 202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139, 87, 9, 235, 181,
	54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22, 233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201,
	74, 20, 246, 168, 116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53};

//
// Compute a Dallas Semiconductor 8 bit CRC. These show up in the ROM
// and the registers.  (note: this might better be done without to
// table, it would probably be smaller and certainly fast enough
// compared to all those delayMicrosecond() calls.  But I got
// confused, so I use this table from the examples.)
//
uint8_t OneWire::crc8(const uint8_t *addr, uint8_t len) {
	uint8_t crc = 0;

	while (len--) {
		crc = pgm_read_byte(dscrc_table + (crc ^ *addr++));
	}
	return crc;
}
#else
//
// Compute a Dallas Semiconductor 8 bit CRC directly.
// this is much slower, but much smaller, than the lookup table.
//
uint8_t PURE OneWire::crc8(const uint8_t *addr, uint8_t len) {
	uint8_t crc = 0;

	while (len--) {
		uint8_t inbyte = *addr++;
		for (uint8_t i = 8; i; i--) {
			uint8_t mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			if (mix)
				crc ^= 0x8C;
			inbyte >>= 1;
		}
	}
	return crc;
}
#endif

/**
 * Compute the 1-Wire CRC16 and compare it against the received CRC.
 * Example usage (reading a DS2408):
 *    // Put everything in a buffer so we can compute the CRC easily.
 *    uint8_t buf[13];
 *    buf[0] = 0xF0;    // Read PIO Registers
 *    buf[1] = 0x88;    // LSB address
 *    buf[2] = 0x00;    // MSB address
 *    WriteBytes(net, buf, 3);    // Write 3 cmd bytes
 *    ReadBytes(net, buf+3, 10);  // Read 6 data bytes, 2 0xFF, 2 CRC16
 *    if (!CheckCRC16(buf, 11, &buf[11])) {
 *        // Handle error.
 *    }
 *
 * @param input - Array of bytes to checksum.
 * @param len - How many bytes to use.
 * @param inverted_crc - The two CRC16 bytes in the received data.
 *                       This should just point into the received data, *not* at a 16-bit integer.
 * @param crc - The crc starting value (optional)
 * @return True, iff the CRC matches.
 */
bool PURE OneWire::check_crc16(const uint8_t* input, uint16_t len, const uint8_t* inverted_crc, uint16_t crc) {
	crc = ~crc16(input, len, crc);
	return (crc & 0xFF) == inverted_crc[0] && (crc >> 8) == inverted_crc[1];
}

/**
 * Compute a Dallas Semiconductor 16 bit CRC.  This is required to check the integrity of data received from many 1-Wire devices.
 * Note that the CRC computed here is *not* what you'll get from the 1-Wire network, for two reasons:
 *   1) The CRC is transmitted bitwise inverted
 *   2) Depending on the endian-ness of your processor, the binary representation of the two-byte return value may have a different
 *      byte order than the two bytes you get from 1-Wire
 * @param input - Array of bytes to checksum
 * @param len - How many bytes to use
 * @param crc - The crc starting value (optional)
 * @return The CRC16, as defined by Dallas Semiconductor
 * */
uint16_t PURE OneWire::crc16(const uint8_t* input, uint16_t len, uint16_t crc) {
	static const uint8_t oddparity[16] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };

	for (uint16_t i = 0; i < len; i++) {
		// Even though we're just copying a byte from the input,
		// we'll be doing 16-bit computation with it.
		uint16_t cdata = input[i];
		cdata = (cdata ^ crc) & 0xff;
		crc >>= 8;

		if (oddparity[cdata & 0x0F] ^ oddparity[cdata >> 4])
			crc ^= 0xC001;

		cdata <<= 6;
		crc ^= cdata;
		cdata <<= 1;
		crc ^= cdata;
	}
	return crc;
}


/**
 * Constructor
 * @param bus		the bus the device is connected to
 * @param address	the address for the device
 */
OneWireDevice::OneWireDevice(OneWire &bus, ONEWIRE_ADDRESS *address) :
		_bus(bus) {
	memcpy(&_address, address, sizeof(_address));
	(void)checkParasitic();
}

/**
 * Make this the active device
 */
void OneWireDevice::select() {
	_bus.select(&_address);
}

/**
 * Query the device and see if it is parasitically powered
 * @return true if the device is parasitically powered
 */
bool OneWireDevice::checkParasitic() {
	select();
	_bus.write(0xb4);
	_parasitic = !_bus.read();
	return _parasitic;
}

/**
 * Is the device parasitically powered?
 * @return true if the device is parasitically powered
 */
bool PURE OneWireDevice::isParasitic() {
	return _parasitic;
}

/**
 * Get the address of a device
 * @return a pointer to the address
 */
ONEWIRE_ADDRESS CONST *OneWireDevice::getAddress() {
	return &_address;
}


}; // namespace flame
