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

#define DO_TORTURE_VERBOSE 1

#include <string.h>
#include <mhvlib/CharRingBuffer.h>
#include <TestHarness.h>

#define TORTURE_VERBOSE(x) torture_verbose(x)

class TestCharRingBuffer : public TestHarness {
public:
	mhvlib::CharRingBuffer * testRingBuffer;
	uint16_t ringBufferSize;

	TestCharRingBuffer(mhvlib::CharRingBuffer * x, const uint16_t size) {
		testRingBuffer = x;
		ringBufferSize = size;
	}

	void torture() {
		const uint16_t max_iterations = 60000;
		char tmpbuf[30];
		for(uint16_t i=0;i<max_iterations;i++) {
			uint8_t operation = rand() % 4;
			if (operation == 0) {
				// TORTURE_VERBOSE("appendChar\r\n");
				testRingBuffer->append(i % 256);
			} else if (operation == 1) {
				// TORTURE_VERBOSE("consumeChar\r\n");
				testRingBuffer->consume();
			} else if (operation == 2) {
				uint8_t x = i%13;
				// TORTURE_VERBOSE("appendChar*=");
				// TORTURE_VERBOSE(x);
				// TORTURE_VERBOSE("\r\n");
				testRingBuffer->append((void*)"MaryMaryQuiteContrary",x);
			} else if (operation == 3) {
				uint8_t x = i%30;
				// TORTURE_VERBOSE("consumeChar*=");
				// TORTURE_VERBOSE(x);
				// TORTURE_VERBOSE("\r\n");
				testRingBuffer->consume((void*)tmpbuf,x);
			}
		}
	}

	void runTests() {
#define TMPBUFFERSIZE 40
		char tmpbuffer[TMPBUFFERSIZE];

		is(testRingBuffer->size(),
		   (uint16_t)ringBufferSize,
		   PSTR("Initial size what we expect"));
		is(testRingBuffer->head(),
		   (uint16_t)0,
		   PSTR("Initial head where we expect"));
		is(testRingBuffer->used(),
		   (uint16_t)0,
		   PSTR("Initial used what we expect"));
		is(testRingBuffer->length(),
		   (uint16_t)0,
		   PSTR("Initial length what we expect"));
		is(testRingBuffer->full(ringBufferSize),
		   false,
		   PSTR("fit obj size=ringBufferSize"));
		is(testRingBuffer->full(ringBufferSize + 1),
		   true,
		   PSTR("NOT fit obj size=ringBufferSize+1"));
		is(testRingBuffer->freeSpace(),
		   (uint16_t)ringBufferSize,
		   PSTR("freeSpace is initally the buffer size"));
		is(testRingBuffer->peekHead(),
		   (int16_t)-1,
		   PSTR("peekHead initially returns -1"));
		is(testRingBuffer->consume(),
		   (int)-1,
		   PSTR("Consume on initial buffer returns -1"));
		is(testRingBuffer->consume(&tmpbuffer,1),
		   testRingBuffer->failure(),
		   PSTR("consume into a void* initially returns failure"));

		char firstcharpushed = 'Y';
		is(testRingBuffer->append(firstcharpushed),
		   testRingBuffer->success(),
		   PSTR("append a character to the buffer"));
		is(testRingBuffer->peekAtOffset(0),
		   (int)firstcharpushed,
		   PSTR("peekAtOffset(0) returns firstcharpushed"));
		is(testRingBuffer->peekAtOffset(1),
		   (int)-1,
		   PSTR("peekAtOffset(1) returns -1"));
		is(testRingBuffer->size(),
		   (uint16_t)ringBufferSize,
		   PSTR("Size what we expect"));
		is(testRingBuffer->head(),
		   (uint16_t)1,
		   PSTR("Head where we expect"));
		is(testRingBuffer->used(),
		   (uint16_t)1,
		   PSTR("used what we expect"));
		is(testRingBuffer->length(),
		   (uint16_t)1,
		   PSTR("length what we expect"));
		is(testRingBuffer->full(ringBufferSize-1),
		   false,
		   PSTR("Can fit an object of size ringBufferSize-1"));
		is(testRingBuffer->full(ringBufferSize + 1),
		   true,
		   PSTR("Can now NOT fit an object of size ringBufferSize"));
		is(testRingBuffer->freeSpace(),
		   (uint16_t)ringBufferSize-1,
		   PSTR("freeSpace is now ringBufferSize-1"));
		is(testRingBuffer->peekHead(),
		   (int16_t)firstcharpushed,
		   PSTR("peekHead returns char we pushed in"));
		is(testRingBuffer->consume(),
		   (int)firstcharpushed,
		   PSTR("Consume on buffer returns char we pushed in"));
		// is(testRingBuffer->consume(&tmpbuffer,1),
		//    true, // failure!
		//    PSTR("consume into a void* initially returns failure"));

		is(testRingBuffer->consume(),
		   (int)-1,
		   PSTR("Consume now returns -1"));

		is(testRingBuffer->append((void*)"Fred", 4),
		   testRingBuffer->success(),
		   PSTR("Can append a void*"));
		is(testRingBuffer->length(),
		   (uint16_t)4,
		   PSTR("Content length is as expected"));
		is(testRingBuffer->peekHead(),
		   (int)'d',
		   PSTR("Head character as expected"));
		is(testRingBuffer->peek(),
		   (int)'F',
		   PSTR("Next character as expected"));
		is(testRingBuffer->consume(),
		   (int)'F',
		   PSTR("First consumed character as expected"));
		is(testRingBuffer->length(),
		   (uint16_t)3,
		   PSTR("Content length is shorter"));
		is(testRingBuffer->peekHead(),
		   (int)'d',
		   PSTR("Post-consume head character as expected"));
		is(testRingBuffer->peek(),
		   (int)'r',
		   PSTR("Post-consume Next character as expected"));
		is(testRingBuffer->consume((void*)tmpbuffer,4),
		   testRingBuffer->failure(),
		   PSTR("Fail to consume more than is in the buffer"));
		is(testRingBuffer->consume((void*)tmpbuffer,3),
		   testRingBuffer->success(),
		   PSTR("Consume from buffer"));
		is_strl_P(tmpbuffer,
			  PSTR("red"),
			  3,
			  PSTR("Buffer popped off the data we were expecting"));
		is_not(testRingBuffer->head(),
		       (uint16_t)0,
		       PSTR("buffer head is not zero after buffer was used"));
		is(testRingBuffer->used(),
		   (uint16_t)0,
		   PSTR("used is zero before flush"));

		testRingBuffer->flush();
		is(testRingBuffer->head(),
		   (uint16_t)0,
		   PSTR("buffer head is zero after flush"));
		is(testRingBuffer->used(),
		   (uint16_t)0,
		   PSTR("used is zero after flush"));

#define secondcharpushed 'X'
		is(testRingBuffer->append(secondcharpushed),
		   testRingBuffer->success(),
		   PSTR("Append a character"));
		is(testRingBuffer->used(),
		   (uint16_t)1,
		   PSTR("used is 1 after append"));
		testRingBuffer->flush();
		is(testRingBuffer->used(),
		   (uint16_t)0,
		   PSTR("used is 0 after flush"));

		// start capacity check:
		uint16_t i;
		for(i=0;i<ringBufferSize;i++) {
			if (testRingBuffer->append(secondcharpushed)== testRingBuffer->failure()) {
				// failure
				is(1,
				   0,
				   PSTR("Append lots of characters to buffer"));
				break;
			}
		}
		if (i == ringBufferSize) {
			is(1,
			   1,
			   PSTR("Append lots of characters to buffer"));
		}
		is(testRingBuffer->append('Y'),
		   testRingBuffer->failure(),
		   PSTR("Fail to push onto ring buffer more than it can hold"));
		is(testRingBuffer->consume(),
		   (int)secondcharpushed,
		   PSTR("consume out of the character ring buffer"));
		is(testRingBuffer->append('Y'),
		   testRingBuffer->success(),
		   PSTR("push onto ring buffer after consuming something"));

		for(i=0;i<ringBufferSize-1;i++) {
			int foo = testRingBuffer->consume();
			if (foo != (int)secondcharpushed) {
				// failure
				is(foo,
				   (int)secondcharpushed,
				   PSTR("Consume expected character"));
				break;
			}
		}
		if (i == ringBufferSize-1) {
			is(1,
			   1,
			   PSTR("Consume expected characters"));
		}
		is (testRingBuffer->consume(),
		    (int)'Y',
		    PSTR("Consume expected final character"));
		// end capacity check

		testRingBuffer->flush();
		testStart(PSTR("Torture"));
		torture();
		testOK();
	}
};

bool all_done_flag = false;
void set_all_done_flag(const void * done) {
	((char*)done)[0] = 'Y';
	all_done_flag = true;
}

class TestIndirectingRingBuffer : public TestCharRingBuffer {
public:
	mhvlib::IndirectingRingBuffer * testRingBuffer;
	TestIndirectingRingBuffer(mhvlib::IndirectingRingBuffer * x, const uint16_t size) : TestCharRingBuffer(x,size) {
		testRingBuffer = x;
	};

	void runTests() {
		//		TestCharRingBuffer::runTests();
		char tmp[10];

		// {
		// 	testRingBuffer->fnoo();
		// 	testRingBuffer->flush();
		// 	is(testRingBuffer->append('X'),
		// 	   testRingBuffer->success(),
		// 	   PSTR("Append an X"));
		// 	is(testRingBuffer->consume(),
		// 	   (int)'X',
		// 	   PSTR("Consume an X"));
		// }
		{
			testRingBuffer->flush();
			is (testRingBuffer->append(testRingBuffer->a_magic_escape_character()),
			    testRingBuffer->success(),
			    PSTR("Append a magic escape character"));
			is(testRingBuffer->used(),
			   (uint16_t)2,
			   PSTR("two characters used for escaping"));
			is(testRingBuffer->consume(),
			   (int)testRingBuffer->a_magic_escape_character(),
			   PSTR("Get a magic escape character back"));
			is(testRingBuffer->consume(),
			   (int)-1,
			   PSTR("No more to consume"));
		}		

		testRingBuffer->flush();

		{
			const uint8_t tmpBufferSize = 10;
			char tmp[tmpBufferSize];
			memset(tmp,'X',tmpBufferSize);
			const uint8_t offset = 4;
			tmp[offset] = testRingBuffer->a_magic_escape_character();
			is(testRingBuffer->append(tmp,tmpBufferSize),
			   testRingBuffer->success(),
			   PSTR("Append a string containing the magic escape character"));
			is(testRingBuffer->used(),
			   (uint16_t)tmpBufferSize+1,
			   PSTR("used is one more than what we put in"));
			is(testRingBuffer->length(),
			   (uint16_t)tmpBufferSize,
			   PSTR("length is what we put in"));
			for(uint8_t i = 0; i<tmpBufferSize; i++) {
				int x = testRingBuffer->consume();
				char expected = 'X';
				if (i == offset) {
					expected = testRingBuffer->a_magic_escape_character();
				}
				is((uint16_t)x,
				   (uint16_t)expected,
				   PSTR("retrieve expected character from ringbuffer"));
			}
			is(testRingBuffer->consume(),
			   -1,
			   PSTR("No more to consume"));
			memset(tmp,'X',tmpBufferSize);
			is(testRingBuffer->append(tmp,tmpBufferSize,&set_all_done_flag),
			   testRingBuffer->success(),
			   PSTR("Append a string to the ringbuffer with a success function"));
			for(uint8_t i = 0; i<tmpBufferSize; i++) {
				is(testRingBuffer->consume(),
				   (int)'X',
				   PSTR("retrieve expected character from ringbuffer"));
				if (i == tmpBufferSize-1) {
					is(all_done_flag,
					   true,
					   PSTR("success function called"));
				} else {
					is(all_done_flag,
					   false,
					   PSTR("success function NOT called"));
				}
			}
			is(testRingBuffer->consume(),
			   (int)-1,
			   PSTR("No more to consume"));
			is(tmp[0],
			   'Y',
			   PSTR("complete function screwed w/buffer"));
		}

		testRingBuffer->flush();

		PGM_P string = PSTR("0123456789");
		{
			is(testRingBuffer->append_P(string),
			   testRingBuffer->success(),
			   PSTR("append a PSTR"));
			is(testRingBuffer->length(),
			   (uint16_t)10,
			   PSTR("length of indirected buffer is 10"));
			is(testRingBuffer->used(),
			   (uint16_t)4,
			   PSTR("used of indirected buffer is 4"));
			is(testRingBuffer->append_P(string),
			   testRingBuffer->success(),
			   PSTR("append a PSTR again"));
			is(testRingBuffer->length(),
			   (uint16_t)20,
			   PSTR("length of indirected buffer is 20"));
			is(testRingBuffer->used(),
			   (uint16_t)8,
			   PSTR("used of indirected buffer is 8"));
			memset(tmp,'X',10);
			is(testRingBuffer->consume(tmp,10),
			   testRingBuffer->success(),
			   PSTR("Consume a pstring"));
			is_strl_P(tmp,
				  string,
				  10,
				  PSTR("Pop off what we expect from pstrings"));
			is(testRingBuffer->length(),
			   (uint16_t)10,
			   PSTR("length of indirected buffer is 10"));
			is(testRingBuffer->used(),
			   (uint16_t)4,
			   PSTR("used of indirected buffer is 4"));
			memset(tmp,'X',10);
			is(testRingBuffer->consume(tmp,10),
			   testRingBuffer->success(),
			   PSTR("Consume a pstring"));
			is_strl_P(tmp,
				  string,
				  10,
				  PSTR("Pop off what we expect from pstrings a second time"));
		}

		{
		  testRingBuffer->flush();
		  uint8_t copies_to_store = 10;
		  for(uint8_t i=0; i<copies_to_store;i++) {
		    testRingBuffer->append_P(string);
		  }
		  is(testRingBuffer->length(),
		     (uint16_t)100,
		     PSTR("length of ringbuffer is 100"));
		  isgt(testRingBuffer->length(),
		       ringBufferSize,
		       PSTR("more content in ringbuffer than its size would indicate"));
		  is(testRingBuffer->used(),
		     (uint16_t)40,
		     PSTR("used of ringbuffer is 40"));
		  is(testRingBuffer->used(),
		     (uint16_t)ringBufferSize,
		     PSTR("used is equal to ringBufferSize")); // you changed RINGBUFFER_SIZE, didn't you?
		  is(testRingBuffer->append_P(string),
		     testRingBuffer->failure(),
		     PSTR("failed to append string again (hopefully because full)"));
		  is (testRingBuffer->append('X'),
		      testRingBuffer->failure(),
		      PSTR("failed to append a single character when it is full"));
		  is(testRingBuffer->canFit('X'),
		     false,
		     PSTR("canFit says no more characters"));

		  memset(tmp,'X',10);
		  for(uint8_t i=0;i<copies_to_store;i++) {
		    is(testRingBuffer->consume(tmp,10),
		       testRingBuffer->success(),
		       PSTR("Consume a lengthed-string"));
		    is_strl_P(tmp,
			      string,
			      10,
			      PSTR("Pop off what we expect from pstrings"));
		  }
		  is(testRingBuffer->consume(tmp,10),
		     testRingBuffer->failure(),
		     PSTR("Fail to pop off any more data from ringbuffer"));
		}

		testRingBuffer->flush();

		{
			char foo[5];
			const char a_char = 'A';
			const char * null_terminated_string = "B";
			PGM_P progam_string = PSTR("C");
			const char * uint_string = "DDD"; // just 1 appended
			is (testRingBuffer->append(a_char),
			    testRingBuffer->success(),
			    PSTR("Append a char"));
			is (testRingBuffer->append(null_terminated_string),
			    testRingBuffer->success(),
			    PSTR("Append a null-terminated string"));
			is (testRingBuffer->append_P(progam_string),
			    testRingBuffer->success(),
			    PSTR("Append a program string"));
			is (testRingBuffer->append(uint_string,1),
			    testRingBuffer->success(),
			    PSTR("Append length of char*"));
			is(testRingBuffer->used(),
			   (uint16_t)4,
			   PSTR("four bytes used"));
			is(testRingBuffer->consume(&foo,4),
			   testRingBuffer->success(),
			   PSTR("consume 4 characters"));
			is_strl_P(foo,
				  PSTR("ABCD"),
				  4,
				  PSTR("Get back what we expect"));
		}
		{
			char foo[5];
			const char a_char = testRingBuffer->a_magic_escape_character();
			char * null_terminated_string = "B";
			null_terminated_string[0] = a_char;
			PGM_P progam_string = PSTR("|"); // sorry - don't change that next time!
			char * uint_string = "DDD"; // just 1 appended
			uint_string[0] = a_char;
			is (testRingBuffer->append(a_char),
			    testRingBuffer->success(),
			    PSTR("Append a char (magic_escape_character)"));
			is (testRingBuffer->append(null_terminated_string),
			    testRingBuffer->success(),
			    PSTR("Append a null-terminated string (mec)"));
			is (testRingBuffer->append_P(progam_string),
			    testRingBuffer->success(),
			    PSTR("Append a program string (mec)"));
			is (testRingBuffer->append(uint_string,1),
			    testRingBuffer->success(),
			    PSTR("Append length of char* (mec)"));
			is(testRingBuffer->used(),
			   (uint16_t)8,
			   PSTR("8 bytes used"));
			is(testRingBuffer->consume(&foo,4),
			   testRingBuffer->success(),
			   PSTR("consume 4 characters"));
			is_strl_P(foo,
				  PSTR("||||"), // again, sorry
				  4,
				  PSTR("Get back what we expect"));
			is(testRingBuffer->consume(),
			   (int)-1,
			   PSTR("ringbuffer now empty"));
		}
		   
	}
};


#define RINGBUFFER_SIZE 40
mhvlib::IndirectingRingBufferImplementation<RINGBUFFER_SIZE> fooBuffer;

MAIN {
	sei(); // move this?

	// TestCharRingBuffer * tester = new TestCharRingBuffer(fooBuffer,RINGBUFFER_SIZE);
	TestIndirectingRingBuffer * tester = new TestIndirectingRingBuffer(&fooBuffer,RINGBUFFER_SIZE);
	fooBuffer.fnoo();
	tester->run();
	for (;;) {}
}
