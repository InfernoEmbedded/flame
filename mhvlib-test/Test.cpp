/*
 * Test.cpp
 *
 *  Created on: 20/02/2010
 *      Author: Deece
 */

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>

extern "C" void __cxa_pure_virtual() {
        cli();
        for (;;);
}

#include <MHV_HardwareSerial.h>
#include <MHV_Timer8.h>
#include <MHV_Timer16.h>
#include <MHV_RingBuffer.h>
#include <MHV_io.h>
#include <MHV_ServoControl.h>

#define RX_BUFFER_SIZE	81
char rxBuf[RX_BUFFER_SIZE];
MHV_RingBuffer rxBuffer(rxBuf, RX_BUFFER_SIZE);

MHV_HardwareSerial serial(&rxBuffer, MHV_USART0, 115200);
MHV_HARDWARESERIAL_ASSIGN_INTERRUPTS(serial, MHV_USART0_INTERRUPTS);

MHV_Timer8 timer(MHV_TIMER8_2);
MHV_TIMER_ASSIGN_2INTERRUPTS(timer, MHV_TIMER2_INTERRUPTS);

#define SERVO_COUNT 1
MHV_Timer16 servoTimer(MHV_TIMER16_1);
MHV_TIMER_ASSIGN_1INTERRUPT(servoTimer, MHV_TIMER1_INTERRUPTS);
MHV_SERVOCONTROLBLOCK servoControlBlocks[SERVO_COUNT];
MHV_ServoControl servos(&servoTimer, servoControlBlocks, SERVO_COUNT);

char progmemSync[] PROGMEM = "Busy writes, progmem string\r\n";
char progmemAsync[] PROGMEM = "Async writes, progmem string\r\n";

char progmemSyncBuffer[] PROGMEM = "Busy writes, progmem buffer\r\n";
char progmemAsyncBuffer[] PROGMEM = "Async writes, progmem buffer\r\n";

bool ledOn = false;
uint8_t count = 0;

void toggleLED(void *data) {
	if (!count) {
		if (ledOn) {
			mhv_pinOff(MHV_ARDUINO_PIN_13);
			ledOn = false;
		} else {
			mhv_pinOn(MHV_ARDUINO_PIN_13);
			ledOn = true;
		}
		count = 5; // toggle every 5 * 30ms
		} else {
		count--;
	}
}

uint16_t servoPosition = 0;
void moveServo(void *data) {
	servoPosition += 100;
	servos.positionServo(0, servoPosition);
}

MHV_SERVOCONTROL_DECLARE_TRIGGER(servoTimer, servos);

int main(void) {
	char txBuf[81];

	serial.busyWrite("Starting up\r\n");

	mhv_setOutput(MHV_ARDUINO_PIN_13);

	timer.setTriggers(&toggleLED, 0, &moveServo, 0);
	timer.setPeriods(30000UL, 20000UL); // 30ms - max an 8 bit timer can count is 32768us @ 16MHz
	timer.enable();

	MHV_SERVOCONTROL_ASSIGN_TRIGGER(servoTimer, servos);
	servos.addServo(0, MHV_ARDUINO_PIN_12);
	servos.enable();

	while (1) {
		// String writes
		snprintf(txBuf, sizeof(txBuf), "Busy writes, %s\r\n", "normal string");
		// Not run in conjunction with the asynchronous calls, so it is safe to ignore return code
		(void) serial.busyWrite(txBuf);
		(void) serial.busyWrite_P(progmemSync);

		/* busyWrite does not wait for the last byte to be sent, so if we're going to do an async write
		 * we must wait here or the write will fail
		 */
		while (!serial.canSend()) {}
		(void) serial.asyncWrite("Async writes, normal string\r\n");
		// Wait until we're done transmitting asynchronously
		while (!serial.canSend()) {}
		(void) serial.asyncWrite_P(progmemAsync);
		while (!serial.canSend()) {}

		// Buffer writes
		(void) serial.busyWrite("Busy writes, normal buffer\r\n", 28);
		(void) serial.busyWrite_P(progmemSyncBuffer, sizeof(progmemSyncBuffer) - 1);

		/* busyWrite does not wait for the last byte to be sent, so if we're going to do an async write
		 * we must wait here or the write will fail
		 */
		while (!serial.busy()) {}
		(void) serial.asyncWrite("Async writes, normal buffer\r\n", 29);
		// Wait until we're done transmitting asynchronously
		while (!serial.canSend()) {}
		(void) serial.asyncWrite_P(progmemAsyncBuffer, sizeof(progmemAsyncBuffer) - 1);
		while (!serial.canSend()) {}

		// Test reads
		serial.busyWrite("OK, now type something and press enter\r\n");
		serial.echo(true);
		// We will use txBuf here to store the incoming data, since we don't need the buffer at this point
		while (-1 == serial.readLine(txBuf, sizeof(txBuf))) {};
		serial.echo(false);
		serial.busyWrite("\nYou said: '");
		serial.busyWrite(txBuf);
		serial.busyWrite("'\r\n");
	}
}
