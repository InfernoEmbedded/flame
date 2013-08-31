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

/* Read the ADC value asynchronously and output it to the serial port
 */

// Bring in the MHV IO header
#include <mhvlib/io.h>

// Bring in the MHV timer and RTC headers
#include <mhvlib/Timer.h>
#include <mhvlib/RTC.h>

// Bring in the MHV ADC header
#include <mhvlib/ADCManager.h>

// Bring in the AVR interrupt header (needed for cli)
#include <avr/interrupt.h>
//
// Bring in the power management header
#include <avr/power.h>
#include <avr/sleep.h>

// Bring in the serial port header
#include <mhvlib/HardwareSerial.h>

// Bring in the AVR interrupt header (needed for sei)
#include <avr/interrupt.h>

// Bring in the AVR PROGMEM header, needed to store data in PROGMEM
#include <avr/pgmspace.h>


using namespace mhvlib;

// A timer we will use to tick the RTC
TimerImplementation<MHV_TIMER8_0, TimerMode::REPETITIVE> tickTimer;
MHV_TIMER_ASSIGN_1INTERRUPT(tickTimer, MHV_TIMER0_INTERRUPTS);

/* A buffer the RTC will use to store alarms - this determines how many alarms
 * can be registered simultaneously
 */
#define ALARM_COUNT	10
#define TIMEZONE 600 // UTC+10
// The RTC object we will use
RTCImplementation<ALARM_COUNT> rtc(TIMEZONE);

// A serial port to talk to the user with
#define RX_BUFFER_SIZE	4
// The number of elements we want to be able to store to send asynchronously
#define TX_ELEMENTS_COUNT 15
MHV_HARDWARESERIAL_CREATE(serial, RX_BUFFER_SIZE, TX_ELEMENTS_COUNT, MHV_USART0, 115200);

#define MAX_ADC_CHANNELS	3

// The ADC manager
MHV_ADC_CREATE(adc, MAX_ADC_CHANNELS, ADCPrescaler::DIVIDE_BY_128);

class OncePerSecond: public TimerListener {
	/**
	 * An event that we will trigger every second
	 * This will trigger an asynchronous read
	 * Execution will continue in the ADC listener assigned to the channel
	 */
	void alarm(UNUSED AlarmSource source) {
		adc.read(ADCChannel::CHANNEL_0, ADCReference::AVCC);
	}
};

OncePerSecond oncePerSecond;
//
class PrintADC : public ADCListener {
	/**
	 * Called when the ADC has a value for us
	 * @param ADCChannel	the channel the value was read from
	 * @param adcValue		the value from the ADC
	 */
	void adc(ADCChannel channel, uint16_t adcValue) {
		serial.write_P(PSTR("ADC Channel "));
		serial.write((uint8_t)channel);
		serial.write_P(PSTR(" = "));
		serial.write(adcValue);
		serial.write_P(PSTR("\r\n"));

// Schedule a read of the next channel
		if (channel < ADCChannel::CHANNEL_3) {
			::adc.read(channel + 1, ADCReference::AVCC);
		}
	}
};

PrintADC printADC;

MAIN {
	// Disable all peripherals and enable just what we need
	power_all_disable();
	power_timer0_enable();
	power_usart0_enable();
	power_adc_enable();
	set_sleep_mode(SLEEP_MODE_IDLE);

	// Enable interrupts
	sei();

	// Configure the tick timer to tick every 1ms
	tickTimer.setTimes(1000, 0);
	tickTimer.setListener1(rtc);

	// Start ticking the RTC through its associated timer
	tickTimer.enable();

	adc.registerListener(ADCChannel::CHANNEL_0, printADC);
	adc.registerListener(ADCChannel::CHANNEL_1, printADC);
	adc.registerListener(ADCChannel::CHANNEL_2, printADC);

	serial.write_P(PSTR("Setting initial alarm\r\n"));
	// Insert the initial alarm
	rtc.addAlarm(oncePerSecond, 1, 0,	// When (seconds, milliseconds)
			1, 0);  // Repeat (seconds, milliseconds)

	serial.write_P(PSTR("Added initial alarm\r\n"));

	// main loop
	for (;;) {
		rtc.handleEvents();
		adc.handleEvents();
		sleep_mode();
	}

	return 0;
}
