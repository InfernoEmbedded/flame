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

/* Uploadable thingy to test the Character Ring Buffer implementation
 */



// Bring in the MHV IO header
#include <mhvlib/io.h>

// Bring in the MHV Serial header
#include <mhvlib/HardwareSerial.h>

// Bring in the AVR delay header (needed for _delay_ms)
#include <util/delay.h>

// Bring in the AVR interrupt header (needed for cli)
#include <avr/interrupt.h>

// Bring in the AVR PROGMEM header, needed to store data in PROGMEM
#include <avr/pgmspace.h>

// Bring in stdio, required for snprintf
#include <stdio.h>

#include <string.h>
#include <mhvlib/CharRingBuffer.h>
#include <TestHarness.h>

#define RINGBUFFER_SIZE 80
mhvlib::CharRingBufferImplementation<RINGBUFFER_SIZE> testRingBuffer;

class TestRingBuffer : public TestHarness {
public:
	void torture() {
		const uint16_t max_iterations = 10000;
		char tmpbuf[30];
		for(uint16_t i=0;i<max_iterations;i++) {
			uint8_t operation = rand() % 4;
			if (operation == 0) {
				testRingBuffer.append(i % 256);
			} else if (operation == 1) {
				testRingBuffer.consume();
			} else if (operation == 2) {
				testRingBuffer.append((void*)"MaryMaryQuiteContrary",i%13);
			} else if (operation == 3) {
				testRingBuffer.consume((void*)tmpbuf,i%30);
			}
		}
	}

	void runTests() { // FIXME: add noreturn attribute here
#define TMPBUFFERSIZE 40
		char tmpbuffer[TMPBUFFERSIZE];

		is(testRingBuffer.size(),
		   (uint16_t)RINGBUFFER_SIZE,
		   PSTR("Initial size what we expect"));
		is(testRingBuffer.head(),
		   (uint16_t)0,
		   PSTR("Initial head where we expect"));
		is(testRingBuffer.used(),
		   (uint16_t)0,
		   PSTR("Initial used what we expect"));
		is(testRingBuffer.length(),
		   (uint16_t)0,
		   PSTR("Initial length what we expect"));
		is(testRingBuffer.full(RINGBUFFER_SIZE),
		   false,
		   PSTR("fit obj size=RINGBUFFER_SIZE"));
		is(testRingBuffer.full(RINGBUFFER_SIZE + 1),
		   true,
		   PSTR("NOT fit obj size=RINGBUFFER_SIZE+1"));
		is(testRingBuffer.freeSpace(),
		   (uint16_t)RINGBUFFER_SIZE,
		   PSTR("freeSpace is initally the buffer size"));
		is(testRingBuffer.peekHead(),
		   (int16_t)-1,
		   PSTR("peekHead initially returns -1"));
		is(testRingBuffer.consume(),
		   (int)-1,
		   PSTR("Consume on initial buffer returns -1"));
		is(testRingBuffer.consume(&tmpbuffer,1),
		   true, // failure!
		   PSTR("consume into a void* initially returns failure"));

		char firstcharpushed = 'Y';
		is(testRingBuffer.append(firstcharpushed),
		   false, // success!
		   PSTR("append a character to the buffer"));

		is(testRingBuffer.size(),
		   (uint16_t)RINGBUFFER_SIZE,
		   PSTR("Size what we expect"));
		is(testRingBuffer.head(),
		   (uint16_t)1,
		   PSTR("Head where we expect"));
		is(testRingBuffer.used(),
		   (uint16_t)1,
		   PSTR("used what we expect"));
		is(testRingBuffer.length(),
		   (uint16_t)1,
		   PSTR("length what we expect"));
		is(testRingBuffer.full(RINGBUFFER_SIZE-1),
		   false,
		   PSTR("Can fit an object of size RINGBUFFER_SIZE-1"));
		is(testRingBuffer.full(RINGBUFFER_SIZE + 1),
		   true,
		   PSTR("Can now NOT fit an object of size RINGBUFFER_SIZE"));
		is(testRingBuffer.freeSpace(),
		   (uint16_t)RINGBUFFER_SIZE-1,
		   PSTR("freeSpace is now RINGBUFFER_SIZE-1"));
		is(testRingBuffer.peekHead(),
		   (int16_t)firstcharpushed,
		   PSTR("peekHead returns char we pushed in"));
		is(testRingBuffer.consume(),
		   (int)firstcharpushed,
		   PSTR("Consume on buffer returns char we pushed in"));
		// is(testRingBuffer.consume(&tmpbuffer,1),
		//    true, // failure!
		//    PSTR("consume into a void* initially returns failure"));

		is(testRingBuffer.consume(),
		   (int)-1,
		   PSTR("Consume not returns -1"));

		is(testRingBuffer.append((void*)"Fred", 4),
		   false, // success
		   PSTR("Can append a void*"));
		is(testRingBuffer.peekHead(),
		   (int)'d',
		   PSTR("Head character as expected"));
		is(testRingBuffer.peek(),
		   (int)'F',
		   PSTR("Next character as expected"));
		is(testRingBuffer.consume(),
		   (int)'F',
		   PSTR("First consumed character as expected"));
		is(testRingBuffer.peekHead(),
		   (int)'d',
		   PSTR("Post-consume head character as expected"));
		is(testRingBuffer.peek(),
		   (int)'r',
		   PSTR("Post-consume Next character as expected"));
		is(testRingBuffer.consume((void*)tmpbuffer,4),
		   true, // failure
		   PSTR("Fail to consume more than is in the buffer"));
		is(testRingBuffer.consume((void*)tmpbuffer,3),
		   false, //success
		   PSTR("Consume from buffer"));
		is_strl_P(tmpbuffer,
			  PSTR("red"),
			  3,
			  PSTR("Buffer popped off the data we were expecting"));
		is_not(testRingBuffer.head(),
		       (uint16_t)0,
		       PSTR("buffer head is not zero after buffer was used"));
		is(testRingBuffer.used(),
		   (uint16_t)0,
		   PSTR("used is zero before flush"));

		testRingBuffer.flush();
		is(testRingBuffer.head(),
		   (uint16_t)0,
		   PSTR("buffer head is zero after flush"));
		is(testRingBuffer.used(),
		   (uint16_t)0,
		   PSTR("used is zero after flush"));

#define secondcharpushed 'X'
		is(testRingBuffer.append(secondcharpushed),
		   false, // success
		   PSTR("Append a character"));
		is(testRingBuffer.used(),
		   (uint16_t)1,
		   PSTR("used is 1 after append"));
		testRingBuffer.flush();
		is(testRingBuffer.used(),
		   (uint16_t)0,
		   PSTR("used is 0 after flush"));

		// start capacity check:
		int i;
		for(i=0;i<RINGBUFFER_SIZE;i++) {
			if (true == testRingBuffer.append(secondcharpushed)) {
				// failure
				is(1,
				   0,
				   PSTR("Append lots of characters to buffer"));
				break;
			}
		}
		if (i == RINGBUFFER_SIZE) {
			is(1,
			   1,
			   PSTR("Append lots of characters to buffer"));
		}
		is(testRingBuffer.append('Y'),
		   true, // failure
		   PSTR("Fail to push onto ring buffer more than it can hold"));
		is(testRingBuffer.consume(),
		   (int)secondcharpushed,
		   PSTR("consume out of the character ring buffer"));
		is(testRingBuffer.append('Y'),
		   false, // success
		   PSTR("push onto ring buffer after consuming something"));

		for(i=0;i<RINGBUFFER_SIZE-1;i++) {
			if ((int)secondcharpushed != testRingBuffer.consume()) {
				// failure
				is(1,
				   0,
				   PSTR("Consume expected character"));
				break;
			}
		}
		if (i == RINGBUFFER_SIZE-1) {
			is(1,
			   1,
			   PSTR("Consume expected character"));
		}
		is (testRingBuffer.consume(),
		    (int)'Y',
		    PSTR("Consume expected character"));
		// end capacity check

		testRingBuffer.flush();
		testStart(PSTR("Torture"));
		torture();
		testOK();
	}
};

TestRingBuffer tester;

MAIN {
	sei(); // move this?

	tester.run();
	for (;;) {}
}
