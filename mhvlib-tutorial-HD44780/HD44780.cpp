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

/* Demonstrate the HD44780 display on port D
 * Data port layout  Port=B, n=0
 * Bit	description
 * n	DB4
 * n+1	DB5
 * n+2	DB6
 * n+3	DB7
 *
 * Control port layout  Port=C, n=0
 * n	RS Register Select
 * n+1	R/W Read/Write
 * n+2	E Enable
 *
 * Visual port layout	Port=C, n=3
 * n	Contrast (V0)
 * n+1	LED Positive
 */

#include <avr/interrupt.h>
#include <util/delay.h>
#include <MHV_io.h>
#include <MHV_Display_HD44780_Direct_Connect.h>
#include <MHV_Timer8.h>

#include <avr/pgmspace.h>

/*
 * Required as the display classes have pure virtual methods
 * This will only get called if a pure virtual method is called in a constructor (never in MHVlib)
 */
extern "C" void __cxa_pure_virtual() {
	cli();
}

// The number of elements we want to be able to store to send asynchronously
#define TX_ELEMENTS_COUNT 10
#define TX_BUFFER_SIZE TX_ELEMENTS_COUNT * sizeof(MHV_TX_BUFFER) + 1
// A buffer for the display to send data, it only contains pointers
char txBuf[TX_BUFFER_SIZE];
MHV_RingBuffer txBuffer(txBuf, TX_BUFFER_SIZE);

// A timer we will use to tick the display
MHV_Timer8 tickTimer(MHV_TIMER8_0);
MHV_TIMER_ASSIGN_1INTERRUPT(tickTimer, MHV_TIMER0_INTERRUPTS);

#define COLUMNS		20
#define	ROWS		4
#define MULTILINE	true
#define	BIGFONT		false
#define CURSORON	false
#define CURSORBLINK	false
#define	LEFT2RIGHT	true
#define	SCROLL		false

MHV_Display_HD44780_Direct_Connect display(MHV_PIN_B0, MHV_PIN_C0, MHV_PIN_C3,
		COLUMNS, ROWS, &txBuffer);

// A timer trigger that will tick the RTC
void displayTrigger(void *data) {
	display.tick1ms();
}

/**
 * Render text using the asynchronous buffers
 * @param	display	the display to draw on
 */
void textAnimation(MHV_Display_Character *display) {
	display->write("1. Here is a string of text");
	display->write_P(PSTR("2. Here is string of text in PROGMEM"));
	display->write("3. Here is a buffer containing some data//This will not show", 40);
	display->write_P(PSTR("4. Here is a buffer in PROGMEM containing some data//This will not show"), 51);
}

int main(void) {
	// Enable global interrupts
	sei();

	// Turn power on
	mhv_setOutput(MHV_PIN_C5);
	mhv_pinOn(MHV_PIN_C5);

	// Enable board LED on
	mhv_setOutput(MHV_PIN_B5);

	display.init(MULTILINE, BIGFONT, CURSORON, CURSORBLINK, LEFT2RIGHT, SCROLL);

	// Configure the tick timer to tick every 0.5ms (at 20MHz)
	tickTimer.setPeriods(MHV_TIMER_PRESCALER_5_256, 36, 0);
	tickTimer.setTriggers(displayTrigger, 0, 0, 0);
	tickTimer.enable();

	textAnimation(&display);

	while (display.txAnimation(ROWS - 1)) {
		for (uint8_t i = 0; i < 12; i++) {
			_delay_ms(100);
		}
	}

	int16_t offsetX = 0;
	display.writeString_P(&offsetX, ROWS - 1, PSTR("All done"));

	for (;;) {};

// Main must return an int, even though we never get here
	return 0;
}


