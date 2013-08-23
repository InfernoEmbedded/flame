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

#ifndef MHVTIMER_H_
#define MHVTIMER_H_

#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <mhvlib/io.h>

namespace mhvlib {

enum class TimerMode : uint8_t {
	ONE_SHOT,
	REPETITIVE,
	PWM_PHASE_CORRECT_VAR_FREQ_8,
	PWM_PHASE_CORRECT_2_OUTPUT_8,
	PWM_FAST_VAR_FREQ_8,
	PWM_FAST_2_OUTPUT_8,
	PWM_FAST_16,
	PWM_PHASE_CORRECT_16,
	PWM_PHASE_FREQ_CORRECT_16
};

enum class TimerType {
	HAS_5_PRESCALERS = 5,
	HAS_7_PRESCALERS = 7
};

enum class TimerPrescaler : uint8_t {
	PRESCALER_DISABLED = 0,
	PRESCALER_5_1 = 1,
	PRESCALER_5_8 = 2,
	PRESCALER_5_64 = 3,
	PRESCALER_5_256 = 4,
	PRESCALER_5_1024 = 5,
	PRESCALER_5_EXT_FALL = 6,
	PRESCALER_5_EXT_RISE = 7,
	PRESCALER_7_1 = 1,
	PRESCALER_7_8 = 2,
	PRESCALER_7_32 = 3,
	PRESCALER_7_64 = 4,
	PRESCALER_7_128 = 5,
	PRESCALER_7_256 = 6,
	PRESCALER_7_1024 = 7,
};
INLINE uint8_t operator| (uint8_t oredWith, TimerPrescaler prescaler) {
	const uint8_t myPrescaler = static_cast<uint8_t>(prescaler);
	return(myPrescaler | oredWith);
}

enum class TimerConnect : uint8_t {
	DISCONNECTED = 0,
	TOGGLE = 1,
	CLEAR = 2,
	SET = 3
};
INLINE uint8_t operator<< (TimerConnect type, uint8_t shift) {
	const uint8_t myType = static_cast<uint8_t>(type);
	return(myType << shift);
}

enum class AlarmSource : uint8_t {
	UNKNOWN,
	TIMER_OUTPUT_1,
	TIMER_OUTPUT_2,
	TIMER_OUTPUT_3,
	RTC
};

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

/**
 * A class which will be notified when an alarm goes off
 */
class TimerListener {
public:
	/**
	 * Called when an alarm goes off
	 */
	virtual void alarm(AlarmSource source) =0;
};

class Timer {
protected:
	TimerPrescaler _prescaler;
	TimerListener *_listener1;
	TimerListener *_listener2;
	TimerListener *_listener3;

	virtual uint8_t calculatePrescaler(uint32_t time, TimerPrescaler *prescaler, uint16_t *factor) =0;

	/**
	 * Calculate the top register
	 * @param	time 		input: the time in timer ticks, output: the scaled timer ticks
	 * @param	factor		the prescaler factor
	 */
	INLINE void calculateTop(uint32_t *time, uint16_t factor) {
		*time = (*time / factor) - 1;

		return;
	}

	/**
	 * Calculate the top register
	 * @param	time 		input: the time in timer ticks, output: the scaled timer ticks
	 * @param	factor		the prescaler factor
	 */
	INLINE void calculateTop(float *time, uint16_t factor) {
		*time = (*time / factor) - 1;

		return;
	}

	virtual void setGenerationMode() =0;
	virtual void _setPrescaler(TimerPrescaler prescaler) =0;

public:
	/**
	 * Set the periods for channels 1 and 2
	 * Times are in microseconds
	 * @param	usec1		the first time in microseconds
	 * @param	usec2		the second time in microseconds
	 * @return false on success
	 */
	INLINE bool setTimes(uint32_t usec1, uint32_t usec2) {
		TimerPrescaler prescaler;
		uint16_t factor;
		uint32_t maxTime;

		usec1 *= F_CPU / 1000000; // time is now in timer ticks
		usec2 *= F_CPU / 1000000; // time is now in timer ticks

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
	 * Set the periods for channels 1 and 2
	 * Times are in microseconds
	 * @param	usec1		the first time in microseconds
	 * @param	usec2		the second time in microseconds
	 * @return false on success
	 */
	INLINE bool setTimes(float usec1, float usec2) {
		TimerPrescaler prescaler;
		uint16_t factor;
		uint32_t maxTime;

		usec1 *= F_CPU / 1000000; // time is now in timer ticks
		usec2 *= F_CPU / 1000000; // time is now in timer ticks

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

		setPeriods(prescaler, (uint16_t)usec1, (uint16_t)usec2);

		return false;
	}

	/**
	 * Set the overflow periods for a 16 bit timer
	 * @param	usec1		the first time in microseconds
	 * @param	usec2		the second time in microseconds
	 * @param	usec3		the third time in microseconds
	 */
	INLINE bool setTimes(uint32_t usec1, uint32_t usec2, uint32_t usec3) {
		TimerPrescaler prescaler;
		uint16_t factor = 0;
		uint32_t maxTime;

		usec1 *= F_CPU / 1000000; // time is now in clocks
		usec2 *= F_CPU / 1000000; // time is now in clocks
		usec3 *= F_CPU / 1000000; // time is now in clocks

		if (usec1 > usec2) {
			maxTime = usec1;
		} else {
			maxTime = usec2;
		}
		if (usec3 > maxTime) {
			maxTime = usec3;
		}

		if (calculatePrescaler(maxTime, &prescaler, &factor)) {
			return true;
		}

		if (usec1) {
			calculateTop(&usec1, factor);
		}
		if (usec2) {
			calculateTop(&usec2, factor);
		}
		if (usec3) {
			calculateTop(&usec3, factor);
		}
		setPeriods(prescaler, usec1, usec2, usec3);

		return false;
	}

	/**
	 * Set the overflow periods for a 16 bit timer
	 * @param	usec1		the first time in microseconds
	 * @param	usec2		the second time in microseconds
	 * @param	usec3		the third time in microseconds
	 */
	INLINE bool setTimes(float usec1, float usec2, float usec3) {
		TimerPrescaler prescaler;
		uint16_t factor = 0;
		uint32_t maxTime;

		usec1 *= F_CPU / 1000000; // time is now in clocks
		usec2 *= F_CPU / 1000000; // time is now in clocks
		usec3 *= F_CPU / 1000000; // time is now in clocks

		if (usec1 > usec2) {
			maxTime = usec1;
		} else {
			maxTime = usec2;
		}
		if (usec3 > maxTime) {
			maxTime = usec3;
		}

		if (calculatePrescaler(maxTime, &prescaler, &factor)) {
			return true;
		}

		if (usec1) {
			calculateTop(&usec1, factor);
		}
		if (usec2) {
			calculateTop(&usec2, factor);
		}
		if (usec3) {
			calculateTop(&usec3, factor);
		}
		setPeriods(prescaler, (uint16_t)usec1, (uint16_t)usec2, (uint16_t)usec3);

		return false;
	}


	virtual uint16_t current() =0;
	virtual void setCurrent(uint16_t current) =0;
	virtual void setPeriods(TimerPrescaler prescaler, uint16_t time1, uint16_t time2) =0;
	virtual void setPeriods(TimerPrescaler prescaler, uint16_t time1, uint16_t time2, uint16_t time3) =0;
	virtual TimerPrescaler getPrescaler() =0;
	virtual uint16_t getPrescalerMultiplier() =0;
	void setPrescaler(TimerPrescaler prescaler);

	virtual uint16_t getTop() =0;
	virtual void setTop(uint16_t value) =0;
	virtual void setOutput(uint8_t channel, uint16_t value) =0;
	virtual void setOutput1(uint16_t value) =0;
	virtual void setOutput2(uint16_t value) =0;
	virtual void setOutput3(uint16_t value) =0;
	virtual uint16_t getOutput(uint8_t channel) =0;
	virtual uint16_t getOutput1() =0;
	virtual uint16_t getOutput2() =0;
	virtual uint16_t getOutput3() =0;
	virtual void connectOutput1(TimerConnect type) =0;
	virtual void connectOutput2(TimerConnect type) =0;
	virtual void connectOutput3(TimerConnect type) =0;
	virtual void enable() =0;
	virtual void disable() =0;
	bool enabled();
	virtual void trigger1() =0;
	void trigger2();
	void trigger3();
	void setListener1(TimerListener &listener);
	void setListener2(TimerListener &listener);
	void setListener3(TimerListener &listener);
	void setListener1(TimerListener *listener);
	void setListener2(TimerListener *listener);
	void setListener3(TimerListener *listener);
	void setListener(uint8_t channel, TimerListener &listener);
	void setListener(uint8_t channel, TimerListener *listener);
	virtual void waitForOverflow();
	virtual void clearOverflow();
};

/**
 * A hardware timer
 * @tparam	bits		the number of bits of the timer (8 or 16)
 * @tparam	type		the type of timer (number of prescalers)
 * @tparam	controlRegA	the address of the first control reg, in SFR_MEM8 format
 */
template<uint8_t bits, TimerType type,
		mhv_register controlRegA, mhv_register controlRegB, mhv_register controlRegC,
		mhv_register outputCompare1, mhv_register outputCompare2, mhv_register outputCompare3,
		mhv_register inputCapture1, mhv_register counter,
		mhv_register interruptFlag, mhv_register interruptMask, uint8_t interruptEnableA, TimerMode mode>
class TimerImplementation: public Timer {
protected:
	bool _haveTime2;
	bool _haveTime3;

	/**
	 * Set the prescaler (internal use only)
	 * @param	prescaler	the prescaler value (only the lowest 3 bits may be set)
	 */
	void _setPrescaler(TimerPrescaler prescaler) {
		_SFR_MEM8(controlRegB) = (_SFR_MEM8(controlRegB) & 0xf8) | prescaler;
	}

	/**
	 * Get the prescaler
	 * @return the prescaler value
	 */
	TimerPrescaler getPrescaler() {
		return (TimerPrescaler) (_SFR_MEM8(controlRegB) & 0x07);
	}

	/**
	 * Set the generation mode
	 */
	void setGenerationMode() {
		switch (bits) {
		case 8:
			switch (mode) {
			case TimerMode::ONE_SHOT:
				// no break
			case TimerMode::REPETITIVE:
				_SFR_MEM8(controlRegA) = (_SFR_MEM8(controlRegA) & 0xfc) | _BV(WGM01);
				_SFR_MEM8(controlRegB) = _SFR_MEM8(controlRegB) & 0xf7;
				break;
			case TimerMode::PWM_PHASE_CORRECT_VAR_FREQ_8:
				_SFR_MEM8(controlRegA) = (_SFR_MEM8(controlRegA) & 0xfc) | _BV(WGM00);
				_SFR_MEM8(controlRegB) = (_SFR_MEM8(controlRegB) & 0xf7) | _BV(WGM02);
				break;
			case TimerMode::PWM_PHASE_CORRECT_2_OUTPUT_8:
				_SFR_MEM8(controlRegA) = (_SFR_MEM8(controlRegA) & 0xfc) | _BV(WGM01);
				_SFR_MEM8(controlRegB) = (_SFR_MEM8(controlRegB) & 0xf7);
				break;
			case TimerMode::PWM_FAST_VAR_FREQ_8:
				_SFR_MEM8(controlRegA) = (_SFR_MEM8(controlRegA) & 0xfc) | _BV(WGM01) | _BV(WGM00);
				_SFR_MEM8(controlRegB) = (_SFR_MEM8(controlRegB) & 0xf7) | _BV(WGM02);
				break;
			case TimerMode::PWM_FAST_2_OUTPUT_8:
				_SFR_MEM8(controlRegA) = (_SFR_MEM8(controlRegA) & 0xfc) | _BV(WGM01) | _BV(WGM00);
				_SFR_MEM8(controlRegB) = (_SFR_MEM8(controlRegB) & 0xf7);
				break;
			default:
				break;
			}
			break;
#ifdef WGM12
		case 16:
			switch (mode) {
			case TimerMode::ONE_SHOT:
				// no break
			case TimerMode::REPETITIVE:
				_SFR_MEM8(controlRegA) = (_SFR_MEM8(controlRegA) & 0xfc);
				_SFR_MEM8(controlRegB) = (_SFR_MEM8(controlRegB) & 0xe7) | _BV(WGM12);
				break;
			case TimerMode::PWM_PHASE_CORRECT_16:
				_SFR_MEM8(controlRegA) = (_SFR_MEM8(controlRegA) & 0xfc);
				_SFR_MEM8(controlRegB) = (_SFR_MEM8(controlRegB) & 0xe7) | _BV(WGM13);
				break;
			case TimerMode::PWM_FAST_16:
				_SFR_MEM8(controlRegA) = (_SFR_MEM8(controlRegA) & 0xfc) | _BV(WGM11);
				_SFR_MEM8(controlRegB) = (_SFR_MEM8(controlRegB) & 0xe7) | _BV(WGM13) | _BV(WGM12);
				break;
			case TimerMode::PWM_PHASE_FREQ_CORRECT_16: // Always use ICR for top
				_SFR_MEM8(controlRegA) = (_SFR_MEM8(controlRegA) & 0xfc);
				_SFR_MEM8(controlRegB) = (_SFR_MEM8(controlRegB) & 0xe7) | _BV(WGM13);
				break;
			default:
				break;
			}
			break;
#endif
		}
	}

public:
	/**
	 * Get the current value of the timer
	 * @return the current value of the timer
	 */
	INLINE uint16_t current() {
		uint16_t ret;

		switch (bits) {
		case 8:
			return _SFR_MEM8(counter);
			break;

		case 16:
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				ret = _SFR_MEM16(counter);
			}
			break;
		}

		return ret;
	}

	/**
	 * Set the current value of the timer
	 * @param current	the new value
	 */
	INLINE void setCurrent(uint16_t current) {
		switch (bits) {
		case 8:
			_SFR_MEM8(counter) = (uint8_t)current;
			return;

		case 16:
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				_SFR_MEM16(counter) = current;
			}
			return;
		}
	}

	/**
	 * Set the prescaler
	 * @param	time		the time in timer ticks
	 * @param	prescaler	the prescaler to set
	 * @param	factor		the prescale factor
	 * return 0 on success
	 */
	uint8_t calculatePrescaler(uint32_t time, TimerPrescaler *prescaler, uint16_t *factor) {
		uint32_t limit = 0;
		if (8 == bits) {
			limit = 256L;
		} else if (16 == bits) {
			limit = 65536L;
		}

		switch (type) {
		case TimerType::HAS_5_PRESCALERS:
			if (time <= limit) {
				*prescaler = TimerPrescaler::PRESCALER_5_1;
				*factor = 1;
			} else if (time <= limit * 8) {
				*prescaler = TimerPrescaler::PRESCALER_5_8;
				*factor = 8;
			} else if (time <= limit * 64) {
				*prescaler = TimerPrescaler::PRESCALER_5_64;
				*factor = 64;
			} else if (time <= limit * 256) {
				*prescaler = TimerPrescaler::PRESCALER_5_256;
				*factor = 256;
			} else if (time <= limit * 1024) {
				*prescaler = TimerPrescaler::PRESCALER_5_1024;
				*factor = 1024;
			} else {
				return 1;
			}
			break;

		case TimerType::HAS_7_PRESCALERS:
			if (time <= limit) {
				*prescaler = TimerPrescaler::PRESCALER_7_1;
				*factor = 1;
			} else if (time <= limit * 8) {
				*prescaler = TimerPrescaler::PRESCALER_7_8;
				*factor = 8;
			} else if (time <= limit * 32) {
				*prescaler = TimerPrescaler::PRESCALER_7_32;
				*factor = 32;
			} else if (time <= limit * 64) {
				*prescaler = TimerPrescaler::PRESCALER_7_64;
				*factor = 64;
			} else if (time <= limit * 128) {
				*prescaler = TimerPrescaler::PRESCALER_7_128;
				*factor = 128;
			} else if (time <= limit * 256) {
				*prescaler = TimerPrescaler::PRESCALER_7_256;
				*factor = 256;
			} else if (time <= limit * 1024) {
				*prescaler = TimerPrescaler::PRESCALER_7_1024;
				*factor = 1024;
			} else {
				return 1;
			}
			break;
		}

		return 0;
	}

	/**
	 * Get the prescaler multiplier
	 */
	uint16_t getPrescalerMultiplier() {
		switch (type) {
		case TimerType::HAS_5_PRESCALERS:
			switch (getPrescaler()) {
			case TimerPrescaler::PRESCALER_5_1:
				return 1;
			case TimerPrescaler::PRESCALER_5_8:
				return 8;
			case TimerPrescaler::PRESCALER_5_64:
				return 64;
			case TimerPrescaler::PRESCALER_5_256:
				return 256;
			case TimerPrescaler::PRESCALER_5_1024:
				return 1024;
			default:
				break;
			}
			break;
		case TimerType::HAS_7_PRESCALERS:
			switch (getPrescaler()) {
			case TimerPrescaler::PRESCALER_7_1:
				return 1;
			case TimerPrescaler::PRESCALER_7_8:
				return 8;
			case TimerPrescaler::PRESCALER_7_32:
				return 32;
			case TimerPrescaler::PRESCALER_7_64:
				return 64;
			case TimerPrescaler::PRESCALER_7_128:
				return 128;
			case TimerPrescaler::PRESCALER_7_256:
				return 256;
			case TimerPrescaler::PRESCALER_7_1024:
				return 1024;
			default:
				break;
			}
			break;
		}

		return 0;
	}

	/**
	 * Set the overflow periods for an 8 bit timer
	 * @param	prescaler	the prescaler to use
	 * @param	time1		the first time in prescaled timer ticks
	 * @param	time2		the second time in prescaled timer ticks
	 */
	void setPeriods(TimerPrescaler prescaler, uint16_t time1, uint16_t time2) {
		_prescaler = prescaler;

		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			_setPrescaler(prescaler);

			switch (bits) {
			case 8:
				_SFR_MEM8(counter) = 0;
				_SFR_MEM8(outputCompare1) = time1;
				_SFR_MEM8(outputCompare2) = time2;
				break;
			case 16:
				_SFR_MEM16(counter) = 0;
				_SFR_MEM16(outputCompare1) = time1;
				_SFR_MEM16(outputCompare2) = time2;
				break;
			}
		}
	}

	/**
	 * Set the overflow periods for a 16 bit timer
	 * @param	prescaler	the prescaler to use
	 * @param	time1		the first time in prescaled timer ticks
	 * @param	time2		the second time in prescaled timer ticks
	 * @param	time3		the second time in prescaled timer ticks
	 */
	void setPeriods(TimerPrescaler prescaler, uint16_t time1, uint16_t time2, uint16_t time3) {
		_prescaler = prescaler;

		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			_setPrescaler(prescaler);

			switch (bits) {
			case 8:
				_SFR_MEM8(counter) = 0;
				_SFR_MEM8(outputCompare1) = time1;
				_SFR_MEM8(outputCompare2) = time2;
				if (0 != outputCompare3) {
					_SFR_MEM8(outputCompare3) = time3;
				}
				break;

			case 16:
				_SFR_MEM16(counter) = 0;
				_SFR_MEM16(outputCompare1) = time1;
				_SFR_MEM16(outputCompare2) = time2;
				if (0 != outputCompare3) {
					_SFR_MEM16(outputCompare3) = time3;
				}
				break;
			}
		}
	}

	/**
	 * Get the number of timer cycles available
	 * @return the number of cycles in an iteration of the timer
	 */
	uint16_t getTop() {
		switch (bits) {
		case 8:
			switch (mode) {
			case TimerMode::ONE_SHOT:
			case TimerMode::REPETITIVE:
			case TimerMode::PWM_PHASE_CORRECT_VAR_FREQ_8:
			case TimerMode::PWM_FAST_VAR_FREQ_8:
				return _SFR_MEM8(outputCompare1);
			case TimerMode::PWM_PHASE_CORRECT_2_OUTPUT_8:
			case TimerMode::PWM_FAST_2_OUTPUT_8:
				return 255;
			default:
				return 0;
			}
			break;
		case 16:
			switch (mode) {
			case TimerMode::ONE_SHOT:
			case TimerMode::REPETITIVE:
				return _SFR_MEM16(outputCompare1);
			case TimerMode::PWM_PHASE_CORRECT_16:
			case TimerMode::PWM_FAST_16:
			case TimerMode::PWM_PHASE_FREQ_CORRECT_16:
				return _SFR_MEM16(inputCapture1);
			default:
				return 0;
			}
			break;
		}
	}

	/**
	 * Set the number of timer cycles available
	 * @param	value	the number of cycles in an iteration of the timer
	 */
	void setTop(uint16_t value) {
		switch (bits) {
		case 8:
			switch (mode) {
			case TimerMode::ONE_SHOT:
			case TimerMode::REPETITIVE:
			case TimerMode::PWM_PHASE_CORRECT_VAR_FREQ_8:
			case TimerMode::PWM_FAST_VAR_FREQ_8:
				_SFR_MEM8(outputCompare1) = value;
				break;
			default:
				return;
			}
			break;
		case 16:
			switch (mode) {
			case TimerMode::ONE_SHOT:
			case TimerMode::REPETITIVE:
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
					_SFR_MEM16(outputCompare1) = value;
				}
				break;
			case TimerMode::PWM_PHASE_CORRECT_16:
			case TimerMode::PWM_FAST_16:
			case TimerMode::PWM_PHASE_FREQ_CORRECT_16:
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
					_SFR_MEM16(inputCapture1) = value;
				}
				break;
			default:
				break;
			}
			break;
		}
	}

	/**
	 * Set the current value of a channel
	 * @param channel	the channel to set
	 * @param value		the new value of the channel
	 */
	INLINE void setOutput(uint8_t channel, uint16_t value) {
		switch (bits) {
		case 8:
			switch (channel) {
			case 1:
				_SFR_MEM8(outputCompare1) = value;
				break;
			case 2:
				_SFR_MEM8(outputCompare2) = value;
				break;
			}
			break;
		case 16:
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				switch (channel) {
				case 1:
					_SFR_MEM16(outputCompare1) = value;
					break;
				case 2:
					_SFR_MEM16(outputCompare2) = value;
					break;
				case 3:
					_SFR_MEM16(outputCompare3) = value;
					break;
				}
			}
			break;
		}
	}

	/**
	 * Set the current value of channel 1
	 * @param value	the new value of channel 1
	 */
	INLINE void setOutput1(uint16_t value) {
		switch (bits) {
		case 8:
			_SFR_MEM8(outputCompare1) = value;
			break;
		case 16:
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				_SFR_MEM16(outputCompare1) = value;
			}
			break;
		}
	}

	/**
	 * Set the current value of channel 2
	 * @param value	the new value of channel 2
	 */
	INLINE void setOutput2(uint16_t value) {
		switch (bits) {
		case 8:
			_SFR_MEM8(outputCompare2) = value;
			break;
		case 16:
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				_SFR_MEM16(outputCompare2) = value;
			}
			break;
		}
	}

	/**
	 * Set the current value of channel 3
	 * @param value	the new value of channel 3
	 */
	INLINE void setOutput3(uint16_t value) {
		switch (bits) {
		case 8:
			_SFR_MEM8(outputCompare3) = value;
			break;
		case 16:
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				_SFR_MEM16(outputCompare3) = value;
			}
			break;
		}
	}


	/**
	 * Get the current value of a channel
	 * @param	channel	the channel to get
	 * @return the current value of the selected channel
	 */
	uint16_t getOutput(uint8_t channel) {
		switch (bits) {
		case 8:
		switch (channel) {
		case 1:
			return _SFR_MEM8(outputCompare1);
		case 2:
			return _SFR_MEM8(outputCompare2);
		default:
			return 0;
		}
		break;
		case 16:
			uint16_t ret = 0;

			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				switch (channel) {
				case 1:
					ret = _SFR_MEM16(outputCompare1);
					break;
				case 2:
					ret = _SFR_MEM16(outputCompare2);
					break;
				case 3:
					ret = _SFR_MEM16(outputCompare3);
					break;
				}
			}

			return ret;
			break;
		}

		return 0;
	}

	/**
	 * Return the current value of channel 1
	 * @return the current value of channel 1
	 */
	INLINE uint16_t getOutput1() {
		switch (bits) {
		case 8:
		return _SFR_MEM8(outputCompare1);
		case 16:
			uint16_t ret;
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				ret =  _SFR_MEM16(outputCompare1);
			}
			return ret;
		}
	}

	/**
	 * Return the current value of channel 2
	 * @return the current value of channel 2
	 */
	INLINE uint16_t getOutput2() {
		switch (bits) {
		case 8:
		return _SFR_MEM8(outputCompare2);
		case 16:
			uint16_t ret;
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				ret =  _SFR_MEM16(outputCompare2);
			}
			return ret;
		}
	}

	/**
	 * Return the current value of channel 3
	 * @return the current value of channel 3
	 */
	INLINE uint16_t getOutput3() {
		switch (bits) {
		case 16:
			uint16_t ret;
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				ret =  _SFR_MEM16(outputCompare3);
			}
			return ret;

		default:
			return 0;
		}
	}


	void connectOutput(uint8_t channel, TimerConnect connectType) {
		switch (channel) {
		case 1:
			_SFR_MEM8(controlRegA) = (_SFR_MEM8(controlRegA) & 0x3F) | (connectType << 6);
			break;
		case 2:
			_SFR_MEM8(controlRegA) = (_SFR_MEM8(controlRegA) & 0xCF) | (connectType << 4);
			break;
		case 3:
			if (16 == bits) {
				_SFR_MEM8(controlRegA) = (_SFR_MEM8(controlRegA) & 0xF3) | (connectType << 2);
			}
			break;
		}
	}


	/**
	 * Connect channel 1 to an output pin
	 * @param connectType	the method of connection
	 */
	INLINE void connectOutput1(TimerConnect connectType) {
		_SFR_MEM8(controlRegA) = (_SFR_MEM8(controlRegA) & 0x3F) | (connectType << 6);
	}

	/**
	 * Connect channel 2 to an output pin
	 * @param connectType	the method of connection
	 */
	INLINE void connectOutput2(TimerConnect connectType) {
		_SFR_MEM8(controlRegA) = (_SFR_MEM8(controlRegA) & 0xCF) | (connectType << 4);
	}

	/**
	 * Connect channel 3 to an output pin
	 * @param connectType	the method of connection
	 */
	INLINE void connectOutput3(TimerConnect connectType) {
		_SFR_MEM8(controlRegA) = (_SFR_MEM8(controlRegA) & 0xF3) | (connectType << 2);
	}

	/**
	 * Enable the timer module
	 */
	void enable() {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {

			switch (bits) {
			case 8:
				_SFR_MEM8(counter) = 0;
				break;
			case 16:
				_SFR_MEM16(counter) = 0;
				break;
			}
			_setPrescaler(_prescaler);
			setGenerationMode();
			if (_listener1) {
				_SFR_MEM8(interruptMask) |= _BV(interruptEnableA);
			}

			if (_SFR_MEM8(outputCompare2) && _listener2) {
				_SFR_MEM8(interruptMask) |= _BV(interruptEnableA + 1);
				_haveTime2 = true;
			} else {
				_haveTime2 = false;
			}

			if ((16 == bits) && _SFR_MEM8(outputCompare3) && _listener3) {
				_SFR_MEM8(interruptMask) |= _BV(interruptEnableA + 2);
				_haveTime3 = true;
			} else {
				_haveTime3 = false;
			}
		}
	}

	/**
	 * Disable the timer
	 */
	void disable() {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			_setPrescaler(TimerPrescaler::PRESCALER_DISABLED);
			_SFR_MEM8(interruptMask) &= ~_BV(interruptEnableA);
			if (_haveTime2) {
				_SFR_MEM8(interruptMask) &= ~_BV(interruptEnableA + 1);
			}
			if ((bits == 16) && _haveTime3) {
				_SFR_MEM8(interruptMask) &= ~_BV(interruptEnableA + 2);
			}
		}
	}

	/**
	 * Trigger the listener for channel 1
	 */
	void trigger1() {
		if (TimerMode::ONE_SHOT == mode) {
			disable();
		}
		_listener1->alarm(AlarmSource::TIMER_OUTPUT_1);
	}

	/**
	 * Wait for the timer to overflow
	 */
	INLINE void waitForOverflow() {
		while (!(_SFR_MEM8(interruptFlag) & 0x01)) {};
	}

	/**
	 * Clear a timer overflow
	 */
	INLINE void clearOverflow() {
		_SFR_MEM8(interruptFlag) &= 0x01;
	}

};

}
#endif /* MHVTIMER_H_ */
