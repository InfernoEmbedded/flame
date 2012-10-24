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

/**
 * Illustrates the use of a 20x4 LCD display with a 74HC164 shift register
 * on an AVR ATMEGA 328P.
 *
 *  The circuit (see docs/HD44780_Shift_Register.pdf for a schematic):
 *
 *--- Shift Register ---
 * SR Pin 1 (A) to AVR PD4 (Data)
 * SR Pin 2 (B) to AVR PD4 (Data)
 * SR Pin 3 (QA) to LCD pin 4 (RS)
 * SR Pin 7 (GND) to Ground
 * SR Pin 8 (CLK) to AVR PD2 (Clock)
 * SR Pin 9 (CLR) to +5v *
 * SR Pin 10 (QE) to LCD pin 11 (D4)
 * SR Pin 11 (QF) to LCD pin 12 (D5)
 * SR Pin 12 (QG) to LCD pin 13 (D6)
 * SR Pin 13 (QH) to LCD pin 14 (D7)
 * SR Pin 14 (Vcc) to +5v
 *--- LCD HD44780 --- *
 * LCD pin 1 (Vss) to Ground
 * LCD pin 2 (Vdd) to +5V
 * LCD pin 3 (Vo)  to 10k Wiper
 * LCD pin 5 (R/W) to Ground
 * LCD pin 6 (E) to AVR (PD7) (Enable)
 */



#include <avr/interrupt.h>
#include <util/delay.h>
#include <mhvlib/io.h>
#include <mhvlib/Display_HD44780_Shift_Register.h>
#include <mhvlib/Timer.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include <avr/sleep.h>

using namespace mhvlib;

// A timer we will use to tick the display
TimerImplementation<MHV_TIMER8_2, TIMER_MODE::REPETITIVE>tickTimer;
MHV_TIMER_ASSIGN_1INTERRUPT(tickTimer, MHV_TIMER2_INTERRUPTS);

#define COLUMNS		20
#define	ROWS		4
#define TX_COUNT	10
#define MULTILINE	true
#define	BIGFONT		false
#define CURSORON	false
#define CURSORBLINK	false
#define	LEFT2RIGHT	true
#define	SCROLL		false

Display_HD44780_Shift_Register<COLUMNS, ROWS, TX_COUNT, MHV_PIN_D4, MHV_PIN_D7, MHV_PIN_D2> display;

/**
 * +
 * Render text using the asynchronous buffers
 * @param	display	the display to draw on
 */
void textAnimation(Device_TX *display) {
	display->write("1. Here is a string of text");
	display->write_P(PSTR("2. Here is string of text in PROGMEM"));
	display->write("3. Here is a buffer containing some data//This will not show", 40);
	display->write_P(PSTR("4. Here is a buffer in PROGMEM containing some data//This will not show"), 51);
}

MAIN {
	// Disable all peripherals and enable just what we need
	power_all_disable();
	power_timer2_enable();
	// Specify what level sleep to perform
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	// Enable global interrupts
	sei();

	display.init(MULTILINE, BIGFONT, CURSORON, CURSORBLINK, LEFT2RIGHT, SCROLL);

	textAnimation(&display);

	while (display.txAnimation(ROWS - 1)) {
		for (uint8_t i = 0; i < 12; i++) {
			_delay_ms(100);
		}
	}

	int16_t offsetX = 0;
	display.writeString_P(&offsetX, ROWS - 1, PSTR("All done"));

	for (;;) {
		sleep_mode();
	};

	return 0;
}


