/*
 * Copyright (c) 2012, Make, Hack, Void Inc
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

#ifndef MHV_SOFTWAREPWM_H_
#define MHV_SOFTWAREPWM_H_

#include <mhvlib/Timer.h>

namespace mhvlib {

class SoftwarePWMListener {
public:
	uint8_t	when;

	virtual void reset() =0;
	virtual void set() =0;
	virtual bool check() =0;

	void setDutyCycle(uint8_t dutyCycle) {
		when = dutyCycle;
	}

	uint8_t getDutyCycle() {
		return when;
	}
};

template<MHV_DECLARE_PIN(pin)>
class SoftwarePWMPin : public SoftwarePWMListener {
public:
	SoftwarePWMPin() {
		setOutput(MHV_PIN_PARMS(pin));
	}
	void reset() {
		pinOff(MHV_PIN_PARMS(pin));
	}
	void set() {
		pinOn(MHV_PIN_PARMS(pin));
	}
	bool check() {
		return pinRead(MHV_PIN_PARMS(pin));
	}
};


template<uint8_t listenerCount=8>
class SoftwarePWM : public TimerListener {
private:
	Timer					&_timer;
	SoftwarePWMListener		*_listeners[listenerCount];
	uint8_t					_listenerCount;
	uint8_t					_ticks;

public:
	/**
	 * Create a new software PWM manager
	 * @param timer			the timer to drive the manager with
	 */
	SoftwarePWM(Timer &timer) :
		_timer(timer),
		_listenerCount(0),
		_ticks(0) {}

	/**
	 * Register a listener
	 * @param 	listener		the listener to register
	 * @return the index of listener, or 0 if the add failed
	 */
	uint8_t addListener(SoftwarePWMListener &listener) {
		if (_listenerCount == listenerCount) {
			return 0;
		}

		_listeners[_listenerCount] = &listener;
		_listeners[_listenerCount]->when = 0;
		_listeners[_listenerCount]->reset();

		return _listenerCount++;
	}

	/**
	 * Handle the timer interrupt by setting the outputs
	 */
	void alarm() {
		if (!_ticks++) {
			for (uint8_t i = 0; i < _listenerCount; i++) {
				_listeners[i]->set();
			}
		} else {
			for (uint8_t i = 0; i < _listenerCount; i++) {
				if (_listeners[i]->check() && _listeners[i]->when < _ticks) {
					_listeners[i]->reset();
				}
			}
		}
	}
};

}
#endif /* MHV_SOFTWAREPWM_H_ */
