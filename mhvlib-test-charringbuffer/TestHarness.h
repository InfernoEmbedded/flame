#ifndef __MHVLIB_TESTINGHARNESS
#define __MHVLIB_TESTINGHARNESS 1

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
 public:
}; // end class TestHarness

#endif

