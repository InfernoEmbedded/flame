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
#include <MHV_AD.h>

#define RX_BUFFER_SIZE	81
char rxBuf[RX_BUFFER_SIZE];
MHV_RingBuffer rxBuffer(rxBuf, RX_BUFFER_SIZE);

MHV_HardwareSerial serial(&rxBuffer, MHV_USART0, 115200);
MHV_HARDWARESERIAL_ASSIGN_INTERRUPTS(serial, MHV_USART0_INTERRUPTS);

MHV_Timer8 tickTimer(MHV_TIMER8_2);
MHV_TIMER_ASSIGN_2INTERRUPTS(tickTimer, MHV_TIMER2_INTERRUPTS);

#define SERVO_COUNT 1
MHV_Timer16 servoTimer(MHV_TIMER16_3);
MHV_TIMER_ASSIGN_1INTERRUPT(servoTimer, MHV_TIMER3_INTERRUPTS);
MHV_SERVOCONTROLBLOCK servoControlBlocks[SERVO_COUNT];
MHV_ServoControl servos(&servoTimer, servoControlBlocks, SERVO_COUNT);

char progmemSync[] PROGMEM = "Busy writes, progmem string\r\n";
char progmemAsync[] PROGMEM = "Async writes, progmem string\r\n";

char progmemSyncBuffer[] PROGMEM = "Busy writes, progmem buffer\r\n";
char progmemAsyncBuffer[] PROGMEM = "Async writes, progmem buffer\r\n";

bool ledOn = false;
uint8_t countLED = 0;
uint8_t countADC = 0;

char tickBuf[81];

// VCC is 5 volts
#define VCC 5
void adcTrigger(uint16_t adc) {
	if (MHV_AD_CHANNEL_0 == MHV_AD_CHANNEL) {
		uint32_t voltage = adc;
		voltage *= VCC * 1000;
		voltage /= MHV_AD_MAX;
		snprintf(tickBuf, sizeof(tickBuf), "ADC is %d, Voltage is %ldmV\r\n", adc, voltage);
#if defined(__AVR_ATmega48__) || defined(__AVR_ATmega48A__) || defined(__AVR_ATmega88__) || \
		defined(__AVR_ATmega88A__) || defined(__AVR_ATmega88PA__) || defined(__AVR_ATmega168__) || \
		defined(__AVR_ATmega168A__) || defined(__AVR_ATmega168PA__) || defined(__AVR_ATmega328__) || \
		defined(__AVR_ATmega328P__)
	} else { // if (MHV_AD_TEMPERATURE == MHV_AD_CHANNEL)
		// Temperature is quite inaccurate, unless calibrated
		snprintf(tickBuf, sizeof(tickBuf), "Temperature is about %f\6fC\r\n", (double)((adc + 287) * 1.1) / MHV_AD_MAX);
#endif
	}
	while (!serial.canSend()) {}
	(void) serial.asyncWrite(tickBuf);
}

MHV_AD_ASSIGN_INTERRUPT(adcTrigger);

void inline tickLED(void) {
	if (!countLED) {
		if (ledOn) {
			mhv_pinOff(MHV_ARDUINO_PIN_13);
			ledOn = false;
		} else {
			mhv_pinOn(MHV_ARDUINO_PIN_13);
			ledOn = true;
		}

		countLED = 5; // toggle every 5 * 30ms
	} else {
		countLED--;
	}
}


void inline tickADC(void) {
	if (!countADC) {
		mhv_ad_asyncRead(MHV_AD_CHANNEL_0, MHV_AD_REFERENCE_AVCC);
		countADC = 100; // read the ADC every 100 x 30ms
#if defined(__AVR_ATmega48__) || defined(__AVR_ATmega48A__) || defined(__AVR_ATmega88__) || \
		defined(__AVR_ATmega88A__) || defined(__AVR_ATmega88PA__) || defined(__AVR_ATmega168__) || \
		defined(__AVR_ATmega168A__) || defined(__AVR_ATmega168PA__) || defined(__AVR_ATmega328__) || \
		defined(__AVR_ATmega328P__)
	} else if (50 == countADC) {
		mhv_ad_asyncRead(MHV_AD_TEMPERATURE, MHV_AD_REFERENCE_1V1);
#endif
	} else {
		countADC--;
	}
}

uint16_t servoPosition = 0;
void moveServo(void *data) {
	servoPosition += 100;
	servos.positionServo(0, servoPosition);
}


#define PWM_INCREMENT 10
#define PWM_TOP 1000
MHV_Timer16 pwmTimer(MHV_TIMER16_1);
bool pwmDir = 0; // 0 = down, 1 == up
void tickPWM(void) {
	uint16_t current = pwmTimer.getOutput2();
#if 0
	while (!serial.canSend()) {}
	snprintf(tickBuf, sizeof(tickBuf), "PWM output is %d, compA=%d compB=%d\r\n", current, OCR1A, OCR1B);
	(void) serial.asyncWrite(tickBuf);
#endif

	if (pwmDir && current + PWM_INCREMENT >= PWM_TOP / 2) {
		pwmDir = 0;
		pwmTimer.setOutput2(PWM_TOP / 2);
	} else if (!pwmDir && current <= PWM_INCREMENT) {
		pwmDir = 1;
		pwmTimer.setOutput2(0);
	} else if (pwmDir) {
		pwmTimer.setOutput2(current + PWM_INCREMENT);
	} else {
		pwmTimer.setOutput2(current - PWM_INCREMENT);
	}
}


void tick30ms(void *data) {
	tickLED();
	tickADC();
	moveServo(NULL);
	tickPWM();
}


MHV_SERVOCONTROL_DECLARE_TRIGGER(servoTimer, servos);

int main(void) {
	char txBuf[81];
	sei();

	serial.busyWrite("Starting up\r\n");

	MHV_AD_ENABLE;
	MHV_AD_ENABLE_INTERRUPT;
	mhv_ad_setPrescaler(MHV_AD_PRESCALER_128);

	// Blinking LED on pin 13
	mhv_setOutput(MHV_ARDUINO_PIN_13);

	// Set up PWM output on pin 12 (OCR1B)
	mhv_setOutput(MHV_ARDUINO_PIN_12);
	pwmTimer.setMode(MHV_TIMER_16_PWM_FAST);
	pwmTimer.setPrescaler(MHV_TIMER_PRESCALER_5_1);
	pwmTimer.setTop(PWM_TOP); // PWM freq = 20MHz / 2 / 1000 = 10kHz
	pwmTimer.connectOutput2(MHV_TIMER_CONNECT_CLEAR);
	pwmTimer.setOutput2(0); // This should only go as high as PWM_TOP
	pwmTimer.enable();

	// Set up a timer for automation
	tickTimer.setTriggers(&tick30ms, 0, &moveServo, 0);
	tickTimer.setPeriods(30000UL, 20000UL); // 30ms - max an 8 bit timer can count is 32768us @ 16MHz
	tickTimer.enable();

	MHV_SERVOCONTROL_ASSIGN_TRIGGER(servoTimer, servos);
	servos.addServo(0, MHV_ARDUINO_PIN_11);
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
