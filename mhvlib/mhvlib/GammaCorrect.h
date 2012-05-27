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

#ifndef MHV_GAMMACORRECT_H_
#define MHV_GAMMACORRECT_H_

#include <inttypes.h>
#include <math.h>
#include <avr/pgmspace.h>
#include <mhvlib/io.h>

const uint8_t mhv_gammaValues[] PROGMEM = {
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


uint8_t mhv_calculatedGammaCorrect(uint8_t value);

/* Gamma correct a value via a lookup table
 * This uses less program memory than calculated value, and uses less clocks
 * param	value	the value to gamma correct
 * return the gamma corrected value
 */
inline uint8_t mhv_precalculatedGammaCorrect(uint8_t value) {
	return pgm_read_byte(mhv_gammaValues + value);
}

#define MHV_PRECALCULATED_GAMMA_CORRECT mhv_precalculatedGammaCorrect

#endif /* MHV_GAMMACORRECT_H_ */
