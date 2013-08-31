/*
 * Copyright (c) 2012, Make, Hack, Void Inc
 * All rights reserved.
 *
 *  License: GNU GPL v2 (see mhvlib-Vusb-CONSOLE/vusb/License.txt)
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

#ifndef MHV_VUSBCONSOLE_H_
#define MHV_VUSBCONSOLE_H_

extern "C" {
	#include <vusb/usbdrv.h>
}

#define MAX_TX_SIZE		8

#include <mhvlib/Device_TX.h>
#include <mhvlib/RTC.h>
#include <util/delay.h>

using namespace mhvlib;

namespace mhvlib_gpl {

template<uint8_t txCount>
class VusbConsole : public TimerListener, public Device_TXImplementation<txCount> {
protected:
	RTC		&_rtc;

public:
	/**
	 * Provide a V-USB console using V-USB
	 *   Uses pins D4/D2 for ATmega (can be changed in VUSBKeyboard/usbconfig.h)
	 *   Uses pins B0/B2 for ATtiny25/45/85
	 *
	 * To view the output of the console on your computer, use HID Listen, available from:
	 * http://www.pjrc.com/teensy/hid_listen.html
	 *
	 * @param	rtc			an RTC to schedule jobs on
	 */
	VusbConsole(RTC &rtc) :
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
	 * Periodically called to maintain USB comms
	 */
	void alarm(UNUSED AlarmSource source) {
		usbPoll();

		if (usbInterruptIsReady()) {
			int c;
			unsigned char buf[MAX_TX_SIZE];
			uint8_t bufSize;

			for (bufSize = 0; bufSize < MAX_TX_SIZE; bufSize++) {
				c = Device_TX::nextCharacter();
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
	void runTxBuffers() {
	}
};

}
#endif /* MHV_VUSBCONSOLE_H_ */
