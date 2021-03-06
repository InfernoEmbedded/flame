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

/* Generate a 7V output using a boost circuit fed from VCC
 *
 * Uses Timer1 to generate a PWM signal to drive the transistor.
 * The transistor should be connected to OC1B, which is Arduino pin 12 on the Mega
 * and Arduino pin 10 on others
 *
 * See http://www.ladyada.net/library/diyboostcalc.html for the schematic & calculator
 */

#include <util/delay.h>



// Bring in the FLAME IO header
#include <flame/io.h>

// Bring in the FLAME timer header
#include <flame/Timer.h>

// Bring in the FLAME Voltage Regulator header
#include <flame/VoltageRegulator.h>

// Bring in the FLAME Serial header
#include <flame/HardwareSerial.h>

// Bring in the FLAME ADC header
#include <flame/AD.h>

// Bring in the power management header
#include <avr/power.h>

using namespace flame;


FLAME_HARDWARESERIAL_CREATE(serial, 0, 2, FLAME_USART0, 115200);

// We will be operating using Timer1
#define OUTPUT_PIN FLAME_PIN_TIMER_1_B
TimerImplementation<FLAME_TIMER16_1, TimerMode::PWM_FAST_16> vOutTimer;

#define TARGET_VOLTAGE 7000

// This is only accurate to ~ 5%, you may want to measure it (on the AREF pin) to increase accuracy
#define REFERENCE_VOLTAGE 1100

// The resistors in the resistor divider, the sum should not be greater than 10k
#define R1	7250.00
#define R2	800.00

// Which ADC channel to use
#define ADC_CHANNEL	ADCChannel::CHANNEL_0

VoltageRegulator<VoltageRegulatorModes::BOOST, TARGET_VOLTAGE, REFERENCE_VOLTAGE, ADCReference::REF1V1,
	FLAME_VREG_DIVIDER(R2/(R2+R1)), ADC_CHANNEL> regulator(vOutTimer);

MAIN {
	// Disable all peripherals and enable just what we need
	power_all_disable();
	power_usart0_enable();
	power_timer1_enable();

// Enable interrupts
	sei();

	ad_setPrescaler(ADCPrescaler::DIVIDE_BY_128);

// Set the pin the transistor is connected to to output
	setOutput(OUTPUT_PIN);

// Set the PWM prescaler to 1 (no prescaler)
	vOutTimer.setPrescaler(TimerPrescaler::PRESCALER_5_1);

/* Set the TOP value of the PWM timer - this defines the resolution &
 * frequency of the PWM output. With a 16MHz clock, this gives us a PWM
 * frequency of 4kHz
 */
	vOutTimer.setTop(4095);

/* Tell the pwmTimer to use output compare 2 for PWM output
 * It will turn off the output pin when the timer elapses, and turn it on
 * when it is reset
 */
	vOutTimer.connectOutput2(TimerConnect::CLEAR);

// Start the regulator
	regulator.enable();

// Update the voltage periodically
	for(;;) {
		regulator.alarm(AlarmSource::UNKNOWN);

/* No floating point in the default printf library from AVR LibC
 * (it can be enabled with "-Wl,-u,vfprintf -lprintf_flt -lm" at the cost of memory)
 * so we will print the current voltage in milliVolts
 */
		uint16_t milliVolts = (uint16_t)(1000 * regulator.getVoltage());
		FLAME_DEBUG(serial, "Current Voltage is %u millivolts", milliVolts);

		_delay_ms(100);
	}

	return 0;
}
