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

extern "C" {
	#include <vusb/usbdrv.h>
}

#include <mhvlib/VusbKeyboard.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define MHV_OSCCAL_EEPROM_ADDRESS	1022

/* We use a simplifed keyboard report descriptor which does not support the
 * boot protocol. We don't allow setting status LEDs and but we do allow
 * simultaneous key presses.
 * The report descriptor has been created with usb.org's "HID Descriptor Tool"
 * which can be downloaded from http://www.usb.org/developers/hidpage/.
 * Redundant entries (such as LOGICAL_MINIMUM and USAGE_PAGE) have been omitted
 * for the second INPUT item.
 */
PROGMEM const char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = { /* USB report descriptor */
0x05, 0x01,	// USAGE_PAGE (Generic Desktop)
0x09, 0x06,	// USAGE (Keyboard)
0xa1, 0x01,	// COLLECTION (Application)
0x05, 0x07,	//   USAGE_PAGE (Keyboard)
0x19, 0xe0,	//   USAGE_MINIMUM (Keyboard LeftControl)
0x29, 0xe7,	//   USAGE_MAXIMUM (Keyboard Right GUI)
0x15, 0x00,	//   LOGICAL_MINIMUM (0)
0x25, 0x01,	//   LOGICAL_MAXIMUM (1)
0x75, 0x01,	//   REPORT_SIZE (1)
0x95, 0x08,	//   REPORT_COUNT (8)
0x81, 0x02,	//   INPUT (Data,Var,Abs)
0x95, 0x01,	//   REPORT_COUNT (1)
0x75, 0x08,	//   REPORT_SIZE (8)
0x25, 0x65,	//   LOGICAL_MAXIMUM (101)
0x19, 0x00,	//   USAGE_MINIMUM (Reserved (no event indicated))
0x29, 0x65,	//   USAGE_MAXIMUM (Keyboard Application)
0x81, 0x00,	//   INPUT (Data,Ary,Abs)
0xc0       	// END_COLLECTION
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

static unsigned char	mhv_vusbReportBuffer[2];	/* buffer for HID reports */
static unsigned char	mhv_vusbIdleRate = 1;		/* in 4 ms units */

unsigned char usbFunctionSetup(uchar data[8]) {
	usbRequest_t *rq = (usbRequest_t *) ((void *) data);

	usbMsgPtr = mhv_vusbReportBuffer;
	if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) { /* class request type */
		if (rq->bRequest == USBRQ_HID_GET_REPORT) { /* wValue: ReportType (highbyte), ReportID (lowbyte) */
			/* we only have one report type, so don't look at wValue */
			return sizeof(mhv_vusbReportBuffer);
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

using namespace mhvlib;

namespace mhvlib_gpl {

/**
 * Emulate a USB keyboard using V-USB
 *   Uses pins D4/D2 for ATmega (can be changed in VUSBKeyboard/usbconfig.h)
 *   Uses pins B0/B2 for ATtiny25/45/85
 * @param	rtc	an RTC to schedule jobs on
 */
VusbKeyboard::VusbKeyboard(RTC &rtc) :
		_rtc(rtc) {
#if USB_CFG_HAVE_MEASURE_FRAME_LENGTH
	uchar calibrationValue;

	calibrationValue = eeprom_read_byte(MHV_OSCCAL_EEPROM_ADDRESS); /* calibration value from last time */
	if (calibrationValue != 0xff) {
		OSCCAL = calibrationValue;
	}
#endif

	setInput(MHV_MAKE_PIN(USB_CFG_IOPORTNAME, USB_CFG_DPLUS_BIT));
	setInput(MHV_MAKE_PIN(USB_CFG_IOPORTNAME, USB_CFG_DMINUS_BIT));

	usbDeviceDisconnect();
	for(uint8_t i=0;i<20;i++){  /* 300 ms disconnect */
		_delay_ms(15);
	}
	usbDeviceConnect();

	usbInit();

	_rtc.addAlarm(this, 0, 5, 0, 5);
}

/**
 * Send a single keystroke
 * @param	key			the key to send
 * @param	modifiers	the key modifiers
 *
 * @return false if the keyStroke was not sent
 */
void VusbKeyboard::keyStroke(VUSB_KEYBOARD_KEY key, uint8_t modifiers) {
	keyDown(key, modifiers);
	keysUp(0);
}

/**
 * Send a single keystroke
 * @param	key		the key to send
 *
 * @return false if the keyStroke was not sent
 */
void VusbKeyboard::keyStroke(VUSB_KEYBOARD_KEY key) {
	return keyStroke(key, 0);
}

/**
 * Press a key
 * @param	key			the key to send
 * @param	modifiers	the key modifiers
 */
void VusbKeyboard::keyDown(VUSB_KEYBOARD_KEY key, uint8_t modifiers) {
	while (!usbInterruptIsReady()) {}

	mhv_vusbReportBuffer[0] = modifiers;
	mhv_vusbReportBuffer[1] = key;

	usbSetInterrupt(mhv_vusbReportBuffer, sizeof(mhv_vusbReportBuffer));
}

/**
 * Release all keys
 * @param	modifiers	the key modifiers still held
 */
void VusbKeyboard::keysUp(uint8_t modifiers) {
	while (!usbInterruptIsReady()) {}

	mhv_vusbReportBuffer[0] = modifiers;
	mhv_vusbReportBuffer[1] = 0;
	usbSetInterrupt(mhv_vusbReportBuffer, sizeof(mhv_vusbReportBuffer));
}

/**
 * Release all keys
 */
void VusbKeyboard::keysUp() {
	keysUp(0);
}

/**
 * Periodically called to maintain USB comms
 */
void VusbKeyboard::alarm() {
	usbPoll();
}

}
