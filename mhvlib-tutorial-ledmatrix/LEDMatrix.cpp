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

// Bring in the interrupt library
#include <avr/interrupt.h>

// Bring in the IO library
#include <MHV_io.h>

// Bring in the timers we need for animation and PWM
#include <MHV_Timer8.h>
#include <MHV_Timer16.h>

// Bring in the PWM Matrix core
#include <MHV_PWMMatrix.h>

// Bring in the gamma correction library
#include <MHV_GammaCorrect.h>


/*
 * Required as the display classes have pure virtual methods
 * This will only get called if a pure virtual method is called in a constructor (never in MHVlib)
 */
extern "C" void __cxa_pure_virtual() {
	cli();
	for (;;);
}

// A timer we will use for animation
MHV_Timer8 animateTimer(MHV_TIMER8_2);
MHV_TIMER_ASSIGN_2INTERRUPTS(animateTimer, MHV_TIMER2_INTERRUPTS);

// A timer we will use for the PWM Matrix
MHV_Timer16 ledMatrixTimer(MHV_TIMER16_1);
MHV_TIMER_ASSIGN_1INTERRUPT(ledMatrixTimer, MHV_TIMER1_INTERRUPTS);

/* 4 LED matrix
 * rows are on pins 8 & 9 and are active low
 * cols are on pins 10 & 11 and are active high
 */
#define LED_MATRIX_ROWS	2
#define LED_MATRIX_COLS	2

/* Callbacks that the PWM Matrix will use to manipulate the LEDs
 * These are implemented as callbacks to allow us to replace directly driven
 * LEDs with shift registers
 */
void matrixRowOn(uint8_t row) {
	switch (row) {
	case 0:
		mhv_pinOff(MHV_ARDUINO_PIN_8);
		break;
	case 1:
		mhv_pinOff(MHV_ARDUINO_PIN_9);
		break;
	}
}

void matrixRowOff(uint8_t row) {
	switch (row) {
	case 0:
		mhv_pinOn(MHV_ARDUINO_PIN_8);
		break;
	case 1:
		mhv_pinOn(MHV_ARDUINO_PIN_9);
		break;
	}
}

void matrixColOn(uint8_t col) {
	switch (col) {
	case 0:
		mhv_pinOn(MHV_ARDUINO_PIN_10);
		break;
	case 1:
		mhv_pinOn(MHV_ARDUINO_PIN_11);
		break;
	}
}

void matrixColOff(uint8_t col) {
	switch (col) {
	case 0:
		mhv_pinOff(MHV_ARDUINO_PIN_10);
		break;
	case 1:
		mhv_pinOff(MHV_ARDUINO_PIN_11);
		break;
	}
}

// The framebuffer for the matrix
uint8_t ledMatrixFrameBuffer[LED_MATRIX_ROWS * LED_MATRIX_COLS];

// Create the matrix
MHV_PWMMatrix ledMatrix(LED_MATRIX_ROWS, LED_MATRIX_COLS, ledMatrixFrameBuffer,
		matrixRowOn, matrixRowOff, matrixColOn, matrixColOff);

/* Animation routine for the LED matrix
 * brings up each LED in turn, then takes then down in turn
 */
#define FADERMAX 255

uint8_t led = 0;
uint8_t fader = 0;
bool direction = 1;

void animateMatrix(void *data) {
	if (direction) {
		fader++;
	} else {
		fader--;
	}

	switch (led) {
	case 4:
		led = 0;
		direction = !direction;
		if (direction) {
			fader = 0;
		} else {
			fader = FADERMAX;
		}
// Drop through
	case 0:
		ledMatrix.setPixel(0, 0,
				mhv_precalculatedGammaCorrect(fader));
		break;
	case 1:
		ledMatrix.setPixel(1, 0,
				mhv_precalculatedGammaCorrect(fader));
		break;
	case 2:
		ledMatrix.setPixel(1, 1,
				mhv_precalculatedGammaCorrect(fader));
		break;
	case 3:
		ledMatrix.setPixel(0, 1,
				mhv_precalculatedGammaCorrect(fader));
		break;
	}

	if (direction && FADERMAX == fader) {
		fader = 0;
		led++;
	} else if (!direction && 0 == fader) {
		fader = FADERMAX;
		led++;
	}
}


volatile bool tick = false;

/* Timer tick for the LED matrix
 * Set a flag here to notify the main loop to go out and do the work
 * This keeps the matrix code (which is loopy) out of the interrupt
 * context
 */
void ledMatrixTick(void *data) {
	tick = true;
}

int main(void) {
	sei();

	animateTimer.setTriggers(&animateMatrix, 0, 0, 0);
	animateTimer.setPeriods(30000UL, 0UL); // 30ms - max an 8 bit timer can count is 32768us @ 16MHz

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

	ledMatrixTimer.setTriggers(ledMatrixTick, 0, 0, 0, 0, 0);
	ledMatrixTimer.setPeriods(64, 0, 0);

	ledMatrixTimer.enable();
	animateTimer.enable();

	for (;;) {
		/* Timer tick for the LED matrix
		 * Note that we call this within the main loop, to avoid delaying interrupts
		 */
		if (tick) {
			ledMatrix.tick();
			tick = false;
		}
	}

	return 0;
}

