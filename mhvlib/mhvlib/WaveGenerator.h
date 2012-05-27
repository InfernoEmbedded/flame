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

#ifndef MHVWAVEGENERATOR_H_
#define MHVWAVEGENERATOR_H_

#include <mhvlib/io.h>
#include <mhvlib/DAC.h>
#include <avr/pgmspace.h>

/**
 * Waveform amplification envelope
 *
 *    Attack
 * A ^
 * m |        Decay1
 * p |
 * l |        ^
 * i |       / \
 * t |      /   \ Sustain1                Note off here
 * u |     /     \________ Decay2         |
 * d |    /               \ Sustain2      v
 * e |   /                 \_______________ Release
 *      /                                  \
 *     /                                    \
 *     Samples ------------------------------>
 *     t(0)
 */

typedef uint8_t MHV_AMPLITUDE;

struct mhv_instrument {
	const MHV_SAMPLE 			*samples;		/** The samples for a single/part cycle of the instrument at 440Hz (middle A),
													samples at 32kHz, 72 samples long, stored in progmem */
	uint16_t					attackTime;		/** samples to ramp from 0 to 100% when starting playback */
	uint16_t					decay1Time;		/** samples to ramp from 100% to amount/255 * 100% after the attack */
	MHV_AMPLITUDE				decay1Amount;	/** the amplitude to decay to */
	uint16_t					sustain1Time;	/** samples spent in sustain until decay2 */
	uint16_t					decay2Time;		/** samples spent in decay2 */
	MHV_AMPLITUDE				decay2Amount;   /** the amplitude to decay to */
	uint16_t					releaseTime;	/** samples that playback will continue for when the note is released */
};
typedef struct mhv_instrument MHV_INSTRUMENT;

enum mhv_voicePhase {
	MHV_VOICE_AVAILABLE,
	MHV_VOICE_ATTACK,
	MHV_VOICE_DECAY1,
	MHV_VOICE_SUSTAIN1,
	MHV_VOICE_DECAY2,
	MHV_VOICE_SUSTAIN2,
	MHV_VOICE_RELEASE,
};
typedef enum mhv_voicePhase MHV_VOICE_PHASE;

struct mhv_voice {
	MHV_INSTRUMENT			*instrument;		/** The instrument playing on the voice */
	uint16_t				phaseOffset;		/** The current sample offset for the current phase */
	uint32_t				currentOffset;  	/** The current sample offset for the voice */
	MHV_VOICE_PHASE			phase;				/** The current phase this voice is in */
	MHV_AMPLITUDE			velocity;			/** The overall amplitude of the envelope */
	uint16_t				frequency;			/** The frequency in Hz */
	uint32_t				duration;			/** The duration of the note, samples */
	MHV_AMPLITUDE			amplitude;			/** The current amplitude */
};
typedef struct mhv_voice MHV_VOICE;

class MHV_WaveGenerator : public MHV_DACListener {
private:
	MHV_DAC		&_dac;
	MHV_SAMPLE	*_samples;
	uint8_t		_sampleLength;
	uint32_t	_sampleRate;
	MHV_VOICE	*_voices;
	uint8_t		_voiceCount;

	inline void renderBuffer();
	inline MHV_AMPLITUDE currentAmplitude(MHV_VOICE &voice);
	inline void switchPhase(MHV_VOICE &voice);
	inline MHV_SAMPLE renderVoice(MHV_VOICE &voice);

public:
	MHV_WaveGenerator(MHV_DAC &dac, MHV_SAMPLE *sampleBuffer, uint8_t sampleLength, uint32_t sampleRate, MHV_VOICE *voices, uint8_t voiceCount);
	void moreSamples(MHV_DAC *dac, MHV_SAMPLE *oldSamples, uint8_t sampleLength);
	bool play(MHV_INSTRUMENT *instrument, uint16_t frequency, MHV_AMPLITUDE velocity, uint32_t duration);
};

#endif /* MHVWAVEGENERATOR_H_ */
