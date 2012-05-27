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

#ifndef MHV_VUSBKEYBOARD_H_
#define MHV_VUSBKEYBOARD_H_

#include <MHV_io.h>
#include <MHV_RTC.h>

#define MHV_MOD_CONTROL_LEFT    (1<<0)
#define MHV_MOD_SHIFT_LEFT      (1<<1)
#define MHV_MOD_ALT_LEFT        (1<<2)
#define MHV_MOD_GUI_LEFT        (1<<3)
#define MHV_MOD_CONTROL_RIGHT   (1<<4)
#define MHV_MOD_SHIFT_RIGHT     (1<<5)
#define MHV_MOD_ALT_RIGHT       (1<<6)
#define MHV_MOD_GUI_RIGHT       (1<<7)

enum mhv_vusb_keyboard_key {
	MHV_KEY_A					= 4,
	MHV_KEY_B					= 5,
	MHV_KEY_C					= 6,
	MHV_KEY_D					= 7,
	MHV_KEY_E					= 8,
	MHV_KEY_F					= 9,
	MHV_KEY_G					= 10,
	MHV_KEY_H					= 11,
	MHV_KEY_I					= 12,
	MHV_KEY_J					= 13,
	MHV_KEY_K					= 14,
	MHV_KEY_L					= 15,
	MHV_KEY_M					= 16,
	MHV_KEY_N					= 17,
	MHV_KEY_O					= 18,
	MHV_KEY_P					= 19,
	MHV_KEY_Q					= 20,
	MHV_KEY_R					= 21,
	MHV_KEY_S					= 22,
	MHV_KEY_T					= 23,
	MHV_KEY_U					= 24,
	MHV_KEY_V					= 25,
	MHV_KEY_W					= 26,
	MHV_KEY_X					= 27,
	MHV_KEY_Y					= 28,
	MHV_KEY_Z					= 29,
	MHV_KEY_1					= 30,
	MHV_KEY_2					= 31,
	MHV_KEY_3					= 32,
	MHV_KEY_4					= 33,
	MHV_KEY_5					= 34,
	MHV_KEY_6					= 35,
	MHV_KEY_7					= 36,
	MHV_KEY_8					= 37,
	MHV_KEY_9					= 38,
	MHV_KEY_0					= 39,
	MHV_KEY_ENTER   			= 40,
	MHV_KEY_ESCAPE				= 41,
	MHV_KEY_BACKSPACE			= 42,
	MHV_KEY_TAB				= 43,
	MHV_KEY_SPACE   			= 44,
	MHV_KEY_MINUS				= 45,
	MHV_KEY_EQUALS				= 46,
	MHV_KEY_L_SQUARE			= 47,
	MHV_KEY_R_SQUARE			= 48,
	MHV_KEY_BACKSLASH			= 49,
	MHV_KEY_NON_US_HASH		= 50,
	MHV_KEY_SEMICOLON			= 51,
	MHV_KEY_QUOTE				= 52,
	MHV_KEY_GRAVE_ACCENT		= 53,
	MHV_KEY_COMMA				= 54,
	MHV_KEY_FULLSTOP			= 55,
	MHV_KEY_SLASH				= 56,
	MHV_KEY_CAPSLOCK			= 57,
	MHV_KEY_F1      			= 58,
	MHV_KEY_F2      			= 59,
	MHV_KEY_F3      			= 60,
	MHV_KEY_F4      			= 61,
	MHV_KEY_F5      			= 62,
	MHV_KEY_F6      			= 63,
	MHV_KEY_F7      			= 64,
	MHV_KEY_F8     			= 65,
	MHV_KEY_F9      			= 66,
	MHV_KEY_F10     			= 67,
	MHV_KEY_F11				= 68,
	MHV_KEY_F12				= 69,
	MHV_KEY_PRINTSCREEN		= 70,
	MHV_KEY_SCROLL_LOCK		= 71,
	MHV_KEY_PAUSE				= 72,
	MHV_KEY_INSERT				= 73,
	MHV_KEY_HOME				= 74,
	MHV_KEY_PAGE_UP			= 75,
	MHV_KEY_DELETE				= 76,
	MHV_KEY_END				= 77,
	MHV_KEY_PAGE_DOWN			= 78,
	MHV_KEY_ARROW_RIGHT		= 79,
	MHV_KEY_ARROW_LEFT			= 80,
	MHV_KEY_ARROW_DOWN			= 81,
	MHV_KEY_ARROW_UP			= 82,
	MHV_KEY_NUM_LOCK			= 83,
	MHV_KEYPAD_SLASH			= 84,
	MHV_KEYPAD_ASTERISK		= 85,
	MHV_KEYPAD_MINUS			= 86,
	MHV_KEYPAD_PLUS			= 87,
	MHV_KEYPAD_ENTER			= 88,
	MHV_KEYPAD_1				= 89,
	MHV_KEYPAD_2				= 90,
	MHV_KEYPAD_3				= 91,
	MHV_KEYPAD_4				= 92,
	MHV_KEYPAD_5				= 93,
	MHV_KEYPAD_6				= 94,
	MHV_KEYPAD_7				= 95,
	MHV_KEYPAD_8				= 96,
	MHV_KEYPAD_9				= 97,
	MHV_KEYPAD_0				= 98,
	MHV_KEYPAD_FULLSTOP		= 99,
	MHV_KEY_NON_US_BACKSLASH	= 100,
	MHV_KEY_APPLICATION		= 101,
	MHV_KEYPAD_POWER			= 102,
	MHV_KEYPAD_EQUALS			= 103,
	MHV_KEY_F13				= 104,
	MHV_KEY_F14				= 105,
	MHV_KEY_F15				= 106,
	MHV_KEY_F16				= 107,
	MHV_KEY_F17				= 108,
	MHV_KEY_F18				= 109,
	MHV_KEY_F19				= 110,
	MHV_KEY_F20				= 111,
	MHV_KEY_F21				= 112,
	MHV_KEY_F22				= 113,
	MHV_KEY_F23				= 114,
	MHV_KEY_F24				= 115,
	MHV_KEY_EXECUTE			= 116,
	MHV_KEY_HELP				= 117,
	MHV_KEY_MENU				= 118,
	MHV_KEY_SELECT				= 119,
	MHV_KEY_STOP				= 120,
	MHV_KEY_AGAIN				= 121,
	MHV_KEY_UNDO				= 122,
	MHV_KEY_CUT				= 123,
	MHV_KEY_COPY				= 124,
	MHV_KEY_PASTE				= 125,
	MHV_KEY_FIND				= 126,
	MHV_KEY_MUTE				= 127,
	MHV_KEY_VOLUME_UP			= 128,
	MHV_KEY_VOLUME_DOWN		= 129,
	MHV_KEY_LOCKING_CAPS_LOCK	= 130,
	MHV_KEY_LOCKING_NUM_LOCK	= 131,
	MHV_KEY_LOCKING_SCROLL_LOCK = 132,
	MHV_KEYPAD_COMMA			= 133,
	MHV_KEYPAD_EQUAL			= 134,
	MHV_KEY_CONTROL_LEFT		= 224,
	MHV_KEY_SHIFT_LEFT			= 225,
	MHV_KEY_ALT_LEFT			= 226,
	MHV_KEY_GUI_LEFT			= 227,
	MHV_KEY_CONTROL_RIGHT		= 228,
	MHV_KEY_SHIFT_RIGHT		= 229,
	MHV_KEY_ALT_RIGHT			= 230,
	MHV_KEY_GUI_RIGHT			= 231
};

typedef enum mhv_vusb_keyboard_key MHV_VUSB_KEYBOARD_KEY;

INLINE MHV_VUSB_KEYBOARD_KEY mhv_vusb_keyboard_key_add(MHV_VUSB_KEYBOARD_KEY key, char c) {
	const uint8_t myKey = key;
	return static_cast<MHV_VUSB_KEYBOARD_KEY>(myKey + c);
}

class MHV_VusbKeyboard : public MHV_TimerListener {
protected:
	MHV_RTC		&_rtc;

public:
	MHV_VusbKeyboard(MHV_RTC &rtc);
	void keyStroke(MHV_VUSB_KEYBOARD_KEY key);
	void keyStroke(MHV_VUSB_KEYBOARD_KEY key, uint8_t modifiers);
	void keyDown(MHV_VUSB_KEYBOARD_KEY key, uint8_t modifiers);
	void keysUp(uint8_t modifiers);
	void keysUp();
	void alarm();
};

#endif /* MHV_VUSBKEYBOARD_H_ */
