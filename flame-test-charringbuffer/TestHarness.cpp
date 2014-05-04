#include <TestHarness.h>
#include <flame/HardwareSerial.h>

// flame::HardwareSerial<FLAME_USART0, 115200, 1, 1> serial;
// FLAME_HARDWARESERIAL_ASSIGN_INTERRUPTS(serial, FLAME_USART0 ## _INTERRUPTS);
namespace flame {
	FLAME_HARDWARESERIAL_CREATE(serial, 1, 1, FLAME_USART0, 115200);
}

void TestHarness::testStart(PGM_P testname) {
	flame::serial.busyWrite_P(PSTR("Test: "));
	flame::serial.busyWrite_P(testname);
	flame::serial.busyWrite(": ");
	int16_t paddingrequired = padlen-strlen_P(testname);
	while (paddingrequired >= 0) {
		flame::serial.busyWrite(' ');
		paddingrequired--;
	}
}
void TestHarness::testOK() {
	flame::serial.busyWrite_P(PSTR("OK\r\n"));
	passedTestCount++;
}
void TestHarness::testFailed() {
	flame::serial.busyWrite_P(PSTR("FAILED\r\n"));
	failedTestCount++;
}
void TestHarness::testFailed(const char * explanaton) {
	flame::serial.busyWrite_P(PSTR("FAILED ("));
	flame::serial.busyWrite(explanaton);
	flame::serial.busyWrite(")\r\n");
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
		flame::serial.busyWrite_P(PSTR("FAILED (expected="));
		flame::serial.busyWrite_P(expected,expectedlen);
		flame::serial.busyWrite_P(PSTR(" ne got="));
		flame::serial.busyWrite(got,expectedlen);
		flame::serial.busyWrite(")\r\n");
		failedTestCount++;
	}
}

void TestHarness::torture_verbose_P(PGM_P string) {
  flame::serial.busyWrite_P(string);
}
void TestHarness::torture_verbose(const char * string) {
  flame::serial.busyWrite(string);
}
void TestHarness::torture_verbose(const uint16_t myinteger) {
  flame::serial.busyWrite(myinteger);
}

void TestHarness::run() {
	flame::serial.busyWrite_P(PSTR("Tests Start\r\n"));
	runTests();
	flame::serial.busyWrite_P(PSTR("Tests complete\r\n"));
}

void TestHarness::runTests() {};

