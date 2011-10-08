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
#include <MHV_io.h>

// Bring in the USB Keyboard driver
#include <MHV_VusbKeyboard.h>

// Bring in the power management header
#include <avr/power.h>
#include <avr/sleep.h>

// Bring in the timer header
#include <MHV_Timer8.h>

// Program space header, saves RAM by storing constants in flash
#include <avr/pgmspace.h>

#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
#define ATTINY
// A timer we will use to tick the RTC
MHV_Timer8 tickTimer(MHV_TIMER8_0);
MHV_TIMER_ASSIGN_1INTERRUPT(tickTimer, MHV_TIMER0_INTERRUPTS);

#else
// A timer we will use to tick the RTC
MHV_Timer8 tickTimer(MHV_TIMER8_2);
MHV_TIMER_ASSIGN_1INTERRUPT(tickTimer, MHV_TIMER2_INTERRUPTS);
#endif


/* A buffer the RTC will use to store alarms - this determines how many alarms
 * can be registered simultaneously
 */
#define ALARM_COUNT	4
MHV_ALARM alarms[ALARM_COUNT];

#define TIMEZONE 600 // UTC+10

// The RTC object we will use
MHV_RTC rtc(&tickTimer, alarms, ALARM_COUNT, TIMEZONE);

// A timer trigger that will tick the RTC
void rtcTrigger(void *data) {
	rtc.tick1ms();
}

// The USB Keyboard driver
MHV_VusbKeyboard keyboard(&rtc);


class Typist : public MHV_AlarmListener {
	uint8_t				_state;
	MHV_RTC				*_rtc;
	MHV_VusbKeyboard	*_keyboard;

public:
	Typist(MHV_RTC *rtc, MHV_VusbKeyboard *keyboard);
	void alarm(MHV_ALARM *alarm);
};

// Time between keystrokes (ms)
#define TIME_BETWEEN_KEYSTROKES		200

/**
 * Class to write characters on the keyboard
 * @param	rtc			the rtc to trigger events from
 * @param	keyboard	the keyboard to type on
 */
Typist::Typist(MHV_RTC *rtc, MHV_VusbKeyboard *keyboard) {
	_rtc = rtc;
	_keyboard = keyboard;

	_state = 0;

	MHV_ALARM newAlarm;

// Start writing 10 seconds after boot
	rtc->current(&(newAlarm.when));
	mhv_timestampIncrement(&(newAlarm.when), 10, 0);
	newAlarm.repeat.milliseconds = TIME_BETWEEN_KEYSTROKES;
	newAlarm.repeat.timestamp = 0;
	newAlarm.listener = this;
	rtc->addAlarm(&newAlarm);
}

PROGMEM uint8_t text[] = {
		MHV_KEY_G, 		MHV_MOD_SHIFT_LEFT,
		MHV_KEY_R,		0,
		MHV_KEY_E,		0,
		MHV_KEY_E,		0,
		MHV_KEY_T,		0,
		MHV_KEY_I,		0,
		MHV_KEY_N,		0,
		MHV_KEY_G,		0,
		MHV_KEY_S,		0,
		MHV_KEY_COMMA,	0,
		MHV_KEY_SPACE,	0,
		MHV_KEY_P,		0,
		MHV_KEY_R,		0,
		MHV_KEY_O,		0,
		MHV_KEY_G,		0,
		MHV_KEY_R,		0,
		MHV_KEY_A,		0,
		MHV_KEY_M,		0,
		MHV_KEY_1,		MHV_MOD_SHIFT_LEFT
};

void Typist::alarm(MHV_ALARM *alarm) {
	if (_state >= sizeof(text)) {
		// All done
			_rtc->removeAlarm(this);
			return;
	}

	uint8_t keyAndModifier[2];

	*(uint16_t *)keyAndModifier = pgm_read_word(text + _state);
	_keyboard->keyStroke(keyAndModifier[0], keyAndModifier[1]);
	_state += 2;
}

// Create the typist
Typist typist(&rtc, &keyboard);

int main(void) {
	// Disable all peripherals and enable just what we need
	power_all_disable();
#ifdef ATTINY
	power_timer0_enable();
#define PRESCALER	MHV_TIMER_PRESCALER_5_64
#else
	power_timer2_enable();
#define PRESCALER	MHV_TIMER_PRESCALER_7_64
#endif

#ifdef ATTINY
// Timer will not wake the ATtiny from power save
	set_sleep_mode(SLEEP_MODE_IDLE);
#else
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);
#endif

	// Configure the tick timer to tick every 1ms (at 16MHz)
	tickTimer.setPeriods(PRESCALER, 249, 0);
	tickTimer.setTriggers(rtcTrigger, 0, 0, 0);
	tickTimer.enable();

	sei();

	for (;;) {
		/* All the interesting things happen in the events
		 * See the Typist::alarm method
		 */
		rtc.handleEvents();

		// Sleep until an interrupt occurs
		sleep_mode();
	}
}
