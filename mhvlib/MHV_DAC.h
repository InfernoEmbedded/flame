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

#ifndef MHVDAC_H_
#define MHVDAC_H_

#include <MHV_Timer.h>

typedef int8_t MHV_SAMPLE;

class MHV_DAC;

/**
 * A listener which will be notified when the DAC needs more samples
 */
class MHV_DACListener {
public:
	/**
	 * Called when the DAC needs more samples
	 * @param dac			the DAC that needs more samples
	 * @param oldSamples	a pointer the the old samples (so it can be reused/freed)
	 * @param sampleLength	the number of old samples
	 */
	virtual void moreSamples(MHV_DAC *dac, MHV_SAMPLE *oldSamples, uint8_t sampleLength);

	virtual ~MHV_DACListener();
};

class MHV_DAC : public MHV_TimerListener {
protected:
	MHV_SAMPLE		*_samples;
	uint8_t			_sampleLength;
	uint8_t			_currentSample;
	MHV_Timer		&_timer;
	MHV_DACListener	&_listener;

public:
	MHV_DAC(MHV_Timer &timer, MHV_DACListener &listener);
	void alarm();
	void playSamples(MHV_SAMPLE *samples, uint8_t sampleLength);
	void handleEvents();
};

#endif /* MHVDAC_H_ */
