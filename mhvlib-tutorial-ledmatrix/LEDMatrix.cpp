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

/* Drive a 2x2 LED matrix connected to Arduino pins 8 & 9 (rows),
 * and 10 & 11 (columns)
 */

#define MHVLIB_NEED_PURE_VIRTUAL

// Bring in the interrupt library
#include <avr/interrupt.h>

// Bring in the IO library
#include <MHV_io.h>

// Bring in the timers we need for animation and PWM
#include <MHV_Timer8.h>

// Bring in the RTC
#include <MHV_RTC.h>

// Bring in the PWM Matrix core
#include <MHV_PWMMatrix.h>

// Bring in the gamma correction library
#include <MHV_GammaCorrect.h>

// Bring in the power management header
#include <avr/power.h>
#include <avr/sleep.h>

// A timer we will use to tick the RTC
MHV_Timer8 tickTimer(MHV_TIMER8_2);
MHV_TIMER_ASSIGN_1INTERRUPT(tickTimer, MHV_TIMER2_INTERRUPTS);

// A timer we will use to tick the LED Matrix
MHV_Timer8 ledMatrixTimer(MHV_TIMER8_0);
MHV_TIMER_ASSIGN_1INTERRUPT(tickTimer, MHV_TIMER0_INTERRUPTS);

#define ALARM_COUNT	4
// The RTC object we will use
MHV_RTC_CREATE(rtc, ALARM_COUNT);


/* 4 LED matrix
 * rows are on pins 8 & 9 and are active low
 * cols are on pins 10 & 11 and are active high
 */
#define LED_MATRIX_ROWS	2
#define LED_MATRIX_COLS	2

/* The driver that the PWM Matrix will use to manipulate the LEDs
 * These are implemented as a driver class to allow us to replace directly driven
 * LEDs with shift registers
 */
class LEDDriver : public MHV_PWMMatrixDriver {
public:
	void rowOn(uint16_t row);
	void rowOff(uint16_t row);
	void colOn(uint16_t col);
	void colOff(uint16_t col);
};

void LEDDriver::rowOn(uint16_t row) {
	switch (row) {
	case 0:
		mhv_pinOff(MHV_ARDUINO_PIN_8);
		break;
	case 1:
		mhv_pinOff(MHV_ARDUINO_PIN_9);
		break;
	}
}

void LEDDriver::rowOff(uint16_t row) {
	switch (row) {
	case 0:
		mhv_pinOn(MHV_ARDUINO_PIN_8);
		break;
	case 1:
		mhv_pinOn(MHV_ARDUINO_PIN_9);
		break;
	}
}

void LEDDriver::colOn(uint16_t col) {
	switch (col) {
	case 0:
		mhv_pinOn(MHV_ARDUINO_PIN_10);
		break;
	case 1:
		mhv_pinOn(MHV_ARDUINO_PIN_11);
		break;
	}
}

void LEDDriver::colOff(uint16_t col) {
	switch (col) {
	case 0:
		mhv_pinOff(MHV_ARDUINO_PIN_10);
		break;
	case 1:
		mhv_pinOff(MHV_ARDUINO_PIN_11);
		break;
	}
}

LEDDriver ledDriver;

MHV_PWMMATRIX_CREATE(ledMatrix, LED_MATRIX_ROWS, LED_MATRIX_COLS, 1, ledDriver);

/* Animation routine for the LED matrix
 * brings up each LED in turn, then takes then down in turn
 */
class Animation : public MHV_TimerListener {
private:
	uint8_t		_led;
#define FADERMAX 255
	uint8_t		_fader;
	bool		_direction;

public:
	Animation();
	void alarm();
};

Animation::Animation() :
	_led(0),
	_fader(0),
	_direction(true) {}

void Animation::alarm() {
	if (_direction) {
		_fader++;
	} else {
		_fader--;
	}

	switch (_led) {
	case 4:
		_led = 0;
		_direction = !_direction;
		if (_direction) {
			_fader = 0;
		} else {
			_fader = FADERMAX;
		}
// no break
	case 0:
		ledMatrix.setPixel(0, 0,
				mhv_precalculatedGammaCorrect(_fader));
		break;
	case 1:
		ledMatrix.setPixel(1, 0,
				mhv_precalculatedGammaCorrect(_fader));
		break;
	case 2:
		ledMatrix.setPixel(1, 1,
				mhv_precalculatedGammaCorrect(_fader));
		break;
	case 3:
		ledMatrix.setPixel(0, 1,
				mhv_precalculatedGammaCorrect(_fader));
		break;
	}

	if (_direction && FADERMAX == _fader) {
		_fader = 0;
		_led++;
	} else if (!_direction && 0 == _fader) {
		_fader = FADERMAX;
		_led++;
	}
}

class LEDMatrixTicker : public MHV_TimerListener {
public:
	void alarm();
};

bool tick = false;

// Notify the main loop that the PWM status of the LED matrix should be refreshed
void LEDMatrixTicker::alarm() {
	tick = true;
}

LEDMatrixTicker ledMatrixTicker;


MAIN {
	// Disable all peripherals and enable just what we need
	power_all_disable();
	power_timer2_enable();
	power_timer1_enable();
	set_sleep_mode(SLEEP_MODE_IDLE);

	sei();

	/* Set up LED matrix - 2 rows of 2 columns
	 * We want a framerate of 30fps, and the software will spend half its time on each column
	 * Each frame will therefore be 1/60 seconds
	 * There are 256 PWM periods, so each timer tick needs to be 1 / (60 * 256) seconds
	 * ~ 64 microseconds
	 */
	mhv_setOutput(MHV_ARDUINO_PIN_8);
	mhv_setOutput(MHV_ARDUINO_PIN_9);
	mhv_setOutput(MHV_ARDUINO_PIN_10);
	mhv_setOutput(MHV_ARDUINO_PIN_11);

	// Configure the tick timer to tick every 1 millisecond
	tickTimer.setPeriods(1000, 0);
	tickTimer.setListener1(rtc);
	tickTimer.enable();

	// Configure the tick timer to tick every 64 microseconds
	ledMatrixTimer.setPeriods(64, 0);
	ledMatrixTimer.setListener1(ledMatrixTicker);
	ledMatrixTimer.enable();

	for (;;) {
/* Timer tick for the LED matrix
 * Note that we call this within the main loop, to avoid delaying interrupts
 */
		if (tick) {
			ledMatrix.alarm();
			tick = false;
		}

		sleep_mode();
	}
}

