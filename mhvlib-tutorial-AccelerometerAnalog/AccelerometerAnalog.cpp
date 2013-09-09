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

/* Read an  analog accelerometer (such as the ADXL 335) connected as follows:
 * Description		Arduino (ATMega328)		Raw Port/Pin
 * VCC				Analog 5				C5
 * X				Analog 4				C4
 * Y				Analog 3				C3
 * Z				Analog 2				C2
 * Ground			Analog 1				C1
 */

// Bring in the MHV IO header
#include <mhvlib/io.h>

// Bring in the MHV timer and RTC headers
#include <mhvlib/Timer.h>
#include <mhvlib/RTC.h>

// Bring in the Analog ADC driver
#include <mhvlib/AccelerometerAnalog.h>

// Bring in the serial port header
#include <mhvlib/HardwareSerial.h>

// Bring in a calibrator for the accelerometer
#include <mhvlib/3AxisCalibration/BestSphereGaussNewtonCalibrator.h>

// Bring in the AVR interrupt header (needed for cli)
#include <avr/interrupt.h>
//
// Bring in the power management header
#include <avr/power.h>
#include <avr/sleep.h>

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
#define RX_BUFFER_SIZE	2
// The number of elements we want to be able to store to send asynchronously
#define TX_ELEMENTS_COUNT 6
MHV_HARDWARESERIAL_CREATE(serial, RX_BUFFER_SIZE, TX_ELEMENTS_COUNT, MHV_USART0, 115200);


// Create the EEPROM accessor for calibration data
MHV_EEPROM_CREATE(eeprom);

// One channel per axis
#define MAX_ADC_CHANNELS	3
// The ADC manager
MHV_ADC_CREATE(adc, MAX_ADC_CHANNELS, ADCPrescaler::DIVIDE_BY_128);

// Create a new accelerometer
AccelerometerAnalog<ADCChannel::CHANNEL_4, ADCChannel::CHANNEL_3,
	ADCChannel::CHANNEL_2, ADCReference::AVCC> accelerometer(adc);

class AccelerometerListener : public TripleAxisSensorListener {
	/**
	 * Called when a sample is ready
	 * @param sensor	the sensor whose sample is ready
	 */
	void sampleIsReady(TripleAxisSensor &sensor) {
		TRIPLEAXISSENSOR_READING value;
		sensor.getValue(&value);

		serial.printf(PSTR("Value: x: %f  y: %f  z: %f  magnitude: %f\r\n"),
				value.axis.x, value.axis.y, value.axis.z, sensor.magnitude());
	}

	/**
	 * Called when an acceleration limit is reached
	 * @param sensor	the sensor whose limit was reached
	 * @param which			which limit was reached
	 */
	void limitReached(TripleAxisSensor &sensor, TripleAxisSensorChannel which) {
		TRIPLEAXISSENSOR_READING value;
		sensor.getValue(&value);

		const char *channel = "";
		float val = 0;

		switch (which) {
		case TripleAxisSensorChannel::X:
			channel = "X";
			val = value.axis.x;
			break;

		case TripleAxisSensorChannel::Y:
			channel = "Y";
			val = value.axis.y;
			break;

		case TripleAxisSensorChannel::Z:
			channel = "Z";
			val = value.axis.z;
			break;

		case TripleAxisSensorChannel::MAGNITUDE:
			channel = "Magnitude";
			val = sensor.magnitude();
			break;
		}

		serial.printf(PSTR("%s limit reached: %f\r\n"), channel, val);
	}
};

AccelerometerListener listener;

MAIN {
	// Disable all peripherals and enable just what we need
	power_all_disable();
	power_timer0_enable();
	power_usart0_enable();
	power_adc_enable();
	set_sleep_mode(SLEEP_MODE_IDLE);

	// Enable interrupts
	sei();

	// Establish power lines for the accelerometer
	setOutput(MHV_PIN_C5);
	setOutput(MHV_PIN_C1);
	pinOff(MHV_PIN_C1);
	pinOn(MHV_PIN_C5);

	setOutput(MHV_PIN_B5);
	pinOff(MHV_PIN_B5);

	// Configure the tick timer to tick every 1ms
	tickTimer.setTimes(1000UL, 0UL);
	tickTimer.setListener1(rtc);

	// Start ticking the RTC through its associated timer
	tickTimer.enable();

	if (accelerometer.loadCalibration(eeprom)) {
// No calibration data found, better calibrate the sensor
		serial.write_P(PSTR("Calibrating accelerometer\r\n"));
		BestSphereGaussNewtonCalibrator calibrator(accelerometer, serial);
		calibrator.calibrate();

/* Keep processing events...
 * We do this here so we can restrict the memory usage of the Calibrator to this block only
 */
		while (!accelerometer.isCalibrated()) {
			adc.handleEvents();
			accelerometer.handleEvents();
			sleep_mode();
		}
		accelerometer.saveCalibration(eeprom);
		serial.write_P(PSTR("Accelerometer is calibrated\r\n"));
	} else {
		serial.write_P(PSTR("Using saved calibration data\r\n"));
	}

	TRIPLEAXISSENSOR_OFFSETS offsets;
	TRIPLEAXISSENSOR_SCALING scales;
	accelerometer.getParameters(&offsets, &scales);
	serial.printf(PSTR("Offsets: x: %d\ty: %d\tz: %d\r\n"), offsets.axis.x,  offsets.axis.y,  offsets.axis.z);
	serial.printf(PSTR("Scaling(x0.01): x: %d\ty: %d\tz: %d\r\n"), 100 * scales.axis.x, 100 * scales.axis.y, 100 * scales.axis.z);

	serial.write_P(PSTR("Grabbing accelerometer data at 1Hz\r\n"));
	accelerometer.registerListener(listener);

	accelerometer.setLimit(TripleAxisSensorChannel::X, 1.5f);
	accelerometer.setLimit(TripleAxisSensorChannel::Y, 1.5f);
	accelerometer.setLimit(TripleAxisSensorChannel::Z, 1.5f);
	accelerometer.setLimit(TripleAxisSensorChannel::MAGNITUDE, 2.0f);


	// Insert the initial alarm, repeat 10 times per second
	rtc.addAlarm(accelerometer, 1, 0,	// When (seconds, milliseconds)
			1, 0);  // Repeat (seconds, milliseconds)

	// main loop
	for (;;) {
		adc.handleEvents();
		rtc.handleEvents();
		accelerometer.handleEvents();
		sleep_mode();
	}

	return 0;
}
