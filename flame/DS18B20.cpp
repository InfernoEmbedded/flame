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

#include <flame/DS18B20.h>

namespace flame {

/**
 * Constructor
 * @param bus		the bus the device is connected to
 * @param address	the address for the device
 */
DS18B20::DS18B20(OneWire &bus, ONEWIRE_ADDRESS *address) :
		OneWireDevice(bus, address) {}

/**
 * Count the channels available on the device
 * @return the number of channels
 */
uint8_t CONST DS18B20::countChannels() {
	return 1;
}

/**
 * Read the scratchpad memory from the device
 * @param scratchpad	the output to populate
 * @return true if the read was successful
 */
bool DS18B20::readScratchpad (DS18B20_SCRATCHPAD *scratchpad) {
	_bus.reset();
	select();
	_bus.write(0xbe, OneWireDevice::_parasitic);
	_bus.read(scratchpad->data, sizeof(scratchpad->data));

	return _bus.crc8(scratchpad->data, sizeof(scratchpad->data) - 1) == scratchpad->value.crc;
}

/**
 * Set the current resolution of the device
 * @return true if the resolution was set
 */
bool DS18B20::setResolution(DS18B20Resolution resolution) {
	DS18B20_SCRATCHPAD scratchpad;

	readScratchpad(&scratchpad);
	scratchpad.value.config.resolution = resolution;
	return writeScratchpad(&scratchpad);
}

/**
 * Write to the scratchpad memory on the device
 * @param scratchpad	the data to write
 * @return true if the write was successful
 */
bool DS18B20::writeScratchpad (DS18B20_SCRATCHPAD *scratchpad) {
	_bus.reset();
	select();
	_bus.write(0x4e, OneWireDevice::_parasitic);
	_bus.write(scratchpad->data + 3, 3);

	DS18B20_SCRATCHPAD newScratchpad;
	if (!readScratchpad(&newScratchpad)) {
		return false;
	}

	return scratchpad->value.th.value == newScratchpad.value.th.value &&
			scratchpad->value.th.negative == newScratchpad.value.th.negative &&
			scratchpad->value.tl.value == newScratchpad.value.tl.value &&
			scratchpad->value.tl.negative == newScratchpad.value.tl.negative &&
			scratchpad->value.config.resolution == newScratchpad.value.config.resolution;
}

/**
 * Get the read delay for the current resolution (required for parasitically powered devices, as the bus must remain
 * powered for this time)
 * @return the read delay in ms, or 0 if an error occurred
 */
uint16_t DS18B20::getReadDelay() {
	DS18B20_SCRATCHPAD scratchpad;
	if (!readScratchpad(&scratchpad)) {
		return 0;
	}

	switch (scratchpad.value.config.resolution) {
	case 0:
		return 94;
	case 1:
		return 188;
	case 2:
		return 375;
	case 3:
		return 750;
	}

	return 0;
}

/**
 * Get the current resolution
 * @return the read delay in ms, or 0 if an error occurred
 */
uint8_t DS18B20::getResolution() {
	DS18B20_SCRATCHPAD scratchpad;
	if (!readScratchpad(&scratchpad)) {
		return 0;
	}

	switch (scratchpad.value.config.resolution) {
	case 0:
		return 9;
	case 1:
		return 10;
	case 2:
		return 11;
	case 3:
		return 12;
	}

	return 0;
}


/**
 * Commit the scratchpad contents to EEPROM
 */
void DS18B20::copyScratchpad() {
	_bus.reset();
	select();
	_bus.write(0x48, OneWireDevice::_parasitic);
}

/**
 * Initiate a reading
 * @param channel	the channel to read (unused, there is only 1 channel on this device)
 * @post must wait at least 750ms if the device is parasitically powered, otherwise call isReady() to poll the device
 */
void DS18B20::read(uint8_t UNUSED(channel)) {
	_bus.reset();
	select();
	_bus.write(0x44, OneWireDevice::_parasitic);
}

/**
 * Get the temperature from the device
 * @pre read or readall must be called, and isReady has returned true or the read delay has elapsed (if the device is parasitically powered)
 */
void DS18B20::fetchTemperature() {
	DS18B20_SCRATCHPAD scratchpad;
	if (readScratchpad(&scratchpad)) {
		Temperature temperature = (float)scratchpad.value.temperature / 16;
		if (scratchpad.value.temperatureNegative) {
			temperature *= -1;
		}
		setTemperature(0, temperature);
	}
}

/**
 * Check if the reading is complete
 * @return true if the reading is ready, false if is not or if the device is parasitically powered
 */
bool DS18B20::isReady() {
	if (_parasitic) {
		return false;
	}

	_bus.reset();
	select();

	if (_bus.read() != 0) {
		fetchTemperature();
		return true;
	}

	return false;
}

/**
 * Initiate a reading on all devices on the bus
 * @post must wait at least 750ms if any of the devices are parasitically powered, otherwise call isReady() to poll each device
 */
void DS18B20Factory::readAll() {
	_bus.reset();
	_bus.skip();
	_bus.write(0x44, true);
}


}; // namespace flame
