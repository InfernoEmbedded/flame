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


#ifndef MHV_TIMER8_H_
#define MHV_TIMER8_H_

#include <inttypes.h>

enum MHV_TIMER_MODE {
	MHV_TIMER_ONE_SHOT,
	MHV_TIMER_REPETITIVE
};

enum MHV_TIMER_TYPE {
	MHV_TIMER_TYPE_5_PRESCALERS,
	MHV_TIMER_TYPE_7_PRESCALERS
};

#define MHV_TIMER_ASSIGN_1INTERRUPT(mhvTimer, mhvTimerVectors) \
	_MHV_TIMER_ASSIGN_1INTERRUPT(mhvTimer, mhvTimerVectors)
#define _MHV_TIMER_ASSIGN_1INTERRUPT(mhvTimer, mhvTimerVect1, mhvTimerVect2) \
ISR(mhvTimerVect1) { \
	mhvTimer.trigger1(); \
}

#define MHV_TIMER_ASSIGN_2INTERRUPTS(mhvTimer, mhvTimerVectors) \
	_MHV_TIMER_ASSIGN_2INTERRUPTS(mhvTimer, mhvTimerVectors)
#define _MHV_TIMER_ASSIGN_2INTERRUPTS(mhvTimer, mhvTimerVect1, mhvTimerVect2) \
ISR(mhvTimerVect1) { \
	mhvTimer.trigger1(); \
} \
ISR(mhvTimerVect2) { \
	mhvTimer.trigger2(); \
}

class MHV_Timer8 {
protected:
	volatile uint8_t	*_controlRegA;
	volatile uint8_t	*_controlRegB;
	volatile uint8_t	*_overflowReg1;
	volatile uint8_t	*_overflowReg2;
	volatile uint8_t	*_counter;
	volatile uint8_t	*_interrupt;
	uint8_t				_interruptEnable1;
	uint8_t				_interruptEnable2;
	uint8_t 			_generationMode;
	enum MHV_TIMER_MODE	_mode;
	enum MHV_TIMER_TYPE	_type;
	uint8_t				_counterSize;
	bool				_haveTime2;
	void (*_triggerFunction1)(void *data);
	void *_triggerData1;
	void (*_triggerFunction2)(void *data);
	void *_triggerData2;

	uint8_t setPrescaler(uint32_t time, uint8_t *prescaler, uint16_t *factor);
	void setTop(uint32_t *time, uint16_t factor);
	MHV_Timer8(void);

public:
	MHV_Timer8(enum MHV_TIMER_TYPE type, volatile uint8_t *controlRegA, volatile uint8_t *controlRegB,
			volatile uint8_t *overflowReg1, volatile uint8_t *overflowReg2, volatile uint8_t *counter,
			volatile uint8_t *interrupt, uint8_t interruptEnable1, uint8_t interruptEnable2,
			uint8_t generationMode);
	void setPeriods(uint32_t time1, uint32_t time2);
	void setPeriods(uint8_t prescaler, uint8_t time1, uint8_t time2);
	void enable(void);
	void disable(void);
	bool enabled(void);
	void trigger1(void);
	void trigger2(void);
	void setTriggers(void (*triggerFunction1)(void *triggerData), void *triggerData1,
			void (*triggerFunction2)(void *triggerData), void *triggerData2);
	void setMode(enum MHV_TIMER_MODE mode);
};

#endif /* MHV_TIMER8_H_ */
