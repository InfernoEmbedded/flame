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

/* A blinking LED on Arduino pin 13 - uses a timer to toggle the LED
 */

// Bring in *int*_t types
#include <inttypes.h>

// Bring in the MHV IO header
#include <MHV_io.h>

// Bring in the MHV 8 bit timer header
#include <MHV_Timer8.h>

// Bring in the AVR delay header (needed for _delay_ms)
#include <util/delay.h>

// Bring in the AVR interrupt header (needed for cli)
#include <avr/interrupt.h>

/* Declare an 8 bit timer - we will use Timer 2 since it is an 8 bit timer
 * on all microcontrollers used on Arduino boards
 */
MHV_Timer8 tickTimer(MHV_TIMER8_2);

/* Each timer module generates interrupts
 * We must assign the timer object created above to handle these interrupts
 */
MHV_TIMER_ASSIGN_2INTERRUPTS(tickTimer, MHV_TIMER2_INTERRUPTS);

/* A trigger function that will be called every time the timer is triggered
 * Since we want a timer triggered every 333ms, but the hardware is incapable
 * of this, we will instead trigger every 1ms and maintain a counter instead
 */
void timerTrigger(void *data) {
// static variables are initialised once at boot, and persist between calls

// A counter to keep track of how many times we are called
	static uint16_t count = 0;

// What is the next action to take
	static bool turnOn = true;

/* Increment count, and if it is the 333rd time, toggle the LED and reset the
 * counter
 */
	if (333 == ++count) {
		if (turnOn) {
			mhv_pinOn(MHV_ARDUINO_PIN_13);
		} else {
			mhv_pinOff(MHV_ARDUINO_PIN_13);
		}

		turnOn = !turnOn;
		count = 0;
	}
}

int main() {
// Enable output on pin 13 of the Arduino - this normally has an LED connected
	mhv_setOutput(MHV_ARDUINO_PIN_13);

/* We want the timer to trigger ever 333ms, however, the maximum an 8 bit timer
 * can count to is 32768us @ 16MHz
 * Instead, we will trigger the timer every 1 ms, and increment a counter within
 * the trigger function to give us our 333ms count
 */
	tickTimer.setPeriods(1000UL, 0);

// Tell the timer to call our trigger function
	tickTimer.setTriggers(timerTrigger, 0, 0, 0);

// Start the timer
	tickTimer.enable();

// Enable interrupts
	sei();

// Do nothing forever - the timer will call the trigger() function periodically
	for(;;) {}

// Main must return an int, even though we never get here
	return 0;
}
