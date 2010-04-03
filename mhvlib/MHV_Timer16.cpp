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
		volatile uint16_t *overflowReg1, volatile uint16_t *overflowReg2, volatile uint16_t *overflowReg3,
		volatile uint16_t *counter,	volatile uint8_t *interrupt,
		uint8_t interruptEnable1, uint8_t interruptEnable2, uint8_t interruptEnable3,
		uint8_t generationMode) {
	_controlRegA = controlRegA;
	_controlRegB = controlRegB;
	_controlRegC = controlRegC;
	_overflowReg1 = overflowReg1;
	_overflowReg2 = overflowReg2;
	_overflowReg2 = overflowReg3;
	_counter = counter;
	_interrupt = interrupt;
	_interruptEnable1 = interruptEnable1;
	_interruptEnable2 = interruptEnable2;
	_interruptEnable3 = interruptEnable3;
	_generationMode = generationMode;
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

void MHV_Timer16::disable(void) {
	*_controlRegB &= ~_generationMode;
	*_interrupt &= ~_interruptEnable1;
	if (_haveTime2) {
		*_interrupt &= ~_interruptEnable2;
	}
	if (_haveTime3) {
		*_interrupt &= ~_interruptEnable3;
	}
}

void MHV_Timer16::enable(void) {
	uint8_t reg = SREG;
	cli();
	*_counter = 0;
	SREG = reg;
	*_controlRegB |= _generationMode;
	*_interrupt = _interruptEnable1;

	if (_haveTime2) {
		*_interrupt |= _interruptEnable2;
	}
	if (_haveTime3) {
		*_interrupt |= _interruptEnable3;
	}
}

void MHV_Timer16::setPeriods(uint8_t prescaler, uint16_t time1, uint16_t time2, uint16_t time3) {
	uint8_t reg = SREG;
	cli();
	*_controlRegA = 0;
	*_controlRegB = prescaler | _generationMode;

	*_counter = 0;
	*_overflowReg1 = time1;
	if (time2) {
		*_overflowReg2 = time2;
		_haveTime2 = true;
	} else {
		_haveTime2 = false;
	}
	if (time3) {
		*_overflowReg3 = time3;
		_haveTime3 = true;
	} else {
		_haveTime3 = false;
	}
	SREG = reg;
}

#include <MHV_HardwareSerial.h>
#include <stdio.h>
extern MHV_HardwareSerial serial;


/* Times are in microseconds
 */
void MHV_Timer16::setPeriods(uint32_t time1, uint32_t time2, uint32_t time3) {
	uint8_t prescaler;
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

	setPrescaler(maxTime, &prescaler, &factor);
	if (time1) {
		setTop(&time1, factor);
	}
	if (time2) {
		setTop(&time2, factor);
	}
	if (time3) {
		setTop(&time3, factor);
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

