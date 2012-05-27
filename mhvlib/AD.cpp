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

#include <mhvlib/AD.h>

uint16_t mhv_ad_busyRead(uint8_t channel, uint8_t reference) {
	ADMUX = reference | (channel & 0x0F);

#ifdef MUX5
	ADCSRB = (ADCSRB & ~_BV(MUX5)) | ((channel & _BV(5) >> (5-MUX5)));
#endif

// Start the conversion
	ADCSRA |= _BV(ADSC);

	while (ADCSRA & _BV(ADSC)) {};

	return ADC;
}

void mhv_ad_asyncRead(uint8_t channel, uint8_t reference) {
	ADMUX = reference | (channel & 0x0F);

#ifdef MUX5
	ADCSRB = (ADCSRB & ~_BV(MUX5)) | ((channel & _BV(5) >> (5-MUX5)));
#endif

// Start the conversion
	ADCSRA |= _BV(ADSC);
}

void mhv_ad_setPrescaler(MHV_AD_PRESCALER prescaler) {
	ADCSRA = (ADCSRA & 0xf8) | (prescaler & 0x7);
}
