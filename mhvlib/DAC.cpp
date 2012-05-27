/* Copyright (c) 2011, Make, Hack, Void Inc
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

#include <mhvlib/DAC.h>

namespace mhvlib_bsd {

/**
 * Create a new DAC to play samples by PWMing a timer
 * @param	timer		the timer to PWM - we will be calling setOutput2 on this to adjust the output
 * @param	listener	a listener we will notify when we need more samples to play
 */
DAConverter::DAConverter(Timer &timer, DACListener &listener) :
	_timer(timer),
	_listener(listener) {}

/**
 * Play the next sample if available
 */
void DAConverter::alarm() {
	if (_currentSample == _sampleLength) {
		return;
	}

	_timer.setOutput2(_samples[_currentSample++]);
}

/**
 * Play a set of samples (up to 255)
 * Note that this will clobber the current sample set if there is one, and the listener will not be called on it!
 *
 * @param	samples			the samples to play
 * @param	sampleLength	the number of samples to play
 */
void DAConverter::playSamples(SAMPLE *samples, uint8_t sampleLength) {
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		_samples = samples;
		_sampleLength = sampleLength;
		_currentSample = 0;
	}
}

/**
 * Handle any non-interrupt context events for the DAC
 */
void DAConverter::handleEvents() {
	if (_currentSample == _sampleLength) {
		_listener.moreSamples(this, _samples, _sampleLength);
	}
}

}
