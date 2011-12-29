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

#include "MHV_Timer8.h"
#include "MHV_io.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/**
 * Create a new timer
 * @param	timer	an MHV_TIMER_* macro providing the appropriate details of the timer hardware
 */
MHV_Timer8::MHV_Timer8(MHV_TIMER_TYPE type, volatile uint8_t *controlRegA, volatile uint8_t *controlRegB,
		volatile uint8_t *overflowReg1, volatile uint8_t *overflowReg2, volatile uint8_t *counter,
		volatile uint8_t *interrupt, uint8_t interruptEnableA) :
		_controlRegA(controlRegA),
	_controlRegB(controlRegB),
	_outputCompare1(overflowReg1),
	_outputCompare2(overflowReg2),
	_counter(counter),
	_interrupt(interrupt),
	_interruptEnableA(interruptEnableA),
	_prescaler(MHV_TIMER_PRESCALER_DISABLED),
	_mode(MHV_TIMER_REPETITIVE),
	_type(type),
	_counterSize(8),
	_haveTime2(false),
	_listener1(NULL),
	_listener2(NULL) {}

/**
 * Create a new timer (only for use by MHV_Timer16)
 */
MHV_Timer8::MHV_Timer8() {}

/**
 * Get the current value of the timer
 * @return the current value of the timer
 */
uint8_t MHV_Timer8::current() {
	uint8_t ret;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		ret = *_counter;
	}
	return ret;
}


/**
 * Set the prescaler
 * @param	time		the time in timer ticks
 * @param	prescaler	the prescaler to set
 * @param	factor		the prescale factor
 * return 0 on success
 */
uint8_t MHV_Timer8::calculatePrescaler(uint32_t time, MHV_TIMER_PRESCALER *prescaler, uint16_t *factor) {
	uint32_t limit = 0;
	if (8 == _counterSize) {
		limit = 256L;
	} else if (16 == _counterSize) {
		limit = 65536L;
	}

	switch (_type) {
	case MHV_TIMER_TYPE_5_PRESCALERS:
		if (time <= limit) {
			*prescaler = MHV_TIMER_PRESCALER_5_1;
			*factor = 1;
		} else if (time <= limit * 8) {
			*prescaler = MHV_TIMER_PRESCALER_5_8;
			*factor = 8;
		} else if (time <= limit * 64) {
			*prescaler = MHV_TIMER_PRESCALER_5_64;
			*factor = 64;
		} else if (time <= limit * 256) {
			*prescaler = MHV_TIMER_PRESCALER_5_256;
			*factor = 256;
		} else if (time <= limit * 1024) {
			*prescaler = MHV_TIMER_PRESCALER_5_1024;
			*factor = 1024;
		} else {
			return 1;
		}
		break;

	case MHV_TIMER_TYPE_7_PRESCALERS:
		if (time <= limit) {
			*prescaler = MHV_TIMER_PRESCALER_7_1;
			*factor = 1;
		} else if (time <= limit * 8) {
			*prescaler = MHV_TIMER_PRESCALER_7_8;
			*factor = 8;
		} else if (time <= limit * 32) {
			*prescaler = MHV_TIMER_PRESCALER_7_32;
			*factor = 32;
		} else if (time <= limit * 64) {
			*prescaler = MHV_TIMER_PRESCALER_7_64;
			*factor = 64;
		} else if (time <= limit * 128) {
			*prescaler = MHV_TIMER_PRESCALER_7_128;
			*factor = 128;
		} else if (time <= limit * 256) {
			*prescaler = MHV_TIMER_PRESCALER_7_256;
			*factor = 256;
		} else if (time <= limit * 1024) {
			*prescaler = MHV_TIMER_PRESCALER_7_1024;
			*factor = 1024;
		} else {
			return 1;
		}
		break;
	}

	return 0;
}

/**
 * Calculate the top register
 * @param	time 		input: the time in timer ticks, output: the scaled timer ticks
 * @param	factor		the prescaler factor
 */
void MHV_Timer8::calculateTop(uint32_t *time, uint16_t factor) {
	*time = *time / factor - 1;
	return;
}

/**
 * Set the periods for channels 1 and 2
 * Times are in microseconds
 * @param	usec1	the period for channel 1
 * @param	usec2	the period for channel 2
 * @return false on success
 */
bool MHV_Timer8::setPeriods(uint32_t usec1, uint32_t usec2) {
	MHV_TIMER_PRESCALER prescaler;
	uint16_t factor;
	uint32_t maxTime;

	usec1 *= F_CPU / (1000000 * 2); // time is now in timer ticks
	usec2 *= F_CPU / (1000000 * 2); // time is now in timer ticks

	if (usec1 > usec2) {
		maxTime = usec1;
	} else {
		maxTime = usec2;
	}

	if (calculatePrescaler(maxTime, &prescaler, &factor)) {
		return true;
	}
	calculateTop(&usec1, factor);
	calculateTop(&usec2, factor);

	setPeriods(prescaler, usec1, usec2);

	return false;
}




/**
 * Set the prescaler (internal use only)
 * @param	prescaler	the prescaler value (only the lowest 3 bits may be set)
 */
void MHV_Timer8::_setPrescaler(MHV_TIMER_PRESCALER prescaler) {
	*_controlRegB = (*_controlRegB & 0xf8) | prescaler;
}

/**
 * Get the prescaler
 * @return the prescaler value
 */
MHV_TIMER_PRESCALER MHV_Timer8::getPrescaler() {
	return (MHV_TIMER_PRESCALER)(*_controlRegB & 0x07);
}

/**
 * Get the prescaler multiplier
 */
uint16_t MHV_Timer8::getPrescalerMultiplier() {
	switch (_type) {
	case MHV_TIMER_TYPE_5_PRESCALERS:
		switch (getPrescaler()) {
		case MHV_TIMER_PRESCALER_5_1:
			return 1;
		case MHV_TIMER_PRESCALER_5_8:
			return 8;
		case MHV_TIMER_PRESCALER_5_64:
			return 64;
		case MHV_TIMER_PRESCALER_5_256:
			return 256;
		case MHV_TIMER_PRESCALER_5_1024:
			return 1024;
		default:
			break;
		}
		break;
	case MHV_TIMER_TYPE_7_PRESCALERS:
		switch (getPrescaler()) {
		case MHV_TIMER_PRESCALER_7_1:
			return 1;
		case MHV_TIMER_PRESCALER_7_8:
			return 8;
		case MHV_TIMER_PRESCALER_7_32:
			return 32;
		case MHV_TIMER_PRESCALER_7_64:
			return 64;
		case MHV_TIMER_PRESCALER_7_128:
			return 128;
		case MHV_TIMER_PRESCALER_7_256:
			return 256;
		case MHV_TIMER_PRESCALER_7_1024:
			return 1024;
		default:
			break;
		}
		break;
	}

	return 0;
}

/**
 * set the prescaler
 * @param 	prescaler	the prescaler value
 */
void MHV_Timer8::setPrescaler(MHV_TIMER_PRESCALER prescaler) {
	_prescaler = prescaler;
}

/**
 * Set the generation mode
 */
void MHV_Timer8::setGenerationMode() {
	switch (_mode) {
	case MHV_TIMER_ONE_SHOT:
	case MHV_TIMER_REPETITIVE:
		*_controlRegA = (*_controlRegA & 0xfc) | _BV(WGM01);
		*_controlRegB = (*_controlRegB & 0xf7);
		break;
	case MHV_TIMER_8_PWM_PHASE_CORRECT_VAR_FREQ:
		*_controlRegA = (*_controlRegA & 0xfc) | _BV(WGM00);
		*_controlRegB = (*_controlRegB & 0xf7) | _BV(WGM02);
		break;
	case MHV_TIMER_8_PWM_PHASE_CORRECT_2_OUTPUT:
		*_controlRegA = (*_controlRegA & 0xfc) | _BV(WGM01) | _BV(WGM00);
		*_controlRegB = (*_controlRegB & 0xf7);
		break;
	case MHV_TIMER_8_PWM_FAST_VAR_FREQ:
		*_controlRegA = (*_controlRegA & 0xfc) | _BV(WGM01) | _BV(WGM00);
		*_controlRegB = (*_controlRegB & 0xf7) | _BV(WGM02);
		break;
	case MHV_TIMER_8_PWM_FAST_2_OUTPUT:
		*_controlRegA = (*_controlRegA & 0xfc) | _BV(WGM01) | _BV(WGM00);
		*_controlRegB = (*_controlRegB & 0xf7);
		break;
	default:
		break;
	}
}

/**
 * Set the overflow periods
 * @param	prescaler	the prescaler to use
 * @param	time1		the first time in prescaled timer ticks
 * @param	time2		the second time in prescaled timer ticks
 */
void MHV_Timer8::setPeriods(MHV_TIMER_PRESCALER prescaler, uint8_t time1, uint8_t time2) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		_prescaler = prescaler;
		_setPrescaler(prescaler);

		*_counter = 0;
		*_outputCompare1 = time1;
		*_outputCompare2 = time2;
	}
}

/**
 * Get the number of timer cycles available
 * @return the number of cycles in an iteration of the timer
 */
uint8_t MHV_Timer8::getTop() {
	switch (_mode) {
	case MHV_TIMER_ONE_SHOT:
	case MHV_TIMER_REPETITIVE:
	case MHV_TIMER_8_PWM_PHASE_CORRECT_VAR_FREQ:
	case MHV_TIMER_8_PWM_FAST_VAR_FREQ:
		return *_outputCompare1;
	case MHV_TIMER_8_PWM_PHASE_CORRECT_2_OUTPUT:
	case MHV_TIMER_8_PWM_FAST_2_OUTPUT:
		return 255;
	default:
		return 0;
	}
}

/**
 * Set the number of timer cycles available
 * @param	value	the number of cycles in an iteration of the timer
*/
void MHV_Timer8::setTop(uint8_t value) {
	switch (_mode) {
	case MHV_TIMER_ONE_SHOT:
	case MHV_TIMER_REPETITIVE:
	case MHV_TIMER_8_PWM_PHASE_CORRECT_VAR_FREQ:
	case MHV_TIMER_8_PWM_FAST_VAR_FREQ:
		*_outputCompare1 = value;
		break;
	default:
		return;
	}
}

/**
 * Set the current value of a channel
 * @param channel	the channel to set
 * @param value		the new value of the channel
 */
void MHV_Timer8::setOutput(uint8_t channel, uint8_t value) {
	switch (channel) {
	case 1:
		*_outputCompare1 = value;
		break;
	case 2:
		*_outputCompare2 = value;
		break;
	}
}

/**
 * Set the current value of channel 1
 * @param value	the new value of channel 1
 */
void MHV_Timer8::setOutput1(uint8_t value) {
	*_outputCompare1 = value;
}

/**
 * Set the current value of channel 2
 * @param value	the new value of channel 2
 */
void MHV_Timer8::setOutput2(uint8_t value) {
	*_outputCompare2 = value;
}

/**
 * Get the current value of a channel
 * @param	channel	the channel to get
 * @return the current value of the selected channel
 */
uint8_t MHV_Timer8::getOutput(uint8_t channel) {
	switch (channel) {
	case 1:
		return *_outputCompare1;
		break;
	case 2:
		return *_outputCompare2;
		break;
	default:
		return 0;
	}
}

/**
 * Return the current value of channel 1
 * @return the current value of channel 1
 */
uint8_t MHV_Timer8::getOutput1() {
	return *_outputCompare1;
}

/**
 * Return the current value of channel 2
 * @return the current value of channel 2
 */
uint8_t MHV_Timer8::getOutput2() {
	return *_outputCompare2;
}

/**
 * Connect channel 1 to an output pin
 * @param type	the method of connection
 */
void MHV_Timer8::connectOutput1(MHV_TIMER_CONNECT_TYPE type) {
	*_controlRegA = (*_controlRegA & 0x3F) | (type << 6);
}

/**
 * Connect channel 2 to an output pin
 * @param type	the method of connection
 */
void MHV_Timer8::connectOutput2(MHV_TIMER_CONNECT_TYPE type) {
	*_controlRegA = (*_controlRegA & 0xCF) | (type << 4);
}


/**
 * Enable the timer module
 */
void MHV_Timer8::enable() {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {

		*_counter = 0;
		_setPrescaler(_prescaler);
		setGenerationMode();
		if (_listener1) {
			*_interrupt |= _BV(_interruptEnableA);
		}
		if (*_outputCompare2 && _listener2) {
			*_interrupt |= _BV(_interruptEnableA + 1);
			_haveTime2 = true;
		} else {
			_haveTime2 = false;
		}
	}
}

/**
 * Disable the timer
 */
void MHV_Timer8::disable() {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		_setPrescaler(MHV_TIMER_PRESCALER_DISABLED);
		*_interrupt &= ~_BV(_interruptEnableA);
		if (_haveTime2) {
			*_interrupt &= ~_BV(_interruptEnableA + 1);
		}
	}
}

/**
 * Check if the timer is enabled
 * @return	true if the timer is enabled
 */
bool MHV_Timer8::enabled() {
	return getPrescaler();
}

/**
 * Trigger the listener for channel 1
 */
void MHV_Timer8::trigger1() {
	if (MHV_TIMER_ONE_SHOT == _mode) {
		disable();
	}
	_listener1->alarm();
}

/**
 * Trigger the listener for channel 2
 */
void MHV_Timer8::trigger2() {
	if (_listener2) {
		_listener2->alarm();
	}
}

/**
 * Attach a listener to channel 1
 * @param listener	the listener to attach
 */
void MHV_Timer8::setListener1(MHV_TimerListener &listener) {
	_listener1 = &listener;
}

/**
 * Attach a listener to channel 2
 * @param listener	the listener to attach
 */
void MHV_Timer8::setListener2(MHV_TimerListener &listener) {
	_listener2 = &listener;
}

/**
 * Attach a listener to channel 1
 * @param listener	the listener to attach
 */
void MHV_Timer8::setListener1(MHV_TimerListener *listener) {
	_listener1 = listener;
}

/**
 * Attach a listener to channel 2
 * @param listener	the listener to attach
 */
void MHV_Timer8::setListener2(MHV_TimerListener *listener) {
	_listener2 = listener;
}


/**
 * Configure the mode of the timer module
 * @param mode	the mode the timer module should operate in
 */
void MHV_Timer8::setMode(MHV_TIMER_MODE mode) {
	_mode = mode;
}
