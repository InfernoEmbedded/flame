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

#include <flame/WaveGenerator.h>
#include <string.h>

namespace flame {

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
WaveGenerator::WaveGenerator(DAConverter &dac, SAMPLE *sampleBuffer, uint8_t sampleLength,
		uint32_t sampleRate, VOICE *voices, uint8_t voiceCount) :
		_dac(dac),
		_samples(sampleBuffer),
		_sampleLength(sampleLength),
		_sampleRate(sampleRate),
		_voices(voices),
		_voiceCount(voiceCount) {
	for (uint8_t i = 0; i < _voiceCount; i++) {
		_voices[i].phase = VoicePhase::AVAILABLE;
	}

// Kick start the DAC, it will come back to us when it needs more data
	moreSamples(&_dac, _samples, _sampleLength);
}

/**
 * Switch the current phase if required
 * @param	voice	the voice to switch the phase of
 */
inline void WaveGenerator::switchPhase(VOICE &voice) {
	switch (voice.phase) {
	case VoicePhase::ATTACK:
		if (voice.phaseOffset == voice.instrument->attackTime) {
			voice.phaseOffset = 0;
			voice.phase = VoicePhase::DECAY1;
		}
		break;
	case VoicePhase::DECAY1:
		if (voice.phaseOffset == voice.instrument->decay1Time) {
			voice.phaseOffset = 0;
			voice.phase = VoicePhase::SUSTAIN1;
		}
		break;
	case VoicePhase::SUSTAIN1:
		if (voice.phaseOffset == voice.instrument->sustain1Time) {
			voice.phaseOffset = 0;
			voice.phase = VoicePhase::DECAY2;
		}
		break;
	case VoicePhase::DECAY2:
		if (voice.phaseOffset == voice.instrument->decay1Time) {
			voice.phaseOffset = 0;
			voice.phase = VoicePhase::SUSTAIN2;
		}
		break;
	case VoicePhase::RELEASE:
		if (voice.phaseOffset == voice.instrument->releaseTime) {
			voice.phase = VoicePhase::AVAILABLE;
		}
		break;
	default:
		break;
	}

	if (voice.currentOffset == voice.duration) {
		voice.phaseOffset = 0;
		voice.phase = VoicePhase::RELEASE;
	}
}


/**
 * Calculate the current amplitude
 * @param voice	the voice to work on
 * @return the current amplitude
 */
inline AMPLITUDE WaveGenerator::currentAmplitude(VOICE &voice) {
	uint32_t amplitude = voice.velocity;

	switch (voice.phase) {
	case VoicePhase::ATTACK:
		amplitude *= voice.phaseOffset;
		amplitude /= voice.instrument->attackTime;
		voice.amplitude = amplitude;
		break;
	case VoicePhase::DECAY1:
		amplitude -= voice.phaseOffset * (voice.velocity - voice.instrument->decay1Amount) / voice.instrument->decay1Time;
		voice.amplitude = amplitude;
		break;
	case VoicePhase::SUSTAIN1:
		amplitude *= voice.instrument->decay1Amount;
		voice.amplitude = amplitude;
		break;
	case VoicePhase::DECAY2:
		amplitude -= voice.phaseOffset * (voice.velocity - voice.instrument->decay2Amount) / voice.instrument->decay2Time;
		voice.amplitude = amplitude;
		break;
	case VoicePhase::SUSTAIN2:
		amplitude *= voice.instrument->decay2Amount;
		voice.amplitude = amplitude;
		break;
	case VoicePhase::RELEASE:
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
inline SAMPLE WaveGenerator::renderVoice(VOICE &voice) {
	if (voice.phase == VoicePhase::AVAILABLE) {
		return 0;
	}

	uint8_t sampleOffset = uint32_t(voice.currentOffset * voice.frequency / 440)  % 72;
	SAMPLE sample = pgm_read_byte(voice.instrument->samples + sampleOffset);

// Calculate amplitude
	AMPLITUDE amplitude = currentAmplitude(voice);

	int16_t bigSample = sample * amplitude;
	bigSample /= _voiceCount;
	bigSample /= 256;


	voice.currentOffset++;
	voice.phaseOffset++;

	switchPhase(voice);

	return (SAMPLE)bigSample;
}

/**
 * Render the current instruments to the buffer
 */
inline void WaveGenerator::renderBuffer() {
	memset(_samples, 0, _sampleLength);

	for (uint8_t i = 0; i < _sampleLength; i++) {
		for (uint8_t voice = 0; voice < _voiceCount; voice++) {
			_samples[i] += renderVoice(_voices[voice]);
		}
	}
}

/**
 * Send more samples to the DAC
 */
void WaveGenerator::moreSamples(DAConverter *dac, UNUSED SAMPLE *oldSamples, UNUSED uint8_t sampleLength) {
	renderBuffer();
	dac->playSamples(_samples, _sampleLength);
}

/**
 * Play a note
 * @return true if the note could not be played
 */
bool WaveGenerator::play(INSTRUMENT *instrument, uint16_t frequency, AMPLITUDE velocity, uint32_t duration) {
// Find a free voice
	uint8_t voice;
	for (voice = 0; voice < _voiceCount; voice++) {
		if (VoicePhase::AVAILABLE == _voices[voice].phase) {
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
	_voices[voice].phase = VoicePhase::ATTACK;

	return false;
}

}
