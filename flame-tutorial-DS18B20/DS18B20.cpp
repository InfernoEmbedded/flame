/*
 * Copyright (c) 2014, Inferno Embedded
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of the Inferno Embedded nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL INFERNO EMBEDDED BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Read temperatures from DS18B20 sensors connected to a 1Wire bus driven by Arduino pin 12 (B4)
 */

// Bring in the FLAME IO header
#include <flame/io.h>

// Bring in the FLAME Serial header
#include <flame/HardwareSerial.h>

// Bring in the Pin header
#include <boards/Arduino.h>
#include <flame/Pin.h>

// Bring in the OneWire driver
#include <flame/OneWire.h>

// Bring in the DS18B20 driver
#include <flame/DS18B20.h>

// Bring in the AVR interrupt header (needed for sei)
#include <avr/interrupt.h>

// Bring in the RTC
#include <flame/Timer.h>
#include <flame/RTC.h>

#include <avr/power.h>
#include <avr/sleep.h>

using namespace flame;


// Create a buffer we will use for a receive buffer
#define RX_BUFFER_SIZE	3
// The number of elements we want to be able to store to send asynchronously
#define TX_ELEMENTS_COUNT 32
/* Declare the serial object on USART0
 * Set the baud rate to 115,200
 */
FLAME_HARDWARESERIAL_CREATE(serial, RX_BUFFER_SIZE, TX_ELEMENTS_COUNT, FLAME_USART0, 115200);


// A timer we will use to tick the RTC
TimerImplementation<FLAME_TIMER8_0, TimerMode::REPETITIVE>tickTimer;
FLAME_TIMER_ASSIGN_1INTERRUPT(tickTimer, FLAME_TIMER0_INTERRUPTS);

/* A buffer the RTC will use to store alarms - this determines how many alarms
 * can be registered simultaneously
 */
#define ALARM_COUNT	10
#define TIMEZONE 600 // UTC+10

// The RTC object we will use
RTCImplementation<ALARM_COUNT> rtc(TIMEZONE);

// The pin to speak 1-Wire over
PinImplementation<FLAME_ARDUINO_PIN_12> oneWirePin;

// The 1-Wire bus driver
OneWire oneWire(oneWirePin);

// The factory class for all DS18B20 sensors on the bus
DS18B20Factory temperatureSensors(oneWire);


class TemperaturePrinter : public TimerListener {
	/* An event that will print the temperature of all DS18B20 devices on the bus
	 */
	void alarm(UNUSED AlarmSource source) {
		uint8_t deviceCount = temperatureSensors.count();
		for (uint8_t deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++) {
			DS18B20 *device = temperatureSensors.getDevice(deviceIndex);
			while (!device->isReady()) {
				serial.printf(PSTR("Device[%d] is not ready\r\n"), deviceIndex);
				_delay_ms(500);
			}

			/* Get the temperature from channel 0, as the DS18B20 only has 1 channel
			 * Note that as we are printing a float here, we must link the floating point enabled
			 * printf with: -Wl,-u,vfprintf -lprintf_flt -lm
			 * For more details, see http://www.nongnu.org/avr-libc/user-manual/group__avr__stdio.html
			 */
			serial.printf(PSTR("Device[%d] temperature=%3.3f\r\n"), deviceIndex, device->getTemperature(0));
		}
	}
};
TemperaturePrinter temperaturePrinter;

class TemperatureRequester : public TimerListener {
	/* An event that will request the temperature from all devices on the bus
	 */
	void alarm(UNUSED AlarmSource source) {
		serial.write_P(PSTR("\r\nRequesting read on all devices\r\n"));
		temperatureSensors.readAll();
		// Schedule a read of all sensors in 1 second (DS18B20 sensors in 12 bit mode take 750ms for a reading)
		rtc.addAlarm(temperaturePrinter, 1 /* seconds */, 0 /* milliseconds */);
	}
};
TemperatureRequester temperatureRequester;

/**
 * Show what sensors were found on the bus
 */
void showSensors() {
	uint8_t deviceCount = temperatureSensors.count();
	serial.printf(PSTR("Found %d devices on the bus\r\n"), deviceCount);

	for (uint8_t deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++) {
		DS18B20 *device = temperatureSensors.getDevice(deviceIndex);
		serial.printf(PSTR("Device[%d] has %d bit resolution and requires %d ms read delay\r\n"), deviceIndex,
				device->getResolution(), device->getReadDelay());
	}
}

MAIN {
	// Disable all peripherals and enable just what we need
	power_all_disable();
	power_timer0_enable();
	power_usart0_enable();
	set_sleep_mode(SLEEP_MODE_IDLE);

	// Enable interrupts
	sei();

	// Configure the tick timer to tick every 1ms (at 16MHz)
	tickTimer.setPeriods(TimerPrescaler::PRESCALER_5_64, 249, 0);
	tickTimer.setListener1(rtc);
	tickTimer.enable();

	showSensors();

	// Insert a repeating event to poll the bus every 10 seconds
	if (rtc.addAlarm(temperatureRequester,
			1, 0,	// When (seconds, milliseconds)
			10, 0)) { // Repeat (seconds, milliseconds)
		serial.write_P(PSTR("Adding alarm failed\r\n"));
	}

	// main loop
	for (;;) {
		rtc.handleEvents();
		sleep_mode();
	}
}
