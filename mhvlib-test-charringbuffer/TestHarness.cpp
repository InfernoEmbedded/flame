#include <TestHarness.h>
#include <HardwareSerial.h>

// mhvlib::HardwareSerial<MHV_USART0, 115200, 1, 1> serial;
// MHV_HARDWARESERIAL_ASSIGN_INTERRUPTS(serial, MHV_USART0 ## _INTERRUPTS);
namespace mhvlib {
	MHV_HARDWARESERIAL_CREATE(serial, 1, 1, MHV_USART0, 115200);
}

void TestHarness::testStart(PGM_P testname) {
	mhvlib::serial.busyWrite_P(PSTR("Test: "));
	mhvlib::serial.busyWrite_P(testname);
	mhvlib::serial.busyWrite(": ");
	int16_t paddingrequired = padlen-strlen_P(testname);
	while (paddingrequired >= 0) {
		mhvlib::serial.busyWrite(' ');
		paddingrequired--;
	}
}
void TestHarness::testOK() {
	mhvlib::serial.busyWrite_P(PSTR("OK\r\n"));
	passedTestCount++;
}
void TestHarness::testFailed() {
	mhvlib::serial.busyWrite_P(PSTR("FAILED\r\n"));
	failedTestCount++;
}
void TestHarness::testFailed(const char * explanaton) {
	mhvlib::serial.busyWrite_P(PSTR("FAILED ("));
	mhvlib::serial.busyWrite(explanaton);
	mhvlib::serial.busyWrite(")\r\n");
	failedTestCount++;
}

void TestHarness::is_strl_P(const char * got,
			    PGM_P expected,
			    const uint16_t expectedlen,
			    PGM_P description) {
	testStart(description);
	if(strncmp_P(got,expected,expectedlen) == 0) {
		testOK();
	} else {
		mhvlib::serial.busyWrite_P(PSTR("FAILED (expected="));
		mhvlib::serial.busyWrite_P(expected,expectedlen);
		mhvlib::serial.busyWrite_P(PSTR(" ne got="));
		mhvlib::serial.busyWrite(got,expectedlen);
		mhvlib::serial.busyWrite(")\r\n");
		failedTestCount++;
	}
}

void TestHarness::run() {
	mhvlib::serial.busyWrite_P(PSTR("Tests Start\r\n"));
	runTests();
	mhvlib::serial.busyWrite_P(PSTR("Tests complete\r\n"));
}
