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

#include "MHV_Timer.h"

/**
 * Attach a listener to channel 1
 * @param listener	the listener to attach
 */
void MHV_Timer::setListener1(MHV_TimerListener &listener) {
	_listener1 = &listener;
}

/**
 * Attach a listener to channel 2
 * @param listener	the listener to attach
 */
void MHV_Timer::setListener2(MHV_TimerListener &listener) {
	_listener2 = &listener;
}

/**
 * Attach a listener to channel 3
 * @param listener	the listener to attach
 */
void MHV_Timer::setListener3(MHV_TimerListener &listener) {
	_listener3 = &listener;
}

/**
 * Attach a listener to channel 1
 * @param listener	the listener to attach
 */
void MHV_Timer::setListener1(MHV_TimerListener *listener) {
	_listener1 = listener;
}

/**
 * Attach a listener to channel 2
 * @param listener	the listener to attach
 */
void MHV_Timer::setListener2(MHV_TimerListener *listener) {
	_listener2 = listener;
}

/**
 * Attach a listener to channel 3
 * @param listener	the listener to attach
 */
void MHV_Timer::setListener3(MHV_TimerListener *listener) {
	_listener3 = listener;
}

/**
 * Set the prescaler (to be used when enabled)
 * @param prescaler	the desired prescaler
 */
void MHV_Timer::setPrescaler(MHV_TIMER_PRESCALER prescaler) {
	_prescaler = prescaler;
}

/**
 * Trigger the listener for channel 2
 */
void MHV_Timer::trigger2() {
	if (_listener2) {
		_listener2->alarm();
	}
}

/**
 * Trigger the listener for channel 3
 */
void MHV_Timer::trigger3() {
	if (_listener3) {
		_listener3->alarm();
	}
}


/**
 * Check if the timer is enabled
 * @return	true if the timer is enabled
 */
bool MHV_Timer::enabled() {
	return getPrescaler();
}
