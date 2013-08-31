/* Copyright (c) 2013, Make, Hack, Void Inc, Tim (cpldcpu@gmail.com)
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"		AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL MAKE, HACK, VOID BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Adapted from:
 * https://github.com/cpldcpu/light_ws2812/blob/master/light_ws2812_AVR/light_ws2812.c
 */

#ifndef MHV_WS2811_H_
#define MHV_WS2811_H_

#define MHV_RGB_ORDER 3

#include <mhvlib/RGBLEDStrip.h>
#include <avr/cpufunc.h>

namespace mhvlib {

/**
 * Create a new WS2811 object to control a string of LED drivers
 * @tparam	dataPin...		the data pin for the LEDs (This must be the same pin that Output2 (OCRnB) is on)
 * @tparam	length		the number of LEDs in the string
 * @tparam  timer		the timer parameters
 */
template<MHV_DECLARE_PIN(dataPin), uint16_t length>
class WS2811: public RGBLEDStrip<length> {
public:
	/**
	 * Constructor
	 */
	WS2811() {
		setOutput(MHV_PIN_PARMS(dataPin));
		pinOff(MHV_PIN_PARMS(dataPin));
	}

	/**
	 * Write the current buffer to the string of chips
	 */
	void flush() {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			uint8_t masklo = _MMIO_BYTE(dataPinOut) & ~_BV(dataPinPin);
			uint8_t maskhi = _MMIO_BYTE(dataPinOut) | _BV(dataPinPin);
			uint8_t currentByte;
			uint8_t bitCount;
			uint16_t remaining = length * 3;
			uint8_t *data = (uint8_t *)RGBLEDStrip<length>::_data;

#if   F_CPU == 20000000
			/* The total length of each bit is 1.25us(25 cycles @ 20Mhz / 50ns per cyc)
			 * * At 0us the dataline is pulled high.
			 * * To send a zero the dataline is pulled low after 0.350us (7 cyc)
			 * * To send a one the dataline is pulled low after 0.700us (14 cyc)
			 * After the entire bitstream has been written, the dataout pin has to remain low
			 * for at least 50us (reset condition).
			 * Data transfer time( H+L=1.25us±600ns)
			 * 0H: high time 0.35us ±150ns (7 cyc = 0.35us )
			 * 0L: low time 0.8us ±150ns (18 cyc = 0.9us 100ns more, but within tolerance )
			 * 1H: high time 0.7us ±150ns (14 cyc = 0.7us )
			 * 1L: low time 0.6us ±150ns (11 cyc = 0.55us 50ns less, but within tolerance)
			 */

			while (remaining--) {
				currentByte = *data++;

				asm volatile(
						"		ldi %0,8			\n\t"	// 0
						"loop%=:out %2, %3			\n\t"	// 1
						"		lsl %1				\n\t"	// 2
						"		dec %0				\n\t"	// 3

						"		rjmp .+0			\n\t"	// 5
						"		nop					\n\t"	// 6
						"		brcs .+2			\n\t"	// 7l / 8h
						"		out %2, %4			\n\t"	// 8l / -

						"		rjmp .+0			\n\t"	// 10
						"		rjmp .+0			\n\t"	// 12
						"		rjmp .+0			\n\t"	// 14
						"		out %2, %4			\n\t"	// 15
						"		breq end%=			\n\t"	// 16 nt. 17 taken

						"		rjmp .+0			\n\t"	// 18
						"		rjmp .+0			\n\t"	// 20
						"		rjmp .+0			\n\t"	// 22
						"		nop					\n\t"	// 23
						"		rjmp loop%=			\n\t"	// 25
						"end%=:						\n\t"
						: "=&d" (bitCount)
						: "r" (currentByte), "I" (dataPinOut - __SFR_OFFSET), "r" (maskhi), "r" (masklo)
				);
			}
#elif F_CPU == 16000000 || F_CPU == 16500000
			/*	The total length of each bit is 1.25us (20 cycles @ 16Mhz)
			 * * At 0us the dataline is pulled high.
			 * * To send a zero the dataline is pulled low after 0.375us (6 cycles).
			 * * To send a one the dataline is pulled low after 0.625us (10 cycles).
			 * After the entire bitstream has been written, the dataout pin has to remain low
			 * for at least 50uS (reset condition).
			 * Due to the loop overhead there is a slight timing error: The loop will execute
			 * in 21 cycles for the last bit write. This does not cause any issues though,
			 * as only the timing between the rising and the falling edge seems to be critical.
			 * Some quick experiments have shown that the bitstream has to be delayed by
			 * more than 3us until it cannot be continued (3us=48 cyles).
			 */

			while (remaining--) {
				currentByte = *data++;

				asm volatile(
						"		ldi %0,8		\n\t"	// 0
						"loop%=:out %2, %3		\n\t"	// 1
						"		lsl %1			\n\t"	// 2
						"		dec %0			\n\t"	// 3

						"		rjmp .+0		\n\t"	// 5

						"		brcs .+2		\n\t"	// 6l / 7h
						"		out %2, %4		\n\t"	// 7l / -

						"		rjmp .+0		\n\t"	// 9

						"		nop				\n\t"	// 10
						"		out %2, %4		\n\t"	// 11
						"		breq end%=		\n\t"	// 12 nt. 13 taken

						"		rjmp .+0		\n\t"	// 14
						"		rjmp .+0		\n\t"	// 16
						"		rjmp .+0		\n\t"	// 18
						"		rjmp loop%=		\n\t"	// 20
						"end%=:					\n\t"
						: "=&d" (bitCount)
						: "r" (currentByte), "I" (dataPinOut - __SFR_OFFSET), "r" (maskhi), "r" (masklo)
				);
			}
#elif F_CPU == 12000000
			/*	The total length of each bit is 1.25us (15 cycles @ 12Mhz)
			 * * At 0us the dataline is pulled high. (cycle 1+0)
			 * * To send a zero the dataline is pulled low after 0.333us (1+4=5 cycles).
			 * * To send a one the dataline is pulled low after 0.666us (1+8=9 cycles).
			 *
			 * Total loop timing is correct, but the timing for the falling edge can
			 * not be accurately reached as the correct 0.375us (4.5 cyc.) and 0.675us (7.5 cyc)
			 * timings fall in between cycles.
			 * Final timing:
			 * * 15 cycles for bits 7-1
			 * * 16 cycles for bit 0
			 * - The bit 0 timing exceeds the 1.25us bit-timing by 66.7ns, which is still
			 * within datasheet tolerances (600ns)
			 */
			asm volatile(
					"		in %0,%2		\n\t"
					"		or %3,%0		\n\t"
					"		and %4,%0		\n\t"
					"olop%=:subi %A6,1		\n\t"	// 12
					"		sbci %B6,0		\n\t"	// 13
					"		brcs exit%=		\n\t"	// 14
					"		ld %1,X+		\n\t"	// 15
					"		ldi %0,8		\n\t"	// 16

					"loop%=:out %2, %3		\n\t"	// 1
					"		lsl %1			\n\t"	// 2
					"		nop				\n\t"	// 3

					"		brcs .+2		\n\t"	// 4nt / 5t
					"		out %2, %4		\n\t"	// 5
					"		dec %0			\n\t"	// 6
					"		rjmp .+0		\n\t"	// 8
					"		out %2, %4		\n\t"	// 9
					"		breq olop%=		\n\t"	// 10nt / 11t
					"		nop				\n\t"	// 11
					"		rjmp .+0		\n\t"	// 13
					"		rjmp loop%=		\n\t"	// 15
					"exit%=:				\n\t"
					: "=&d" (bitCount)
					: "r" (currentByte), "I" (dataPinOut - __SFR_OFFSET), "r" (maskhi), "r" (masklo), "x" (data), "r" (remaining)
			);
#elif F_CPU ==  9600000
			/* The total length of each bit is 1.25us (12 cycles @ 9.6Mhz)
			 * * At 0us the dataline is pulled high. (cycle 1)
			 * * To send a zero the dataline is pulled low after 0.312us (1+3=4 cycles) (error 0.06us)
			 * * To send a one the dataline is pulled low after 0.625us (1+6=7 cycles) (no error).
			 *
			 * 12 cycles can not be reached for bit 0 write. However since the timing
			 * between the rising and falling edge is correct, it seems to be acceptable
			 * to slightly increase bit timing
			 *
			 * Final timing:
			 * * 12 cycles for bits 7-1
			 * * 15 cycles for bit 0
			 *
			 * - The bit 0 timing exceeds the 1.25us timing by 312ns, which is still within
			 * datasheet tolerances (600ns).
			 */
			asm volatile(
					"		in %0,%2		\n\t"
					"		or %3,%0		\n\t"
					"		and %4,%0		\n\t"
					"olop%=:subi %A6,1		\n\t"	// 10
					"		sbci %B6,0		\n\t"	// 11
					"		brcs exit%=		\n\t"	// 12
					"		ld %1,X+		\n\t"	// 14
					"		ldi %0,8		\n\t"	// 15

					"loop%=:out %2, %3		\n\t"	// 1
					"		lsl %1			\n\t"	// 2

					"		brcs .+2		\n\t"	// 3nt / 4t
					"		out %2, %4		\n\t"	// 4
					"		dec %0			\n\t"	// 5
					"		nop				\n\t"	// 6
					"		out %2, %4		\n\t"	// 7
					"		breq olop%=		\n\t"	// 8nt / 9t
					"		rjmp .+0		\n\t"	// 10
					"		rjmp loop%=		\n\t"	// 12
					"exit%=: \n\t"
					: "=&d" (bitCount)
					: "r" (currentByte), "I" (dataPinOut - __SFR_OFFSET), "r" (maskhi), "r" (masklo), "x" (data), "r" (remaining)
			);
#elif F_CPU ==  8000000
			/* 	The total length of each bit is 1.25us (10 cycles @ 8Mhz)
			 * * At 0us the dataline is pulled high. (cycle 1+0=1)
			 * * To send a zero the dataline is pulled low after 0.375us (1+3=4 cycles).
			 * * To send a one the dataline is pulled low after 0.625us (1+5=6 cycles).
			 *
			 * Final timing:
			 * * 10 cycles for bits 7-1
			 * * 14 cycles for bit 0
			 * - The bit 0 timing exceeds the 1.25us bit-timing by 500ns, which is still
			 * within datasheet tolerances (600ns)
			 */
			asm volatile(
					"		in %0,%2		\n\t"
					"		or %3,%0		\n\t"
					"		and %4,%0		\n\t"
					"olop%=:subi %A6,1		\n\t"	// 9
					"		sbci %B6,0		\n\t"	// 10
					"		brcs exit%=		\n\t"	// 11
					"		ld %1,X+		\n\t"	// 13
					"		ldi %0,8		\n\t"	// 14

					"loop%=:out %2, %3		\n\t"	// 1
					"		lsl %1			\n\t"	// 2

					"		brcs .+2		\n\t"	// 3nt / 4t
					"		out %2, %4		\n\t"	// 4
					"		dec %0			\n\t"	// 5
					"		out %2, %4		\n\t"	// 6
					"		breq olop%=		\n\t"	// 7nt / 8t
					"		nop				\n\t"	// 8
					"		rjmp loop%=		\n\t"	// 10
					"exit%=:				\n\t"
					: "=&d" (bitCount)
					: "r" (currentByte), "I" (dataPinOut - __SFR_OFFSET), "r" (maskhi), "r" (masklo), "x" (data), "r" (remaining)
			);
#elif F_CPU ==  4000000
			/* The total length of each bit is 1.25us (5 cycles @ 4Mhz)
			 * * At 0us the dataline is pulled high. (cycle 0+1)
			 * * To send a zero the dataline is pulled low after 0.5us (spec: 0.375us) (2+1=3 cycles).
			 * * To send a one the dataline is pulled low after 0.75us (spec: 0.625us) (3+1=4 cycles).
			 *
			 * The timing of this implementation is slightly off, however it seems to
			 * work empirically.
			 * Final timing:
			 * * 5 cycles for bits 7-1
			 * * 6 cycles for bit 0
			 * - The bit 0 timing exceeds the 1.25us timing by 250ns, which is still within
			 * the tolerances stated in the datasheet (600 ns).
			 */
			asm volatile(
					"		ld %0,X				\n\t"

					"olop%=:out %2, %3			\n\t"	// 1
					"		sbrs %0,7			\n\t"	// 2
					"		out %2, %6			\n\t"	// 3
					"		out %2, %6			\n\t"	// 4
					"		subi r26,-1			\n\t"	// 5

					"		out %2, %3			\n\t"	// 1
					"		sbrs %0,6			\n\t"	// 2
					"		out %2, %6			\n\t"	// 3
					"		out %2, %6			\n\t"	// 4
					"		sbci r27,-1			\n\t"	// 5

					"		out %2, %3			\n\t"	// 1
					"		sbrs %0,5			\n\t"	// 2
					"		out %2, %6			\n\t"	// 3
					"		out %2, %6			\n\t"	// 4
					"		mov %1,%0			\n\t"	// 5

					"		out %2, %3			\n\t"	// 1
					"		sbrs %1,4			\n\t"	// 2
					"		out %2, %6			\n\t"	// 3
					"		out %2, %6			\n\t"	// 4
					"		nop 				\n\t"	// 5

					"		out %2, %3			\n\t"	// 1
					"		sbrs %1,3			\n\t"	// 2
					"		out %2, %6			\n\t"	// 3
					"		out %2, %6			\n\t"	// 4
					"		nop					\n\t"	// 5

					"		out %2, %3			\n\t"	// 1
					"		sbrs %1,2			\n\t"	// 2
					"		out %2, %6			\n\t"	// 3
					"		out %2, %6			\n\t"	// 4
					"		ld %0,X				\n\t"	// 5

					"		out %2, %3			\n\t"	// 1
					"		sbrs %1,1			\n\t"	// 2
					"		out %2, %6			\n\t"	// 3
					"		out %2, %6			\n\t"	// 4
					"		dec %5				\n\t"	// 5

					"		out %2, %3			\n\t"	// 1
					"		sbrs %1,0			\n\t"	// 2
					"		out %2, %6			\n\t"	// 3
					"		out %2, %6			\n\t"	// 4

					"		brne olop%=			\n\t"	// 6

					: "=&d" (bitCount), "=&d" (currentByte)
					: "I" (dataPinOut - __SFR_OFFSET), "r" (maskhi), "x" (data), "r" (remaining), "r" (masklo)
			);
#else
#error Clock speed not supported
#endif
		} // ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	} // void flush()
};

} // namespace mhvlib
#endif /* MHV_WS2811_H_ */
