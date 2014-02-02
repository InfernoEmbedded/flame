/*
 * Copyright (c) 2014, Inferno Embedded
 * All rights reserved.
 *
 *  License: GNU GPL v2 (see flame-Vusb-Keyboard/vusb/License.txt)
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

#ifndef FLAME_VUSBTYPIST_H_
#define FLAME_VUSBTYPIST_H_

#include <flame/VusbKeyboard.h>
#include <flame/Device_TX.h>
#include <avr/pgmspace.h>

extern "C" {
	#include <vusb/usbdrv.h>
}

using namespace flame;
namespace flame_gpl {

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
		VUSB_KEYBOARD_KEY key = FLAME_KEY_BACKSPACE;

		if (c >= 'a' && c <= 'z') {
			key = vusb_keyboard_key_add(FLAME_KEY_A, c - 'a');
		} else if (c >= 'A' && c <= 'Z') {
			key = vusb_keyboard_key_add(FLAME_KEY_A, c - 'A');
			modifiers = FLAME_MOD_SHIFT_LEFT;
		} else if (c >= '1' && c <= '9') {
			key = vusb_keyboard_key_add(FLAME_KEY_1, c - '1');
		} else {
	// Shifted characters
			switch (c) {
			case '!':
				key = FLAME_KEY_1;
				break;
			case '@':
				key = FLAME_KEY_2;
				break;
			case '#':
				key = FLAME_KEY_3;
				break;
			case '$':
				key = FLAME_KEY_4;
				break;
			case '%':
				key = FLAME_KEY_5;
				break;
			case '^':
				key = FLAME_KEY_6;
				break;
			case '&':
				key = FLAME_KEY_7;
				break;
			case '(':
				key = FLAME_KEY_9;
				break;
			case ')':
				key = FLAME_KEY_0;
				break;
			case '{':
				key = FLAME_KEY_L_SQUARE;
				break;
			case '}':
				key = FLAME_KEY_R_SQUARE;
				break;
			case '|':
				key = FLAME_KEY_BACKSLASH;
				break;
			case ':':
				key = FLAME_KEY_SEMICOLON;
				break;
			case '"':
				key = FLAME_KEY_QUOTE;
				break;
			case '<':
				key = FLAME_KEY_COMMA;
				break;
			case '>':
				key = FLAME_KEY_FULLSTOP;
				break;
			case '?':
				key = FLAME_KEY_SLASH;
				break;
			case '_':
				key = FLAME_KEY_MINUS;
				break;
			case '+':
				key = FLAME_KEY_EQUALS;
				break;
			case '~':
				key = FLAME_KEY_GRAVE_ACCENT;
				break;
			}

			if (FLAME_KEY_BACKSPACE != key) {
				modifiers = FLAME_MOD_SHIFT_LEFT;
			}
		}

		if (FLAME_KEY_BACKSPACE == key) {
			switch (c) {
			case '0':
				key = FLAME_KEY_0;
				break;
			case '\n':
				key = FLAME_KEY_ENTER;
				break;
			case '\t':
				key = FLAME_KEY_TAB;
				break;
			case '*':
				key = FLAME_KEYPAD_ASTERISK;
				break;
			case '[':
				key = FLAME_KEY_L_SQUARE;
				break;
			case ']':
				key = FLAME_KEY_R_SQUARE;
				break;
			case '\\':
				key = FLAME_KEY_BACKSLASH;
				break;
			case ';':
				key = FLAME_KEY_SEMICOLON;
				break;
			case '\'':
				key = FLAME_KEY_QUOTE;
				break;
			case ',':
				key = FLAME_KEY_COMMA;
				break;
			case '.':
				key = FLAME_KEY_FULLSTOP;
				break;
			case '/':
				key = FLAME_KEY_SLASH;
				break;
			case '-':
				key = FLAME_KEY_MINUS;
				break;
			case '=':
				key = FLAME_KEY_EQUALS;
				break;
			case '`':
				key = FLAME_KEY_GRAVE_ACCENT;
				break;
			}
		}

		if (FLAME_KEY_BACKSPACE == key) {
			key = FLAME_KEY_SPACE;
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
	void alarm(AlarmSource source) {
		VusbKeyboard::alarm(source);
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
#endif /* FLAME_VUSBTYPIST_H_ */
