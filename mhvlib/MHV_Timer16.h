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


#ifndef MHV_TIMER16_H_
#define MHV_TIMER16_H_

#include <MHV_Timer8.h>

#define MHV_TIMER_ASSIGN_3INTERRUPTS(mhvTimer, mhvTimerVectors) \
	_MHV_TIMER_ASSIGN_3INTERRUPTS(mhvTimer, mhvTimerVectors)
#define _MHV_TIMER_ASSIGN_3INTERRUPTS(mhvTimer, mhvTimerVect1, mhvTimerVect2, mhvTimerVect3) \
ISR(mhvTimerVect1) { \
	mhvTimer.trigger1(); \
} \
ISR(mhvTimerVect2) { \
	mhvTimer.trigger2(); \
} \
ISR(mhvTimerVect3) { \
	mhvTimer.trigger3(); \
}

class MHV_Timer16 : public MHV_Timer8 {
protected:
	volatile uint8_t	*_controlRegC;
	volatile uint16_t	*_outputCompare1;
	volatile uint16_t	*_outputCompare2;
	volatile uint16_t	*_outputCompare3;
	volatile uint16_t	*_counter;
	bool				_haveTime3;
	volatile uint16_t	*_inputCapture1;

	void (*_triggerFunction3)(void *data);
	void *_triggerData3;

	void setGenerationMode(void);

public:
	MHV_Timer16(volatile uint8_t *controlRegA, volatile uint8_t *controlRegB, volatile uint8_t *controlRegC,
			volatile uint16_t *outputCompare1, volatile uint16_t *outputCompare2, volatile uint16_t *outputCompare3,
			volatile uint16_t *counter,	volatile uint8_t *interrupt, volatile uint16_t *inputCapture1);
	void setPeriods(uint32_t time1, uint32_t time2, uint32_t time3);
	void setPeriods(MHV_TIMER_PRESCALER prescaler, uint16_t time1, uint16_t time2, uint16_t time3);
	uint16_t getTop(void);
	void setTop(uint16_t value);
	void setOutput1(uint16_t value);
	void setOutput2(uint16_t value);
	void setOutput3(uint16_t value);
	uint16_t getOutput1(void);
	uint16_t getOutput2(void);
	uint16_t getOutput3(void);
	void connectOutput1(MHV_TIMER_CONNECT_TYPE type);
	void connectOutput2(MHV_TIMER_CONNECT_TYPE type);
	void connectOutput3(MHV_TIMER_CONNECT_TYPE type);
	void enable(void);
	void disable(void);
	void trigger3(void);
	void setTriggers(void (*triggerFunction1)(void *triggerData), void *triggerData1,
			void (*triggerFunction2)(void *triggerData), void *triggerData2,
			void (*triggerFunction3)(void *triggerData), void *triggerData3);
	uint16_t current(void);
};

#endif /* MHV_TIMER16_H_ */
