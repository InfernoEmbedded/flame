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

#ifndef DS18B20_H_
#define DS18B20_H_

#include <flame/OneWire.h>
#include <flame/Temperature.h>

namespace flame {

#define DS18B20_FAMILYCODE	0x28
#define DS18B20_WRITE_SCRATCHCPAD_PARASITIC_DURATION	10 //< time to hold the bus high when writing the scratchpad data (milliseconds)

//enum class DS18B20Resolution : uint8_t {
//	BITS_9	= 0,
//	BITS_10	= 1,
//	BITS_11	= 2,
//	BITS_12	= 3
//};

typedef uint8_t DS18B20Resolution;

union ds18b20Scratchpad {
	uint8_t data[9];
	struct {
		uint16_t temperature:11;
		uint16_t temperatureNegative:5;
		struct {
			uint8_t	value:7;
			bool 	negative:1;
		} th;
		struct {
			uint8_t	value:7;
			bool 	negative:1;
		} tl;
		struct {
			uint8_t unused:1;
			DS18B20Resolution resolution:2;
			uint8_t reserved:5;
		} config;
		uint8_t reserved[3];
		uint8_t crc;
	} value;
}; // union ds18b20Scratchpad
typedef union ds18b20Scratchpad DS18B20_SCRATCHPAD;

class DS18B20 : public OneWireDevice, public Thermometer {
protected:

public:
	DS18B20(OneWire &bus, ONEWIRE_ADDRESS *address);

	uint8_t countChannels();
	void read(uint8_t channel);
	bool isReady();
	bool readScratchpad(DS18B20_SCRATCHPAD *scratchpad);
	bool writeScratchpad(DS18B20_SCRATCHPAD *scratchpad);
	void copyScratchpad();
	bool setResolution(DS18B20Resolution resolution);
	uint8_t getResolution();
	uint16_t getReadDelay();
	void fetchTemperature();
	// Todo: Add alarm functionality

}; // class DS18B20

class DS18B20Factory : public OneWireDeviceFactory<DS18B20, DS18B20_FAMILYCODE> {
protected:

public:
	/**
	 * Constructor
	 * @param bus	the bus to look for devices on
	 */
	DS18B20Factory(OneWire &bus) :
				OneWireDeviceFactory<DS18B20, DS18B20_FAMILYCODE>(bus) {}
	void readAll();
}; // class DS18B20Factory



}; // namespace flame

#endif
