/*
 * Copyright (c) 2011, Make, Hack, Void Inc
 * All rights reserved.
 *
 *  License: GNU GPL v2 (see mhvlib-Vusb-Console/vusb/License.txt)
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

extern "C" {
	#include <vusb/usbdrv.h>
}

#include <MHV_VusbConsole.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define MHV_OSCCAL_EEPROM_ADDRESS	1022

#define MAX_TX_SIZE		8

// Taken from usb_debug_only.c:
PROGMEM const char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = { /* USB report descriptor */
		0x06, 0x31, 0xFF,		// Usage Page 0xFF31 (vendor defined)
		0x09, 0x74,				// Usage 0x74
		0xA1, 0x53,				// Collection 0x53
		0x75, 0x08,				// report size = 8 bits
		0x15, 0x00,				// logical minimum = 0
		0x26, 0xFF, 0x00,		// logical maximum = 255
		0x95, MAX_TX_SIZE,		// report count
		0x09, 0x75,				// usage
		0x81, 0x02,				// Input (array)
		0xC0					// end collection
};

/* This code has been borrowed from Sparkfun's AVR Stick firmware
 * See: http://www.sparkfun.com/products/9147
 */
#if USB_CFG_HAVE_MEASURE_FRAME_LENGTH
#include <avr/eeprom.h>
/* ------------------------------------------------------------------------- */
/* ------------------------ Oscillator Calibration ------------------------- */
/* ------------------------------------------------------------------------- */

/* Calibrate the RC oscillator to 8.25 MHz. The core clock of 16.5 MHz is
 * derived from the 66 MHz peripheral clock by dividing. Our timing reference
 * is the Start Of Frame signal (a single SE0 bit) available immediately after
 * a USB RESET. We first do a binary search for the OSCCAL value and then
 * optimize this value with a neighboorhod search.
 * This algorithm may also be used to calibrate the RC oscillator directly to
 * 12 MHz (no PLL involved, can therefore be used on almost ALL AVRs), but this
 * is wide outside the spec for the OSCCAL value and the required precision for
 * the 12 MHz clock! Use the RC oscillator calibrated to 12 MHz for
 * experimental purposes only!
 */
static void calibrateOscillator(void)
{
uchar       step = 128;
uchar       trialValue = 0, optimumValue;
int         x, optimumDev, targetValue = (unsigned)(1499 * (double)F_CPU / 10.5e6 + 0.5);

    /* do a binary search: */
    do{
        OSCCAL = trialValue + step;
        x = usbMeasureFrameLength();    /* proportional to current real frequency */
        if(x < targetValue)             /* frequency still too low */
            trialValue += step;
        step >>= 1;
    }while(step > 0);
    /* We have a precision of +/- 1 for optimum OSCCAL here */
    /* now do a neighborhood search for optimum value */
    optimumValue = trialValue;
    optimumDev = x; /* this is certainly far away from optimum */
    for(OSCCAL = trialValue - 1; OSCCAL <= trialValue + 1; OSCCAL++){
        x = usbMeasureFrameLength() - targetValue;
        if(x < 0)
            x = -x;
        if(x < optimumDev){
            optimumDev = x;
            optimumValue = OSCCAL;
        }
    }
    OSCCAL = optimumValue;
}
/*
Note: This calibration algorithm may try OSCCAL values of up to 192 even if
the optimum value is far below 192. It may therefore exceed the allowed clock
frequency of the CPU in low voltage designs!
You may replace this search algorithm with any other algorithm you like if
you have additional constraints such as a maximum CPU clock.
For version 5.x RC oscillators (those with a split range of 2x128 steps, e.g.
ATTiny25, ATTiny45, ATTiny85), it may be useful to search for the optimum in
both regions.
*/

void    usbEventResetReady(void)
{
    calibrateOscillator();
	uchar calibrationValue = eeprom_read_byte(MHV_OSCCAL_EEPROM_ADDRESS);
	if (calibrationValue != OSCCAL) {
		eeprom_write_byte(MHV_OSCCAL_EEPROM_ADDRESS, OSCCAL);
	}
}
#endif

static unsigned char	mhv_vusbIdleRate = 1;		/* in 4 ms units */

unsigned char usbFunctionSetup(uchar data[8]) {
	usbRequest_t *rq = (usbRequest_t *) ((void *) data);

	usbMsgPtr = NULL;
	if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) { /* class request type */
		if (rq->bRequest == USBRQ_HID_GET_REPORT) { /* wValue: ReportType (highbyte), ReportID (lowbyte) */
			return 0;
		} else if (rq->bRequest == USBRQ_HID_GET_IDLE) {
			usbMsgPtr = &mhv_vusbIdleRate;
			return 1;
		} else if (rq->bRequest == USBRQ_HID_SET_IDLE) {
			mhv_vusbIdleRate = rq->wValue.bytes[1];
		}
	} else {
		/* no vendor specific requests implemented */
	}
	return 0;
}


/**
 * Provide a V-USB console using V-USB
 *   Uses pins D4/D2 for ATmega (can be changed in VUSBKeyboard/usbconfig.h)
 *   Uses pins B0/B2 for ATtiny25/45/85
 *
 * To view the output of the console on your computer, use HID Listen, available from:
 * http://www.pjrc.com/teensy/hid_listen.html
 *
 * @param	txBuffer	a ringbuffer to store data in
 * @param	rtc			an RTC to schedule jobs on
 */
MHV_VusbConsole::MHV_VusbConsole(MHV_RingBuffer &txBuffer, MHV_RTC &rtc) :
				MHV_Device_TX(txBuffer),
				_rtc(rtc) {
#if USB_CFG_HAVE_MEASURE_FRAME_LENGTH
	uchar calibrationValue;

	calibrationValue = eeprom_read_byte(MHV_OSCCAL_EEPROM_ADDRESS); /* calibration value from last time */
	if (calibrationValue != 0xff) {
		OSCCAL = calibrationValue;
	}
#endif

	mhv_setInput(MHV_MAKE_PIN(USB_CFG_IOPORTNAME, USB_CFG_DPLUS_BIT));
	mhv_setInput(MHV_MAKE_PIN(USB_CFG_IOPORTNAME, USB_CFG_DMINUS_BIT));

	usbDeviceDisconnect();
    for(uint8_t i=0;i<20;i++){  /* 300 ms disconnect */
        _delay_ms(15);
    }
	usbDeviceConnect();

	usbInit();

	MHV_ALARM newAlarm;

	_rtc.current(newAlarm.when);
	mhv_timestampIncrement(newAlarm.when, 0, 5);
	newAlarm.repeat.milliseconds = 5;
	newAlarm.repeat.timestamp = 0;
	newAlarm.listener = this;

	_rtc.addAlarm(newAlarm);
}

/**
 * Periodically called to maintain USB comms
 * @param alarm	the alarm that triggered the call
 */
void MHV_VusbConsole::alarm(const MHV_ALARM &alarm) {
	usbPoll();

	if (usbInterruptIsReady()) {
		int c;
		unsigned char buf[MAX_TX_SIZE];
		uint8_t bufSize;

		for (bufSize = 0; bufSize < MAX_TX_SIZE; bufSize++) {
			c = nextCharacter();
			if (-1 == c) {
				break;
			}
			buf[bufSize] = (unsigned char)c;
		}

		if (bufSize) {
			for (; bufSize < MAX_TX_SIZE; bufSize++) {
				buf[bufSize] = '\0';
			}
			usbSetInterrupt(buf, bufSize);
		}
	}
}

/**
 * Start transmitting a new string
 * (does nothing, alarm will immediately pick up the next character)
 */
void MHV_VusbConsole::runTxBuffers() {
}

