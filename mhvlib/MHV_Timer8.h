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
#include <avr/io.h>

enum mhv_timer_mode {
	MHV_TIMER_ONE_SHOT,
	MHV_TIMER_REPETITIVE,
	MHV_TIMER_8_PWM_PHASE_CORRECT_VAR_FREQ,
	MHV_TIMER_8_PWM_PHASE_CORRECT_2_OUTPUT,
	MHV_TIMER_8_PWM_FAST_VAR_FREQ,
	MHV_TIMER_8_PWM_FAST_2_OUTPUT,
	MHV_TIMER_16_PWM_FAST,
	MHV_TIMER_16_PWM_PHASE_CORRECT,
	MHV_TIMER_16_PWM_PHASE_FREQ_CORRECT
};
typedef enum mhv_timer_mode MHV_TIMER_MODE;

enum mhv_timer_type {
	MHV_TIMER_TYPE_5_PRESCALERS,
	MHV_TIMER_TYPE_7_PRESCALERS
};
typedef enum mhv_timer_type MHV_TIMER_TYPE;

enum mhv_timer_prescaler {
	MHV_TIMER_PRESCALER_DISABLED   = 0,
	MHV_TIMER_PRESCALER_5_1        = 1,
	MHV_TIMER_PRESCALER_5_8        = 2,
	MHV_TIMER_PRESCALER_5_64       = 3,
	MHV_TIMER_PRESCALER_5_256      = 4,
	MHV_TIMER_PRESCALER_5_1024     = 5,
	MHV_TIMER_PRESCALER_5_EXT_RISE = 6,
	MHV_TIMER_PRESCALER_5_EXT_FALL = 7,
	MHV_TIMER_PRESCALER_7_1        = 1,
	MHV_TIMER_PRESCALER_7_8        = 2,
	MHV_TIMER_PRESCALER_7_32       = 3,
	MHV_TIMER_PRESCALER_7_64       = 4,
	MHV_TIMER_PRESCALER_7_128      = 5,
	MHV_TIMER_PRESCALER_7_256      = 6,
	MHV_TIMER_PRESCALER_7_1024     = 7,
};
typedef enum mhv_timer_prescaler MHV_TIMER_PRESCALER;

enum mhv_timer_connect_type {
	MHV_TIMER_CONNECT_DISCONNECTED = 0,
	MHV_TIMER_CONNECT_TOGGLE = 1,
	MHV_TIMER_CONNECT_CLEAR = 2,
	MHV_TIMER_CONNECT_SET = 3
};
typedef enum mhv_timer_connect_type MHV_TIMER_CONNECT_TYPE;

#define MHV_TIMER_ASSIGN_1INTERRUPT(mhvTimer, mhvTimerVectors) \
	_MHV_TIMER_ASSIGN_1INTERRUPT(mhvTimer, mhvTimerVectors)
#define _MHV_TIMER_ASSIGN_1INTERRUPT(mhvTimer, mhvTimerVect1, mhvTimerVect2, mhvTimerVect3) \
ISR(mhvTimerVect1) { \
	mhvTimer.trigger1(); \
}

#define MHV_TIMER_ASSIGN_2INTERRUPTS(mhvTimer, mhvTimerVectors) \
	_MHV_TIMER_ASSIGN_2INTERRUPTS(mhvTimer, mhvTimerVectors)
#define _MHV_TIMER_ASSIGN_2INTERRUPTS(mhvTimer, mhvTimerVect1, mhvTimerVect2, mhvTImerVect3) \
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
	volatile uint8_t	*_outputCompare1;
	volatile uint8_t	*_outputCompare2;
	volatile uint8_t	*_counter;
	volatile uint8_t	*_interrupt;
	MHV_TIMER_PRESCALER	_prescaler;
	MHV_TIMER_MODE		_mode;
	MHV_TIMER_TYPE		_type;
	uint8_t				_counterSize;
	bool				_haveTime2;
	void (*_triggerFunction1)(void *data);
	void *_triggerData1;
	void (*_triggerFunction2)(void *data);
	void *_triggerData2;

	uint8_t calculatePrescaler(uint32_t time, MHV_TIMER_PRESCALER *prescaler, uint16_t *factor);
	void calculateTop(uint32_t *time, uint16_t factor);
	void setGenerationMode(void);
	MHV_Timer8(void);
	void _setPrescaler(MHV_TIMER_PRESCALER prescaler);

public:
	MHV_Timer8(MHV_TIMER_TYPE type, volatile uint8_t *controlRegA, volatile uint8_t *controlRegB,
			volatile uint8_t *overflowReg1, volatile uint8_t *overflowReg2, volatile uint8_t *counter,
			volatile uint8_t *interrupt);
	void setPeriods(uint32_t usec1, uint32_t usec2);
	uint8_t current(void);
	void setPeriods(MHV_TIMER_PRESCALER prescaler, uint8_t time1, uint8_t time2);
	MHV_TIMER_PRESCALER getPrescaler(void);
	uint16_t getPrescalerMultiplier(void);
	void setPrescaler(MHV_TIMER_PRESCALER prescaler);

	uint8_t getTop(void);
	void setTop(uint8_t value);
	void setOutput(uint8_t channel, uint8_t value);
	void setOutput1(uint8_t value);
	void setOutput2(uint8_t value);
	uint8_t getOutput(uint8_t channel);
	uint8_t getOutput1(void);
	uint8_t getOutput2(void);
	void connectOutput1(MHV_TIMER_CONNECT_TYPE type);
	void connectOutput2(MHV_TIMER_CONNECT_TYPE type);
	void enable(void);
	void disable(void);
	bool enabled(void);
	void trigger1(void);
	void trigger2(void);
	void setTriggers(void (*triggerFunction1)(void *triggerData), void *triggerData1,
			void (*triggerFunction2)(void *triggerData), void *triggerData2);
	void setMode(MHV_TIMER_MODE mode);
};

#endif /* MHV_TIMER8_H_ */
