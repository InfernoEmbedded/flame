/* Copyright (c) 2011, Make, Hack, Void Inc
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of the Make, Hack, Void nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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

#ifndef MHV_FONT_SANSSERIF_8X10_H
#define MHV_FONT_SANSSERIF_8X10_H

#include <mhvlib/Font.h>

#include <inttypes.h>
#include <avr/pgmspace.h>

namespace mhvlib {

// Font widths
static const uint8_t fontSansSerif8x10Widths[] PROGMEM = {
		0x01, 0x01, 0x01, 0x05, 0x05, 0x06, 0x05, 0x01, 0x02, 0x02,
		0x01, 0x03, 0x02, 0x02, 0x01, 0x03, 0x04, 0x02, 0x04, 0x04,
		0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x01, 0x02, 0x04, 0x04,
		0x04, 0x05, 0x08, 0x05, 0x04, 0x05, 0x04, 0x04, 0x04, 0x06,
		0x04, 0x01, 0x03, 0x04, 0x03, 0x05, 0x04, 0x06, 0x04, 0x06,
		0x04, 0x05, 0x05, 0x04, 0x05, 0x07, 0x05, 0x05, 0x05, 0x02,
		0x03, 0x02, 0x03, 0x04, 0x01, 0x04, 0x03, 0x04, 0x04, 0x04,
		0x02, 0x04, 0x03, 0x01, 0x02, 0x03, 0x01, 0x05, 0x03, 0x04,
		0x03, 0x03, 0x02, 0x04, 0x02, 0x03, 0x03, 0x05, 0x03, 0x04,
		0x04, 0x03, 0x01, 0x03, 0x05, 0x00,
};

// Font offsets into the data table
static const uint16_t fontSansSerif8x10Offsets[] PROGMEM = {
		0,   1,   2,   3,   8,  13,  19,  24,  25, 27,
		29,  30,  33,  35,  37,  38,  41,  45,  47, 51,
		55,  59,  63,  67,  71,  75,  79,  80,  82, 86,
		90,  94,  99, 107, 112, 116, 121, 125, 129, 133,
		139, 143, 144, 147, 151, 154, 159, 163, 169, 173,
		179, 183, 188, 193, 197, 202, 209, 214, 219, 224,
		226, 229, 231, 234, 238, 239, 243, 246, 250, 254,
		258, 260, 264, 267, 268, 270, 273, 274, 279, 282,
		286, 289, 292, 294, 298, 300, 303, 306, 311, 314,
		318, 322, 325, 326, 329, 334
};

//Font data
static const uint8_t fontSansSerif8x10Data[] PROGMEM = {
	0x00, // 32
	0x2F, // 33
	0x03, // 34
	0x12, 0x3F, 0x12, 0x3F, 0x12, // 35
	0x12, 0x25, 0x7F, 0x29, 0x12, // 36
	0x23, 0x13, 0x08, 0x04, 0x32, 0x31, // 37
	0x10, 0x2A, 0x2D, 0x32, 0x28, // 38
	0x03, // 39
	0x7E, 0x81, // 40
	0x81, 0x7E, // 41
	0x03, // 42
	0x08, 0x1C, 0x08, // 43
	0x40, 0x20, // 44
	0x10, 0x10, // 45
	0x20, // 46
	0xE0, 0x18, 0x07, // 47
	0x1E, 0x21, 0x21, 0x1E, // 48
	0x02, 0x3F, // 49
	0x22, 0x31, 0x29, 0x26, // 50
	0x12, 0x21, 0x25, 0x1E, // 51
	0x0C, 0x0A, 0x3F, 0x08, // 52
	0x12, 0x25, 0x25, 0x39, // 53
	0x1E, 0x29, 0x29, 0x3A, // 54
	0x01, 0x31, 0x0D, 0x03, // 55
	0x1A, 0x25, 0x25, 0x1A, // 56
	0x26, 0x29, 0x29, 0x1E, // 57
	0x24, // 58
	0x40, 0x24, // 59
	0x08, 0x14, 0x14, 0x22, // 60
	0x14, 0x14, 0x14, 0x14, // 61
	0x22, 0x14, 0x14, 0x08, // 62
	0x02, 0x01, 0x29, 0x09, 0x06, // 63
	0x3C, 0x42, 0xBD, 0xA5, 0xA5, 0xBD, 0x22, 0x1C, // 64
	0x30, 0x0E, 0x09, 0x0E, 0x30, // 65
	0x3F, 0x29, 0x29, 0x1E, // 66
	0x1C, 0x22, 0x21, 0x21, 0x12, // 67
	0x3F, 0x21, 0x21, 0x1E, // 68
	0x3F, 0x29, 0x29, 0x21, // 69
	0x3F, 0x09, 0x09, 0x01, // 70
	0x0C, 0x32, 0x21, 0x29, 0x29, 0x1A, // 71
	0x3F, 0x04, 0x04, 0x3F, // 72
	0x3F, // 73
	0x10, 0x20, 0x3F, // 74
	0x3F, 0x0C, 0x12, 0x21, // 75
	0x3F, 0x20, 0x20, // 76
	0x3F, 0x02, 0x04, 0x02, 0x3F, // 77
	0x3F, 0x04, 0x08, 0x3F, // 78
	0x0C, 0x12, 0x21, 0x21, 0x12, 0x0C, // 79
	0x3F, 0x09, 0x09, 0x06, // 80
	0x0C, 0x12, 0x21, 0x29, 0x12, 0x2C, // 81
	0x3F, 0x09, 0x09, 0x36, // 82
	0x12, 0x25, 0x29, 0x29, 0x12, // 83
	0x01, 0x01, 0x3F, 0x01, 0x01, // 84
	0x3F, 0x20, 0x20, 0x3F, // 85
	0x03, 0x1C, 0x20, 0x1C, 0x03, // 86
	0x0F, 0x30, 0x0C, 0x03, 0x0C, 0x30, 0x0F, // 87
	0x21, 0x12, 0x0C, 0x12, 0x21, // 88
	0x01, 0x06, 0x38, 0x06, 0x01, // 89
	0x21, 0x31, 0x2D, 0x23, 0x21, // 90
	0xFF, 0x81, // 91
	0x07, 0x18, 0xE0, // 92
	0x81, 0xFF, // 93
	0x06, 0x01, 0x06, // 94
	0x80, 0x80, 0x80, 0x80, // 95
	0x01, // 96
	0x3A, 0x2A, 0x2A, 0x3C, // 97
	0x3F, 0x24, 0x3C, // 98
	0x18, 0x24, 0x24, 0x24, // 99
	0x18, 0x24, 0x24, 0x3F, // 100
	0x1C, 0x2A, 0x2A, 0x2C, // 101
	0x3E, 0x05, // 102
	0x48, 0x94, 0x94, 0x7C, // 103
	0x3F, 0x04, 0x3C, // 104
	0x3A, // 105
	0x80, 0x74, // 106
	0x3F, 0x08, 0x34, // 107
	0x3F, // 108
	0x3C, 0x04, 0x3C, 0x04, 0x3C, // 109
	0x3C, 0x04, 0x3C, // 110
	0x18, 0x24, 0x24, 0x18, // 111
	0x7C, 0x14, 0x1C, // 112
	0x1C, 0x14, 0x7C, // 113
	0x3C, 0x04, // 114
	0x24, 0x2A, 0x2A, 0x12, // 115
	0x1F, 0x24, // 116
	0x3C, 0x20, 0x3C, // 117
	0x0C, 0x30, 0x0C, // 118
	0x0C, 0x30, 0x0C, 0x30, 0x0C, // 119
	0x34, 0x08, 0x34, // 120
	0x4C, 0x90, 0x90, 0x7C, // 121
	0x24, 0x34, 0x2C, 0x24, // 122
	0x08, 0x76, 0x81, // 123
	0xE7, // 124
	0x81, 0x76, 0x08, // 125
	0x08, 0x04, 0x08, 0x10, 0x08, // 126
};

/**
 * Font descriptor
 */
static const FONT fontSansSerif8x10 = {
	10,								// width
	8,								// height
	' ',							// first character
	0x60,							// character count
	' ',							// unknown character
	1,								// number of bytes per column
	fontSansSerif8x10Widths,	// widths
	fontSansSerif8x10Offsets,	// offsets
	fontSansSerif8x10Data		// data
};

}

#endif /* MHV_FONT_SANSSERIF_8X10_H */
