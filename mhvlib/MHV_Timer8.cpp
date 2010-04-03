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
MHV_Timer8::MHV_Timer8(enum MHV_TIMER_TYPE type, volatile uint8_t *controlRegA, volatile uint8_t *controlRegB,
		volatile uint8_t *overflowReg1, volatile uint8_t *overflowReg2, volatile uint8_t *counter,
		volatile uint8_t *interrupt, uint8_t interruptEnable1, uint8_t interruptEnable2,
		uint8_t generationMode) {
	_controlRegA = controlRegA;
	_controlRegB = controlRegB;
	_overflowReg1 = overflowReg1;
	_overflowReg2 = overflowReg2;
	_counter = counter;
	_interrupt = interrupt;
	_interruptEnable1 = interruptEnable1;
	_interruptEnable2 = interruptEnable2;
	_generationMode = generationMode;
	_mode = MHV_TIMER_REPETITIVE;
	_type = type;
	_counterSize = 8;
}

MHV_Timer8::MHV_Timer8() {};

/* Set the prescaler
 * param	time		the time in timer ticks
 * param	prescaler	the prescaler to set
 * param	factor		the prescale factor
 * return 0 on success
 */
uint8_t MHV_Timer8::setPrescaler(uint32_t time, uint8_t *prescaler, uint16_t *factor) {
	uint32_t limit = 0;
	if (8 == _counterSize) {
		limit = 256L;
	} else if (16 == _counterSize) {
		limit = 65536L;
	}

	switch (_type) {
	case MHV_TIMER_TYPE_5_PRESCALERS:
		if (time <= limit) {
			*prescaler = 1;
			*factor = 1;
		} else if (time <= limit * 8) {
			*prescaler = 2;
			*factor = 8;
		} else if (time <= limit * 64) {
			*prescaler = 3;
			*factor = 64;
		} else if (time <= limit * 256) {
			*prescaler = 4;
			*factor = 256;
		} else if (time <= limit * 1024) {
			*prescaler = 5;
			*factor = 1024;
		} else {
			return 1;
		}
		break;

	case MHV_TIMER_TYPE_7_PRESCALERS:
		if (time <= limit) {
			*prescaler = 1;
			*factor = 1;
		} else if (time <= limit * 8) {
			*prescaler = 2;
			*factor = 8;
		} else if (time <= limit * 32) {
			*prescaler = 3;
			*factor = 32;
		} else if (time <= limit * 64) {
			*prescaler = 4;
			*factor = 64;
		} else if (time <= limit * 128) {
			*prescaler = 5;
			*factor = 128;
		} else if (time <= limit * 256) {
			*prescaler = 6;
			*factor = 256;
		} else if (time <= limit * 1024) {
			*prescaler = 7;
			*factor = 1024;
		} else {
			return 1;
		}
		break;
	}

	return 0;
}

/* Calculate the top register
 * param	time 		the time in timer ticks
 * param	factor		the prescaler factor
 * param	top			the top register to set
 */
void MHV_Timer8::setTop(uint32_t *time, uint16_t factor) {
	*time = *time / factor - 1;
	return;
}

/* Times are in microseconds
 */
void MHV_Timer8::setPeriods(uint32_t time1, uint32_t time2) {
	uint8_t prescaler;
	uint16_t factor;
	uint32_t maxTime;

	time1 *= F_CPU / (1000000 * 2); // time is now in timer ticks
	time2 *= F_CPU / (1000000 * 2); // time is now in timer ticks

	if (time1 > time2) {
		maxTime = time1;
	} else {
		maxTime = time2;
	}

	setPrescaler(maxTime, &prescaler, &factor);
	setTop(&time1, factor);
	setTop(&time2, factor);
	setPeriods(prescaler, time1, time2);
}

/* Set the overflow periods
 * param	prescaler	the prescaler to use
 * param	time1		the first time in prescaled timer ticks
 * param	time2		the second time in prescaled timer ticks
 */
void MHV_Timer8::setPeriods(uint8_t prescaler, uint8_t time1, uint8_t time2) {
	uint8_t reg = SREG;
	cli();

	if (_controlRegB) {
		*_controlRegA &= ~_generationMode;
		*_controlRegB = prescaler;
	} else {
		*_controlRegA = ~_generationMode & prescaler;
	}

	*_counter = 0;
	*_overflowReg1 = time1;
	*_overflowReg2 = time2;

	SREG = reg;
}


void MHV_Timer8::enable(void) {
	uint8_t reg = SREG;
	cli();

	*_counter = 0;
	*_controlRegA |= _generationMode;
	*_interrupt |= _interruptEnable1;
	if (*_overflowReg2) {
		*_interrupt |= _interruptEnable2;
		_haveTime2 = true;
	} else {
		_haveTime2 = false;
	}

	SREG = reg;
}

void MHV_Timer8::disable(void) {
	uint8_t reg = SREG;
	cli();

	*_controlRegA &= ~_generationMode;
	*_interrupt &= ~_interruptEnable1;
	if (_haveTime2) {
		*_interrupt &= ~_interruptEnable2;
	}

	SREG = reg;
}

bool MHV_Timer8::enabled(void) {
	return *_controlRegA & _generationMode;
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

void MHV_Timer8::setMode(enum MHV_TIMER_MODE mode) {
	_mode = mode;
}
