/*
 * Copyright (c) 2011, Make, Hack, Void Inc
 * All rights reserved.
 *
 *  License: GNU GPL v2 (see mhvlib-Vusb-Keyboard/vusb/License.txt)
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

/* Demonstrates how to use the VUSB Keyboard driver
 * With GCC 4.5.2, Optimisation need to be disabled for this program to work (-O0)
 *
 */
#define MHVLIB_NEED_PURE_VIRTUAL

// Bring in the MHV IO header
#include <mhvlib/io.h>

// Bring in the USB Keyboard driver
#include <mhvlib/VusbTypist.h>

// Bring in the power management header
#include <avr/power.h>
#include <avr/sleep.h>

// Bring in the timer header
#include <mhvlib/Timer.h>

// Program space header, saves RAM by storing constants in flash
#include <avr/pgmspace.h>

using namespace mhvlib_bsd;
using namespace mhvlib_gpl;

#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
#define ATTINY
// A timer we will use to tick the RTC
TimerImplementation<MHV_TIMER8_0, TIMER_MODE::REPETITIVE> tickTimer;
MHV_TIMER_ASSIGN_1INTERRUPT(tickTimer, MHV_TIMER0_INTERRUPTS);

#else
// A timer we will use to tick the RTC
TimerImplementation<MHV_TIMER8_2, TIMER_MODE::REPETITIVE> tickTimer;
MHV_TIMER_ASSIGN_1INTERRUPT(tickTimer, MHV_TIMER2_INTERRUPTS);
#endif


/* A buffer the RTC will use to store alarms - this determines how many alarms
 * can be registered simultaneously
 */
#define ALARM_COUNT	4
// The RTC object we will use
RTCImplementation<ALARM_COUNT> rtc;

// The number of buffers we can print to
#define TX_BUFFERS	4

// The USB Keyboard driver
VusbTypist<TX_BUFFERS> typist(rtc);

class TypeString : public TimerListener {
public:
	void alarm();
};

void TypeString::alarm() {
	typist.write_P(PSTR("Greetings, program!"));
}

TypeString typeString;

MAIN {
	// Disable all peripherals and enable just what we need
	power_all_disable();
#ifdef ATTINY
	power_timer0_enable();
#define PRESCALER	TIMER_PRESCALER::PRESCALER_5_64
#else
	power_timer2_enable();
#define PRESCALER	TIMER_PRESCALER::PRESCALER_7_64
#endif

	set_sleep_mode(SLEEP_MODE_IDLE);

	// Configure the tick timer to tick every 1ms (at 16MHz)
	tickTimer.setPeriods(PRESCALER, 249, 0);
	tickTimer.setListener1(rtc);
	tickTimer.enable();

	sei();

	// Set up the string to be typed after 10 seconds
	rtc.addAlarm(typeString, 10, 0);

	for (;;) {
		/* All the interesting things happen in the events
		 * See the Typist::alarm method
		 */
		rtc.handleEvents();

		// Sleep until an interrupt occurs
		sleep_mode();
	}

	return 0;
}
