/*
 * Copyright (c) 2012, Make, Hack, Void Inc
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

#ifndef MHV_VUSBTYPIST_H_
#define MHV_VUSBTYPIST_H_

#include <mhvlib/VusbKeyboard.h>
#include <mhvlib/Device_TX.h>
#include <avr/pgmspace.h>

extern "C" {
	#include <vusb/usbdrv.h>
}

using namespace mhvlib;
namespace mhvlib_gpl {

/**
 * A USB keyboard emulator that will type what you print to it
 * @tparam	txBuffers	the number of output buffers
 */
template<uint8_t txBuffers>
class VusbTypist : public VusbKeyboard, public Device_TXImplementation<txBuffers> {
protected:
	bool _isTyping;

	/**
	 * Start transmitting a new string
	 * (does nothing, alarm will immediately pick up the next character)
	 */
	CONST void runTxBuffers() {}

	/**
	 * Type a single character on the keyboard
	 * @param	c	the character to type
	 */
	void typeChar(char c) {
		uint8_t modifiers = 0;
		VUSB_KEYBOARD_KEY key = MHV_KEY_BACKSPACE;

		if (c >= 'a' && c <= 'z') {
			key = vusb_keyboard_key_add(MHV_KEY_A, c - 'a');
		} else if (c >= 'A' && c <= 'Z') {
			key = vusb_keyboard_key_add(MHV_KEY_A, c - 'A');
			modifiers = MHV_MOD_SHIFT_LEFT;
		} else if (c >= '1' && c <= '9') {
			key = vusb_keyboard_key_add(MHV_KEY_1, c - '1');
		} else {
	// Shifted characters
			switch (c) {
			case '!':
				key = MHV_KEY_1;
				break;
			case '@':
				key = MHV_KEY_2;
				break;
			case '#':
				key = MHV_KEY_3;
				break;
			case '$':
				key = MHV_KEY_4;
				break;
			case '%':
				key = MHV_KEY_5;
				break;
			case '^':
				key = MHV_KEY_6;
				break;
			case '&':
				key = MHV_KEY_7;
				break;
			case '(':
				key = MHV_KEY_9;
				break;
			case ')':
				key = MHV_KEY_0;
				break;
			case '{':
				key = MHV_KEY_L_SQUARE;
				break;
			case '}':
				key = MHV_KEY_R_SQUARE;
				break;
			case '|':
				key = MHV_KEY_BACKSLASH;
				break;
			case ':':
				key = MHV_KEY_SEMICOLON;
				break;
			case '"':
				key = MHV_KEY_QUOTE;
				break;
			case '<':
				key = MHV_KEY_COMMA;
				break;
			case '>':
				key = MHV_KEY_FULLSTOP;
				break;
			case '?':
				key = MHV_KEY_SLASH;
				break;
			case '_':
				key = MHV_KEY_MINUS;
				break;
			case '+':
				key = MHV_KEY_EQUALS;
				break;
			case '~':
				key = MHV_KEY_GRAVE_ACCENT;
				break;
			}

			if (MHV_KEY_BACKSPACE != key) {
				modifiers = MHV_MOD_SHIFT_LEFT;
			}
		}

		if (MHV_KEY_BACKSPACE == key) {
			switch (c) {
			case '0':
				key = MHV_KEY_0;
				break;
			case '\n':
				key = MHV_KEY_ENTER;
				break;
			case '\t':
				key = MHV_KEY_TAB;
				break;
			case '*':
				key = MHV_KEYPAD_ASTERISK;
				break;
			case '[':
				key = MHV_KEY_L_SQUARE;
				break;
			case ']':
				key = MHV_KEY_R_SQUARE;
				break;
			case '\\':
				key = MHV_KEY_BACKSLASH;
				break;
			case ';':
				key = MHV_KEY_SEMICOLON;
				break;
			case '\'':
				key = MHV_KEY_QUOTE;
				break;
			case ',':
				key = MHV_KEY_COMMA;
				break;
			case '.':
				key = MHV_KEY_FULLSTOP;
				break;
			case '/':
				key = MHV_KEY_SLASH;
				break;
			case '-':
				key = MHV_KEY_MINUS;
				break;
			case '=':
				key = MHV_KEY_EQUALS;
				break;
			case '`':
				key = MHV_KEY_GRAVE_ACCENT;
				break;
			}
		}

		if (MHV_KEY_BACKSPACE == key) {
			key = MHV_KEY_SPACE;
		}

		keyDown(key, modifiers);
	}


public:
	/**
	 * Emulate a USB keyboard using V-USB
	 *   This class can also be passed strings, which it will type out on the keyboard
	 *  @param	rtc			an RTC to trigger events from
	 */
	VusbTypist(RTC &rtc) :
			VusbKeyboard(rtc),
			_isTyping(false) {}

	/**
	 * Periodically called to maintain USB comms
	 */
	void alarm() {
		VusbKeyboard::alarm();
		if (usbInterruptIsReady()) {
			int c = Device_TX::nextCharacter();

			if (-1 == c) {
				if (_isTyping) {
					_isTyping = false;
					keysUp();
				}
				return;
			}

			_isTyping = true;
			typeChar(c);
		}
	}
};

}
#endif /* MHV_VUSBTYPIST_H_ */
