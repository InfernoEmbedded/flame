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

#define MHV_KEY_A					4
#define MHV_KEY_B					5
#define MHV_KEY_C					6
#define MHV_KEY_D					7
#define MHV_KEY_E					8
#define MHV_KEY_F					9
#define MHV_KEY_G					10
#define MHV_KEY_H					11
#define MHV_KEY_I					12
#define MHV_KEY_J					13
#define MHV_KEY_K					14
#define MHV_KEY_L					15
#define MHV_KEY_M					16
#define MHV_KEY_N					17
#define MHV_KEY_O					18
#define MHV_KEY_P					19
#define MHV_KEY_Q					20
#define MHV_KEY_R					21
#define MHV_KEY_S					22
#define MHV_KEY_T					23
#define MHV_KEY_U					24
#define MHV_KEY_V					25
#define MHV_KEY_W					26
#define MHV_KEY_X					27
#define MHV_KEY_Y					28
#define MHV_KEY_Z					29
#define MHV_KEY_1					30
#define MHV_KEY_2					31
#define MHV_KEY_3					32
#define MHV_KEY_4					33
#define MHV_KEY_5					34
#define MHV_KEY_6					35
#define MHV_KEY_7					36
#define MHV_KEY_8					37
#define MHV_KEY_9					38
#define MHV_KEY_0					39
#define MHV_KEY_ENTER   			40
#define MHV_KEY_ESCAPE				41
#define MHV_KEY_BACKSPACE			42
#define MHV_KEY_TAB					43
#define MHV_KEY_SPACE   			44
#define MHV_KEY_MINUS				45
#define MHV_KEY_EQUALS				46
#define MHV_KEY_L_SQUARE			47
#define MHV_KEY_R_SQUARE			48
#define MHV_KEY_BACKSLASH			49
#define MHV_KEY_NON_US_HASH			50
#define MHV_KEY_SEMICOLON			51
#define MHV_KEY_QUOTE				52
#define MHV_KEY_GRAVE_ACCENT		53
#define MHV_KEY_COMMA				54
#define MHV_KEY_FULLSTOP			55
#define MHV_KEY_SLASH				56
#define MHV_KEY_CAPSLOCK			57
#define MHV_KEY_F1      			58
#define MHV_KEY_F2      			59
#define MHV_KEY_F3      			60
#define MHV_KEY_F4      			61
#define MHV_KEY_F5      			62
#define MHV_KEY_F6      			63
#define MHV_KEY_F7      			64
#define MHV_KEY_F8     				65
#define MHV_KEY_F9      			66
#define MHV_KEY_F10     			67
#define MHV_KEY_F11					68
#define MHV_KEY_F12					69
#define MHV_KEY_PRINTSCREEN			70
#define MHV_KEY_SCROLL_LOCK			71
#define MHV_KEY_PAUSE				72
#define MHV_KEY_INSERT				73
#define MHV_KEY_HOME				74
#define MHV_KEY_PAGE_UP				75
#define MHV_KEY_DELETE				76
#define MHV_KEY_END					77
#define MHV_KEY_PAGE_DOWN			78
#define MHV_KEY_ARROW_RIGHT			79
#define MHV_KEY_ARROW_LEFT			80
#define MHV_KEY_ARROW_DOWN			81
#define MHV_KEY_ARROW_UP			82
#define MHV_KEY_NUM_LOCK			83
#define MHV_KEYPAD_SLASH			84
#define MHV_KEYPAD_ASTERISK			85
#define MHV_KEYPAD_MINUS			86
#define MHV_KEYPAD_PLUS				87
#define MHV_KEYPAD_ENTER			88
#define MHV_KEYPAD_1				89
#define MHV_KEYPAD_2				90
#define MHV_KEYPAD_3				91
#define MHV_KEYPAD_4				92
#define MHV_KEYPAD_5				93
#define MHV_KEYPAD_6				94
#define MHV_KEYPAD_7				95
#define MHV_KEYPAD_8				96
#define MHV_KEYPAD_9				97
#define MHV_KEYPAD_0				98
#define MHV_KEYPAD_FULLSTOP			99
#define MHV_KEY_NON_US_BACKSLASH	100
#define MHV_KEY_APPLICATION			101
#define MHV_KEYPAD_POWER			102
#define MHV_KEYPAD_EQUALS			103
#define MHV_KEY_F13					104
#define MHV_KEY_F14					105
#define MHV_KEY_F15					106
#define MHV_KEY_F16					107
#define MHV_KEY_F17					108
#define MHV_KEY_F18					109
#define MHV_KEY_F19					110
#define MHV_KEY_F20					111
#define MHV_KEY_F21					112
#define MHV_KEY_F22					113
#define MHV_KEY_F23					114
#define MHV_KEY_F24					115
#define MHV_KEY_EXECUTE				116
#define MHV_KEY_HELP				117
#define MHV_KEY_MENU				118
#define MHV_KEY_SELECT				119
#define MHV_KEY_STOP				120
#define MHV_KEY_AGAIN				121
#define MHV_KEY_UNDO				122
#define MHV_KEY_CUT					123
#define MHV_KEY_COPY				124
#define MHV_KEY_PASTE				125
#define MHV_KEY_FIND				126
#define MHV_KEY_MUTE				127
#define MHV_KEY_VOLUME_UP			128
#define MHV_KEY_VOLUME_DOWN			129
#define MHV_KEY_LOCKING_CAPS_LOCK	130
#define MHV_KEY_LOCKING_NUM_LOCK	131
#define MHV_KEY_LOCKING_SCROLL_LOCK	132
#define MHV_KEYPAD_COMMA			133
#define MHV_KEYPAD_EQUAL			134
#define MHV_KEY_CONTROL_LEFT		224
#define MHV_KEY_SHIFT_LEFT			225
#define MHV_KEY_ALT_LEFT			226
#define MHV_KEY_GUI_LEFT			227
#define MHV_KEY_CONTROL_RIGHT		228
#define MHV_KEY_SHIFT_RIGHT			229
#define MHV_KEY_ALT_RIGHT			230
#define MHV_KEY_GUI_RIGHT			231


class MHV_VusbKeyboard : public MHV_AlarmListener {
protected:
	MHV_RTC		*_rtc;

public:
	MHV_VusbKeyboard(MHV_RTC *rtc);
	void keyStroke(uint8_t key);
	void keyStroke(uint8_t key, uint8_t modifiers);
	void keyDown(uint8_t key, uint8_t modifiers);
	void keysUp(uint8_t modifiers);
	void keysUp();
	void alarm(MHV_ALARM *alarm);
};

#endif /* MHV_VUSBKEYBOARD_H_ */
