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


#include "MHV_Timer16.h"
#include <avr/interrupt.h>

/* Create a new timer
 * param: time the time in microseconds
 */
MHV_Timer16::MHV_Timer16(volatile uint8_t *controlRegA, volatile uint8_t *controlRegB, volatile uint8_t *controlRegC,
		volatile uint16_t *outputCompare1, volatile uint16_t *outputCompare2, volatile uint16_t *outputCompare3,
		volatile uint16_t *counter,	volatile uint8_t *interrupt, volatile uint16_t *inputCapture1) {
	_controlRegA = controlRegA;
	_controlRegB = controlRegB;
	_controlRegC = controlRegC;
	_outputCompare1 = outputCompare1;
	_outputCompare2 = outputCompare2;
	_outputCompare3 = outputCompare3;
	_counter = counter;
	_interrupt = interrupt;
	_inputCapture1 = inputCapture1;
	_counterSize = 16;
	_mode = MHV_TIMER_REPETITIVE;
	_type = MHV_TIMER_TYPE_5_PRESCALERS;
}

uint16_t MHV_Timer16::current(void) {
	uint16_t ret;
	uint8_t reg = SREG;
	cli();
	ret = *_counter;
	SREG = reg;
	return ret;
}

/* Set the generation mode
 * param	mode	the mode to run the timer in
 */
void MHV_Timer16::setGenerationMode(void) {
	switch (_mode) {
	case MHV_TIMER_ONE_SHOT:
	case MHV_TIMER_REPETITIVE:
		*_controlRegA = (*_controlRegA & 0xfc);
		*_controlRegB = (*_controlRegB & 0xe7) | _BV(WGM12);
		break;
	case MHV_TIMER_16_PWM_PHASE_CORRECT:
		*_controlRegA = (*_controlRegA & 0xfc);
		*_controlRegB = (*_controlRegB & 0xe7) | _BV(WGM13);
		break;
	case MHV_TIMER_16_PWM_FAST:
		*_controlRegA = (*_controlRegA & 0xfc) | _BV(WGM11);
		*_controlRegB = (*_controlRegB & 0xe7) | _BV(WGM13) | _BV(WGM12);
		break;
	case MHV_TIMER_16_PWM_PHASE_FREQ_CORRECT: // Always use ICR for top
		*_controlRegA = (*_controlRegA & 0xfc);
		*_controlRegB = (*_controlRegB & 0xe7) | _BV(WGM13);
		break;
	default:
		break;
	}
}

/* Get the number of timer cycles available
 */
uint16_t MHV_Timer16::getTop(void) {
	switch (_mode) {
	case MHV_TIMER_ONE_SHOT:
	case MHV_TIMER_REPETITIVE:
		return *_outputCompare1;
	case MHV_TIMER_16_PWM_PHASE_CORRECT:
	case MHV_TIMER_16_PWM_FAST:
	case MHV_TIMER_16_PWM_PHASE_FREQ_CORRECT:
		return *_inputCapture1;
	default:
		return 0;
	}
}

/* Set the number of timer cycles available
*/
void MHV_Timer16::setTop(uint16_t value) {
	uint8_t reg;

	switch (_mode) {
	case MHV_TIMER_ONE_SHOT:
	case MHV_TIMER_REPETITIVE:
		reg = SREG;
		cli();

		*_outputCompare1 = value;

		SREG = reg;
		break;
	case MHV_TIMER_16_PWM_PHASE_CORRECT:
	case MHV_TIMER_16_PWM_FAST:
	case MHV_TIMER_16_PWM_PHASE_FREQ_CORRECT:
		reg = SREG;
		cli();

		*_inputCapture1 = value;

		SREG = reg;
	default:
		break;
	}
}

void MHV_Timer16::setOutput1(uint16_t value) {
	uint8_t reg = SREG;
	cli();

	*_outputCompare1 = value;

	SREG = reg;
}

void MHV_Timer16::setOutput2(uint16_t value) {
	uint8_t reg = SREG;
	cli();

	*_outputCompare2 = value;

	SREG = reg;
}

void MHV_Timer16::setOutput3(uint16_t value) {
	uint8_t reg = SREG;
	cli();

	*_outputCompare3 = value;

	SREG = reg;
}

uint16_t MHV_Timer16::getOutput1(void) {
	return *_outputCompare1;
}

uint16_t MHV_Timer16::getOutput2(void) {
	return *_outputCompare2;
}

uint16_t MHV_Timer16::getOutput3(void) {
	return *_outputCompare3;
}

void MHV_Timer16::connectOutput1(MHV_TIMER_CONNECT_TYPE type) {
	*_controlRegA = (*_controlRegA & 0x3F) | (type << 6);
}

void MHV_Timer16::connectOutput2(MHV_TIMER_CONNECT_TYPE type) {
	*_controlRegA = (*_controlRegA & 0xCF) | (type << 4);
}

void MHV_Timer16::connectOutput3(MHV_TIMER_CONNECT_TYPE type) {
	*_controlRegA = (*_controlRegA & 0xF3) | (type << 2);
}

void MHV_Timer16::disable(void) {
	uint8_t reg = SREG;
	cli();

	_setPrescaler(MHV_TIMER_PRESCALER_DISABLED);
	*_interrupt &= ~_BV(OCIE1A);

	if (_haveTime2) {
		*_interrupt &= ~_BV(OCIE1B);
	}

	if (_haveTime3) {
		*_interrupt &= ~_BV(OCIE1C);
	}

	SREG = reg;
}

void MHV_Timer16::enable(void) {
	uint8_t reg = SREG;
	cli();

	*_counter = 0;
	_setPrescaler(_prescaler);
	setGenerationMode();
	if (_triggerFunction1) {
		*_interrupt |= _BV(OCIE1A);
	}
	if (*_outputCompare2 && _triggerFunction2) {
		*_interrupt |= _BV(OCIE1B);
		_haveTime2 = true;
	} else {
		_haveTime2 = false;
	}

	if (*_outputCompare3 && _triggerFunction3) {
		*_interrupt |= _BV(OCIE1C);
		_haveTime3 = true;
	} else {
		_haveTime3 = false;
	}

	SREG = reg;
}

void MHV_Timer16::setPeriods(MHV_TIMER_PRESCALER prescaler, uint16_t time1, uint16_t time2, uint16_t time3) {
	uint8_t reg = SREG;
	cli();

	_prescaler = prescaler;
	_setPrescaler(prescaler);

	*_counter = 0;
	*_outputCompare1 = time1;
	*_outputCompare2 = time2;
	*_outputCompare3 = time3;

	SREG = reg;
}

#include <MHV_HardwareSerial.h>
#include <stdio.h>
extern MHV_HardwareSerial serial;


/* Times are in microseconds
 */
void MHV_Timer16::setPeriods(uint32_t time1, uint32_t time2, uint32_t time3) {
	MHV_TIMER_PRESCALER prescaler;
	uint16_t factor = 0;
	uint32_t maxTime;

	time1 *= F_CPU / (1000000 * 2); // time is now in clocks
	time2 *= F_CPU / (1000000 * 2); // time is now in clocks
	time3 *= F_CPU / (1000000 * 2); // time is now in clocks

	if (time1 > time2) {
		maxTime = time1;
	} else {
		maxTime = time2;
	}
	if (time3 > maxTime) {
		maxTime = time3;
	}

	calculatePrescaler(maxTime, &prescaler, &factor);
	if (time1) {
		calculateTop(&time1, factor);
	}
	if (time2) {
		calculateTop(&time2, factor);
	}
	if (time3) {
		calculateTop(&time3, factor);
	}
	setPeriods(prescaler, time1, time2, time3);
}

void MHV_Timer16::setTriggers(void (*triggerFunction1)(void *triggerData), void *triggerData1,
		void (*triggerFunction2)(void *triggerData), void *triggerData2,
		void (*triggerFunction3)(void *triggerData), void *triggerData3) {
	_triggerFunction1 = triggerFunction1;
	_triggerData1 = triggerData1;
	_triggerFunction2 = triggerFunction2;
	_triggerData2 = triggerData2;
	_triggerFunction3 = triggerFunction3;
	_triggerData3 = triggerData3;
}


void MHV_Timer16::trigger3() {
	if (_triggerFunction3) {
		_triggerFunction3(_triggerData3);
	}
}

