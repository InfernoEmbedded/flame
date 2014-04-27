/* Copyright (c) 2014, Inferno Embedded
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

#ifndef FLAME_GAMMACORRECT_H_
#define FLAME_GAMMACORRECT_H_

#include <inttypes.h>
#include <math.h>
#include <avr/pgmspace.h>
#include <flame/io.h>

namespace flame {

#if 0 && defined __FLASH
const uint8_t __flash gammaValues[] = {
#else
const uint8_t gammaValues[] PROGMEM = {
#endif
	0,		0,		0,		0,		0,		0,		0,		0,
	0,		0,		0,		0,		0,		0,		0,		0,
	0,		0,		0,		0,		0,		0,		1,		1,
	1,		1,		1,		1,		1,		1,		1,		1,
	1,		2,		2,		2,		2,		2,		2,		2,
	2,		3,		3,		3,		3,		3,		4,		4,
	4,		4,		4,		5,		5,		5,		5,		6,
	6,		6,		6,		7,		7,		7,		7,		8,
	8,		8,		9,		9,		9,		10,		10,		10,
	11,		11,		12,		12,		12,		13,		13,		14,
	14,		15,		15,		15,		16,		16,		17,		17,
	18,		18,		19,		19,		20,		20,		21,		22,
	22,		23,		23,		24,		25,		25,		26,		26,
	27,		28,		28,		29,		30,		30,		31,		32,
	33,		33,		34,		35,		36,		36,		37,		38,
	39,		40,		40,		41,		42,		43,		44,		45,
	46,		46,		47,		48,		49,		50,		51,		52,
	53,		54,		55,		56,		57,		58,		59,		60,
	61,		62,		63,		64,		65,		67,		68,		69,
	70,		71,		72,		73,		75,		76,		77,		78,
	80,		81,		82,		83,		85,		86,		87,		89,
	90,		91,		93,		94,		95,		97,		98,		99,
	101,	102,	104,	105,	107,	108,	110,	111,
	113,	114,	116,	117,	119,	121,	122,	124,
	125,	127,	129,	130,	132,	134,	135,	137,
	139,	141,	142,	144,	146,	148,	150,	151,
	153,	155,	157,	159,	161,	163,	165,	166,
	168,	170,	172,	174,	176,	178,	180,	182,
	184,	186,	189,	191,	193,	195,	197,	199,
	201,	204,	206,	208,	210,	212,	215,	217,
	219,	221,	224,	226,	228,	231,	233,	235,
	238,	240,	243,	245,	248,	250,	253,	255
};

const uint8_t inverseGammaValues[] PROGMEM = {
	0,		28,		37,		43,		48,		53,		57,		61,
	64,		67,		70,		73,		75,		78,		80,		82,
	84,		86,		88,		90,		92,		94,		96,		97,
	99,		101,	102,	104,	105,	107,	108,	110,
	111,	113,	114,	115,	117,	118,	119,	120,
	122,	123,	124,	125,	126,	127,	129,	130,
	131,	132,	133,	134,	135,	136,	137,	138,
	139,	140,	141,	142,	143,	144,	145,	146,
	147,	148,	149,	149,	150,	151,	152,	153,
	154,	155,	155,	156,	157,	158,	159,	160,
	160,	161,	162,	163,	164,	164,	165,	166,
	167,	167,	168,	169,	170,	170,	171,	172,
	173,	173,	174,	175,	175,	176,	177,	177,
	178,	179,	179,	180,	181,	182,	182,	183,
	183,	184,	185,	185,	186,	187,	187,	188,
	189,	189,	190,	190,	191,	192,	192,	193,
	194,	194,	195,	195,	196,	197,	197,	198,
	198,	199,	199,	200,	201,	201,	202,	202,
	203,	203,	204,	205,	205,	206,	206,	207,
	207,	208,	208,	209,	209,	210,	211,	211,
	212,	212,	213,	213,	214,	214,	215,	215,
	216,	216,	217,	217,	218,	218,	219,	219,
	220,	220,	221,	221,	222,	222,	223,	223,
	224,	224,	225,	225,	226,	226,	227,	227,
	228,	228,	229,	229,	230,	230,	230,	231,
	231,	232,	232,	233,	233,	234,	234,	235,
	235,	235,	236,	236,	237,	237,	238,	238,
	239,	239,	240,	240,	240,	241,	241,	242,
	242,	243,	243,	243,	244,	244,	245,	245,
	246,	246,	246,	247,	247,	248,	248,	248,
	249,	249,	250,	250,	251,	251,	251,	252,
	252,	253,	253,	253,	254,	254,	255,	255
};
uint8_t calculatedGammaCorrect(uint8_t value);

/* Gamma correct a value via a lookup table
 * This uses less program memory than calculated value, and uses less clocks
 * param	value	the value to gamma correct
 * return the gamma corrected value
 */
inline uint8_t precalculatedGammaCorrect(uint8_t value) {
#if 0 && defined __FLASH
	return gammaValues[value];
#else
	return pgm_read_byte(gammaValues + value);
#endif
}

inline uint8_t precalculatedInverseGammaCorrect(uint8_t value) {
	return pgm_read_byte(inverseGammaValues + value);
}

}

#endif /* FLAME_GAMMACORRECT_H_ */
