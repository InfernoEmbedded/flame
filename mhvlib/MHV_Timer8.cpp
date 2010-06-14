/*
 * Copyright (c) 2010, Make, Hack, Void Inc
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

/* Create a new timer
 * param: time the time in microseconds
 */
MHV_Timer8::MHV_Timer8(MHV_TIMER_TYPE type, volatile uint8_t *controlRegA, volatile uint8_t *controlRegB,
		volatile uint8_t *overflowReg1, volatile uint8_t *overflowReg2, volatile uint8_t *counter,
		volatile uint8_t *interrupt) {
	_controlRegA = controlRegA;
	_controlRegB = controlRegB;
	_outputCompare1 = overflowReg1;
	_outputCompare2 = overflowReg2;
	_counter = counter;
	_interrupt = interrupt;
	_mode = MHV_TIMER_REPETITIVE;
	_type = type;
	_counterSize = 8;
	_prescaler = MHV_TIMER_PRESCALER_DISABLED;
}

MHV_Timer8::MHV_Timer8() {};

uint8_t MHV_Timer8::current(void) {
	uint8_t ret;
	uint8_t reg = SREG;
	cli();
	ret = *_counter;
	SREG = reg;
	return ret;
}


/* Set the prescaler
 * param	time		the time in timer ticks
 * param	prescaler	the prescaler to set
 * param	factor		the prescale factor
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

/* Calculate the top register
 * param	time 		input: the time in timer ticks, output: the scaled timer ticks
 * param	factor		the prescaler factor
 */
void MHV_Timer8::calculateTop(uint32_t *time, uint16_t factor) {
	*time = *time / factor - 1;
	return;
}

/* Times are in microseconds
 */
void MHV_Timer8::setPeriods(uint32_t time1, uint32_t time2) {
	MHV_TIMER_PRESCALER prescaler;
	uint16_t factor;
	uint32_t maxTime;

	time1 *= F_CPU / (1000000 * 2); // time is now in timer ticks
	time2 *= F_CPU / (1000000 * 2); // time is now in timer ticks

	if (time1 > time2) {
		maxTime = time1;
	} else {
		maxTime = time2;
	}

	calculatePrescaler(maxTime, &prescaler, &factor);
	calculateTop(&time1, factor);
	calculateTop(&time2, factor);
	setPeriods(prescaler, time1, time2);
}

/* Set the prescaler (internal use only)
 * param	prescaler	the prescaler value (only the lowest 3 bits may be set)
 */
void MHV_Timer8::_setPrescaler(MHV_TIMER_PRESCALER prescaler) {
	*_controlRegB = (*_controlRegB & 0xf8) | prescaler;
}

/* Get the prescaler
 * return the prescaler value
 */
MHV_TIMER_PRESCALER MHV_Timer8::getPrescaler(void) {
	return (MHV_TIMER_PRESCALER)(*_controlRegB & 0x07);
}

/* set the prescaler
 * param 	prescaler	the prescaler value
 */
void MHV_Timer8::setPrescaler(MHV_TIMER_PRESCALER prescaler) {
	_prescaler = prescaler;
}

/* Set the generation mode
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

/* Set the overflow periods
 * param	prescaler	the prescaler to use
 * param	time1		the first time in prescaled timer ticks
 * param	time2		the second time in prescaled timer ticks
 */
void MHV_Timer8::setPeriods(MHV_TIMER_PRESCALER prescaler, uint8_t time1, uint8_t time2) {
	uint8_t reg = SREG;
	cli();
	_prescaler = prescaler;
	_setPrescaler(prescaler);

	*_counter = 0;
	*_outputCompare1 = time1;
	*_outputCompare2 = time2;

	SREG = reg;
}

/* Get the number of timer cycles available
 */
uint8_t MHV_Timer8::getTop(void) {
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

/* Set the number of timer cycles available
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

void MHV_Timer8::setOutput1(uint8_t value) {
	*_outputCompare1 = value;
}

void MHV_Timer8::setOutput2(uint8_t value) {
	*_outputCompare2 = value;
}

uint8_t MHV_Timer8::getOutput1(void) {
	return *_outputCompare1;
}

uint8_t MHV_Timer8::getOutput2(void) {
	return *_outputCompare2;
}

void MHV_Timer8::connectOutput1(MHV_TIMER_CONNECT_TYPE type) {
	*_controlRegA = (*_controlRegA & 0x3F) | (type << 6);
}

void MHV_Timer8::connectOutput2(MHV_TIMER_CONNECT_TYPE type) {
	*_controlRegA = (*_controlRegA & 0xCF) | (type << 4);
}


/* Enable the timer module
 */
void MHV_Timer8::enable(void) {
	uint8_t reg = SREG;
	cli();

	*_counter = 0;
	_setPrescaler(_prescaler);
	setGenerationMode();
	if (_triggerFunction1) {
		*_interrupt |= _BV(OCIE0A);
	}
	if (*_outputCompare2 && _triggerFunction2) {
		*_interrupt |= _BV(OCIE0B);
		_haveTime2 = true;
	} else {
		_haveTime2 = false;
	}

	SREG = reg;
}

void MHV_Timer8::disable(void) {
	uint8_t reg = SREG;
	cli();

	_setPrescaler(MHV_TIMER_PRESCALER_DISABLED);
	*_interrupt &= ~_BV(OCIE0A);
	if (_haveTime2) {
		*_interrupt &= ~_BV(OCIE0B);
	}

	SREG = reg;
}

bool MHV_Timer8::enabled(void) {
	return getPrescaler();
}

void MHV_Timer8::trigger1() {
	if (MHV_TIMER_ONE_SHOT == _mode) {
		disable();
	}
	_triggerFunction1(_triggerData1);
}

void MHV_Timer8::trigger2() {
	if (_triggerFunction2) {
		_triggerFunction2(_triggerData2);
	}
}


void MHV_Timer8::setTriggers(void (*triggerFunction1)(void *triggerData), void *triggerData1,
		void (*triggerFunction2)(void *triggerData), void *triggerData2) {
	_triggerFunction1 = triggerFunction1;
	_triggerData1 = triggerData1;

	_triggerFunction2 = triggerFunction2;
	_triggerData2 = triggerData2;
}

void MHV_Timer8::setMode(MHV_TIMER_MODE mode) {
	_mode = mode;
}
