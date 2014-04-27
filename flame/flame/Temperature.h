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

#ifndef FLAME_TEMPERATURE_H_
#define FLAME_TEMPERATURE_H_

#include <stdlib.h>


namespace flame {

typedef float	Temperature;

class TemperatureListener {
public:
	/**
	 * Called when a new temperature reading is available
	 * @param channel	the channel
	 * @param celcius	the temperature in celcius
	 */
	virtual void newTemperature(uint8_t channel, Temperature celcius) =0;
};

class Thermometer {
protected:
	TemperatureListener **_listeners;
	Temperature *_temperatures;

	/**
	 * Handle a reading
	 * @param channel	the channel for the reading
	 * @param celcius	the temperature in celcius
	 */
	void setTemperature(uint8_t channel, Temperature celcius) {
		_temperatures[channel] = celcius;
		if (NULL != _listeners[channel]) {
			_listeners[channel]->newTemperature(channel, celcius);
		}
	}

public:
	/**
	 * Constructor
	 */
	Thermometer() {
		uint8_t channels = countChannels();

		_listeners = (TemperatureListener **)malloc(channels * sizeof(*_listeners));
		_temperatures = (Temperature *)malloc(channels * sizeof(*_temperatures));
		for (uint8_t channel = 0; channel < channels; channel++) {
			_listeners[channel] = NULL;
			_temperatures[channel] = 0.0f;
		}
	}

	/**
	 * Destructor
	 */
	~Thermometer() {
		free(_listeners);
	}

	/**
	 * Get the number of channels available
	 * @return the number of channels available
	 */
	virtual uint8_t countChannels() {
		return 1;
	}

	/**
	 * Initiate a reading
	 * @param channel	the channel to read
	 */
	virtual void read(uint8_t channel) =0;

	/**
	 * Register a listener
	 * @param channel	the channel to register the listener with
	 * @param listener	the listener to register
	 */
	void registerListener(uint8_t channel, TemperatureListener &listener) {
		if (channel >= countChannels()) {
			return;
		}

		_listeners[channel] = &listener;
	}

	/**
	 * Register a listener
	 * @param channel	the channel to register the listener with
	 * @param listener	the listener to register
	 */
	void registerListener(uint8_t channel, TemperatureListener *listener) {
		if (channel >= countChannels()) {
			return;
		}

		_listeners[channel] = listener;
	}

	/**
	 * Get the last known temperature
	 */
	Temperature getTemperature(uint8_t channel) {
		return _temperatures[channel];
	}

};

} // namespace flame
#endif /* FLAME_TEMPERATURE_H_ */
