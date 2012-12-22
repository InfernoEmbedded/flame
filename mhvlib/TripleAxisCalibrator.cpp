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

#include <mhvlib/TripleAxisCalibrator.h>
#include <stdlib.h>

namespace mhvlib {

// Allow up to MAX_DIFFERENCE counts per axis to be considered a good sample
#define MAX_DIFFERENCE	3

#define DIRECT_USER(message) \
	do { \
		*feedback = PSTR(message); \
		if (_lastMessage == *feedback) { \
			*feedback = NULL; \
		} else { \
			_lastMessage = *feedback; \
		} \
	} while (0);

/**
 * Check if the sensor is steady, and complain if it is not
 * @param	sample	the sample
 * @param	feedback	a pointer that will be set to a progmem string to prompt action from the user
 * 						(or NULL)
 * @return true if the sample was good
 */
bool TripleAxisCalibrator::isSampleGood(union Int3Axis *sample, PGM_P *feedback) {
	for (uint8_t i = 0; i < 3; i++) {
		if (abs(sample->value[i] - _previousSample.value[i]) > MAX_DIFFERENCE) {
			DIRECT_USER("Hold it steady");

			memCopy(&_previousSample, sample, sizeof(_previousSample));
			return false;
		}
	}
	return true;
}

/**
 * Send a sample in to the calibrator if it is good enough
 * Prompt the user with directions if it isn't
 * @param	sample		the sample to send
 * @param	feedback	a pointer that will be set to a progmem string to prompt action from the user
 * 						(or NULL)
 * @return true if more samples are required
 */
bool TripleAxisCalibrator::pushSample(union Int3Axis *sample, PGM_P *feedback) {
	*feedback = NULL;

	switch (_state) {
	case SETUP:
		DIRECT_USER ("Place the bottom pointing down");
		_currentSamples = 0;
		return true;
		break;

	case BOTTOM_DOWN:
		if (isSampleGood(sample, feedback)) {
			_currentSamples++;
			addObservation(sample);

			if (_currentSamples > _samplesToTake) {
				DIRECT_USER ("Place the top pointing down");
				_state = TOP_DOWN;
				return true;
			}
		}
		break;

	case TOP_DOWN:
		if (isSampleGood(sample, feedback)) {
			_currentSamples++;
			addObservation(sample);

			if (_currentSamples > _samplesToTake) {
				DIRECT_USER ("Place the left side pointing down");
				_state = LEFT_DOWN;
				return true;
			}
		}
		break;

	case LEFT_DOWN:
		if (isSampleGood(sample, feedback)) {
			_currentSamples++;
			addObservation(sample);

			if (_currentSamples > _samplesToTake) {
				DIRECT_USER ("Place the right side pointing down");
				_state = RIGHT_DOWN;
				return true;
			}
		}
		break;

	case RIGHT_DOWN:
		if (isSampleGood(sample, feedback)) {
			_currentSamples++;
			addObservation(sample);

			if (_currentSamples > _samplesToTake) {
				DIRECT_USER ("Place the front pointing down");
				_state = FRONT_DOWN;
				return true;
			}
		}
		break;

	case FRONT_DOWN:
		if (isSampleGood(sample, feedback)) {
			_currentSamples++;
			addObservation(sample);

			if (_currentSamples > _samplesToTake) {
				DIRECT_USER ("Place the back pointing down");
				_state = BACK_DOWN;
				return true;
			}
		}
		break;

	case BACK_DOWN:
		if (isSampleGood(sample, feedback)) {
			_currentSamples++;
			addObservation(sample);

			if (_currentSamples > _samplesToTake) {
				_state = SETUP;
				return false;
			}
		}
		break;
	} // switch

	return false;
} // TripleAxisCalibrator::pushSample

} // namespace mhvlib
