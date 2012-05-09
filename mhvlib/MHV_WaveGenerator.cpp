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

#include <MHV_WaveGenerator.h>

/**
 * Generate waveforms and write them to the DAC
 *
 * @param	dac				the DAC to write to
 * @param	sampleBuffer	a buffer of samples to write to, used to communicate with the DAC
 * @param	sampleLength	the length of the sample buffer
 * @param	sampleRate		the sample rate of the DAC
 * @param	voices			the voices to use to generate sound
 * @param	voiceCount		the number of voices
 */
MHV_WaveGenerator::MHV_WaveGenerator(MHV_DAC &dac, MHV_SAMPLE *sampleBuffer, uint8_t sampleLength,
		uint32_t sampleRate, MHV_VOICE *voices, uint8_t voiceCount) :
		_dac(dac),
		_samples(sampleBuffer),
		_sampleLength(sampleLength),
		_sampleRate(sampleRate),
		_voices(voices),
		_voiceCount(voiceCount) {
	for (uint8_t i = 0; i < _voiceCount; i++) {
		_voices[i].phase = MHV_VOICE_AVAILABLE;
	}

// Kick start the DAC, it will come back to us when it needs more data
	moreSamples(&_dac, _samples, _sampleLength);
}

/**
 * Switch the current phase if required
 * @param	voice	the voice to switch the phase of
 */
inline void MHV_WaveGenerator::switchPhase(MHV_VOICE &voice) {
	switch (voice.phase) {
	case MHV_VOICE_ATTACK:
		if (voice.phaseOffset == voice.instrument->attackTime) {
			voice.phaseOffset = 0;
			voice.phase = MHV_VOICE_DECAY1;
		}
		break;
	case MHV_VOICE_DECAY1:
		if (voice.phaseOffset == voice.instrument->decay1Time) {
			voice.phaseOffset = 0;
			voice.phase = MHV_VOICE_SUSTAIN1;
		}
		break;
	case MHV_VOICE_SUSTAIN1:
		if (voice.phaseOffset == voice.instrument->sustain1Time) {
			voice.phaseOffset = 0;
			voice.phase = MHV_VOICE_DECAY2;
		}
		break;
	case MHV_VOICE_DECAY2:
		if (voice.phaseOffset == voice.instrument->decay1Time) {
			voice.phaseOffset = 0;
			voice.phase = MHV_VOICE_SUSTAIN2;
		}
		break;
	case MHV_VOICE_RELEASE:
		if (voice.phaseOffset == voice.instrument->releaseTime) {
			voice.phase = MHV_VOICE_AVAILABLE;
		}
		break;
	default:
		break;
	}

	if (voice.currentOffset == voice.duration) {
		voice.phaseOffset = 0;
		voice.phase = MHV_VOICE_RELEASE;
	}
}


/**
 * Calculate the current amplitude
 * @param voice	the voice to work on
 * @return the current amplitude
 */
inline MHV_AMPLITUDE MHV_WaveGenerator::currentAmplitude(MHV_VOICE &voice) {
	uint32_t amplitude = voice.velocity;

	switch (voice.phase) {
	case MHV_VOICE_ATTACK:
		amplitude *= voice.phaseOffset;
		amplitude /= voice.instrument->attackTime;
		voice.amplitude = amplitude;
		break;
	case MHV_VOICE_DECAY1:
		amplitude -= voice.phaseOffset * (voice.velocity - voice.instrument->decay1Amount) / voice.instrument->decay1Time;
		voice.amplitude = amplitude;
		break;
	case MHV_VOICE_SUSTAIN1:
		amplitude *= voice.instrument->decay1Amount;
		voice.amplitude = amplitude;
		break;
	case MHV_VOICE_DECAY2:
		amplitude -= voice.phaseOffset * (voice.velocity - voice.instrument->decay2Amount) / voice.instrument->decay2Time;
		voice.amplitude = amplitude;
		break;
	case MHV_VOICE_SUSTAIN2:
		amplitude *= voice.instrument->decay2Amount;
		voice.amplitude = amplitude;
		break;
	case MHV_VOICE_RELEASE:
		amplitude = voice.amplitude * (voice.instrument->releaseTime - voice.phaseOffset) / voice.instrument->releaseTime;
		break;
	default:
		break;
	}

	return amplitude;
}

/**
 * Get the value of a single sample of a voice
 * @param	voice	the voice to render
 * @return the value of the sample
 */
inline MHV_SAMPLE MHV_WaveGenerator::renderVoice(MHV_VOICE &voice) {
	if (voice.phase == MHV_VOICE_AVAILABLE) {
		return 0;
	}

	uint8_t sampleOffset = uint32_t(voice.currentOffset * voice.frequency / 440)  % 72;
	MHV_SAMPLE sample = pgm_read_byte(voice.instrument->samples + sampleOffset);

// Calculate amplitude
	MHV_AMPLITUDE amplitude = currentAmplitude(voice);

	int16_t bigSample = sample * amplitude;
	bigSample /= _voiceCount;
	bigSample /= 256;


	voice.currentOffset++;
	voice.phaseOffset++;

	switchPhase(voice);

	return (MHV_SAMPLE)bigSample;
}

/**
 * Render the current instruments to the buffer
 */
inline void MHV_WaveGenerator::renderBuffer() {
	mhv_memClear(_samples, _sampleLength);

	for (uint8_t i = 0; i < _sampleLength; i++) {
		for (uint8_t voice = 0; voice < _voiceCount; voice++) {
			_samples[i] += renderVoice(_voices[voice]);
		}
	}
}

/**
 * Send more samples to the DAC
 */
void MHV_WaveGenerator::moreSamples(MHV_DAC *dac, UNUSED MHV_SAMPLE *oldSamples, UNUSED uint8_t sampleLength) {
	renderBuffer();
	dac->playSamples(_samples, _sampleLength);
}

/**
 * Play a note
 * @return true if the note could not be played
 */
bool MHV_WaveGenerator::play(MHV_INSTRUMENT *instrument, uint16_t frequency, MHV_AMPLITUDE velocity, uint32_t duration) {
// Find a free voice
	uint8_t voice;
	for (voice = 0; voice < _voiceCount; voice++) {
		if (MHV_VOICE_AVAILABLE == _voices[voice].phase) {
			break;
		}
	}
	if (voice == _voiceCount) {
		return true;
	}

	_voices[voice].instrument = instrument;
	_voices[voice].velocity = velocity;
	_voices[voice].frequency = frequency;
	_voices[voice].duration = duration;
	_voices[voice].phase = MHV_VOICE_ATTACK;

	return false;
}
