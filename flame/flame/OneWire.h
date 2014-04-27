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

#ifndef OneWire_h
#define OneWire_h

#include <flame/io.h>
#include <flame/Pin.h>
#include <util/delay.h>
#include <stdlib.h>

#define RX_BUFFER_SIZE	3
#define TX_ELEMENTS_COUNT 32
#include <flame/HardwareSerial.h>
FLAME_HARDWARESERIAL_IMPORT(serial, RX_BUFFER_SIZE, TX_ELEMENTS_COUNT, FLAME_USART0, 115200);


namespace flame {

// Select the table-lookup method of computing the 8-bit CRC
// by setting this to 1.  The lookup table enlarges code size by
// about 250 bytes.  It does NOT consume RAM (but did in very
// old versions of OneWire).  If you disable this, a slower
// but very compact algorithm is used.
#ifndef ONEWIRE_CRC8_TABLE
#define ONEWIRE_CRC8_TABLE 0
#endif

#define ONEWIRE_ADDRESS_BYTES	8
#define ONEWIRE_CRC_BYTE		7

struct OneWireAddress {
	uint8_t bytes[ONEWIRE_ADDRESS_BYTES];
};
typedef struct OneWireAddress ONEWIRE_ADDRESS;

class OneWire {
protected:
	Pin	&_dataPin;

	// global search state
	unsigned char _romNumber[ONEWIRE_ADDRESS_BYTES];
	uint8_t _lastDiscrepancy = 0;
	uint8_t _lastFamilyDiscrepancy = 0;
	bool _lastDeviceFlag = false;

public:
	OneWire(Pin &dataPin);
	bool reset();
	void select(OneWireAddress *address);
	void skip();
	void write(uint8_t value, bool power = false);
	void write(const uint8_t *buf, uint16_t length, bool power = false);
	uint8_t read();
	void read(uint8_t *buf, uint16_t length);
	void writeBit(bool value);
	bool readBit();
	void depower();
	void resetSearch(uint8_t familyCode = 0);
	bool search(ONEWIRE_ADDRESS *newAddress, uint8_t retries = 8);
	uint8_t scan(ONEWIRE_ADDRESS **addresses, uint8_t familyCode = 0);
	uint8_t count(uint8_t familyCode = 0);
	static uint8_t crc8(const uint8_t *addr, uint8_t len);
	static bool check_crc16(const uint8_t* input, uint16_t len, const uint8_t* inverted_crc, uint16_t crc = 0);
	static uint16_t crc16(const uint8_t* input, uint16_t len, uint16_t crc = 0);
}; // class OneWire


template<class DeviceClass, uint8_t familyCode>
class OneWireDeviceFactory {
protected:
	OneWire	&_bus;
	DeviceClass **_devices;
	uint8_t _deviceCount;

public:
	/**
	 * Constructor
	 * @param bus	the bus to generate devices from
	 */
	OneWireDeviceFactory(OneWire &bus) : _bus(bus) {
		scan();
	}

	/**
	 * Destructor
	 */
	~OneWireDeviceFactory() {
		if (NULL != _devices) {
			for (uint8_t device = 0; device < _deviceCount; device++) {
				free(_devices[device]);
			}
			free(_devices);
		}
	}

	/**
	 * Count the number of device instances found on the bus
	 */
	uint8_t count() {
		return _deviceCount;
	}

	/**
	 * Scan the 1 wire bus and find the all the devices with our family code
	 * @param devices an array of devices found that will be malloced (the caller must free)
	 */
	void scan() {
		if (NULL != _devices) {
			free(_devices);
		}

		_deviceCount = _bus.count(familyCode);

		_devices = (DeviceClass **)malloc(_deviceCount * sizeof(*_devices));
		if (NULL == _devices) {
			_deviceCount = 0;
			return;
		}

		for (uint8_t device = 0; device < _deviceCount; device++) {
			ONEWIRE_ADDRESS address;
			if (!_bus.search(&address)) {
				for (uint8_t deviceToFree = 0; deviceToFree < device; deviceToFree++) {
					free(_devices[deviceToFree]);
				}
				free(_devices);
				_deviceCount = 0;
				return;
			}

			_devices[device] = new DeviceClass(_bus, &address);
		}
	}

	/**
	 * Get a device instance
	 * @param device	the device instance to get
	 * @return a pointer to the device
	 */
	DeviceClass *getDevice(uint8_t device) {
		return _devices[device];
	}
};

class OneWireDevice {
protected:
	ONEWIRE_ADDRESS	_address;
	OneWire	&_bus;
	bool _parasitic = false;

	bool checkParasitic();

public:
	OneWireDevice(OneWire &bus, ONEWIRE_ADDRESS *address);
	void select();
	bool isParasitic();
	ONEWIRE_ADDRESS *getAddress();

};

}; // namespace flame

#endif
