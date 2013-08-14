#ifndef __MHVLIB_TESTINGHARNESS
#define __MHVLIB_TESTINGHARNESS 1

#include <avr/pgmspace.h>
#include <stdlib.h> // for malloc/free
#include <avr/pgmspace.h>

class TestHarness {
 public:
	int passedTestCount = 0;
	int failedTestCount = 0;
	const uint8_t padlen = 50;

	void run();
	void testStart(PGM_P testname);
	void testOK();
	void testFailed();
	void testFailed(const char * explanaton);
	void torture_verbose(const char * message);
	void torture_verbose_P(const char * message);
	void torture_verbose(const uint16_t message);

	virtual void runTests();

	void is_strl_P(const char * got,
		       PGM_P expected,
		       const uint16_t expectedlen,
		       PGM_P description);

	template <class value_t>
		void is_not(value_t value,
			    value_t expected,
			    PGM_P description) {

		testStart(description);
		if (value != expected) {
			testOK();
		} else {
			char buffer[80];
			snprintf(buffer,80,"got=%d expected=%d",value,expected);
			testFailed(buffer);
		}
	}
	template <class value_t>
		void is(value_t value,
			value_t expected,
			PGM_P description) {

		testStart(description);
		if (value == expected) {
			testOK();
		} else {
			char buffer[80];
			snprintf(buffer,80,"got=%d expected=%d",value,expected);
			testFailed(buffer);
		}
	}
		    
	template <class value_t>
		void isgt(value_t a,
			  value_t b,
			  PGM_P description) {

		testStart(description);
		if (a > b) {
			testOK();
		} else {
			char buffer[80];
			snprintf(buffer,80,"a=%d < b=%d",a,b);
			testFailed(buffer);
		}
	}

	/* virtual ~TestHarness(); */
	/* TestHarness(); */
	void operator delete(void * ptr) {
	  free(ptr);
	} 
	void * operator new(size_t size) {
	  return malloc(size);
	}

 public:
}; // end class TestHarness

#endif

