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


#ifndef MHV_RGB_H_
#define MHV_RGB_H_

#include <mhvlib/io.h>

namespace mhvlib_bsd {

/**
 * Set the colour order of the MHV_RGB struct
 * 	RGB	1
 *	RBG	2
 *	GRB	3
 *	GBR	4
 *	BGR	5
 *	BRG	6
 */

#define MHV_RGB_ORDER_RGB	1
#define MHV_RGB_ORDER_RBG	2
#define MHV_RGB_ORDER_GRB	3
#define MHV_RGB_ORDER_GBR	4
#define MHV_RGB_ORDER_BGR	5
#define MHV_RGB_ORDER_BRG	6

#ifndef MHV_RGB_ORDER
#define MHV_RGB_ORDER	MHV_RGB_ORDER_RGB
#endif

#if MHV_RGB_ORDER == MHV_RGB_ORDER_RGB
struct rgb {
	uint8_t	red;
	uint8_t	green;
	uint8_t	blue;
};
#elif MHV_RGB_ORDER == MHV_RGB_ORDER_RBG
struct rgb {
	uint8_t	red;
	uint8_t	blue;
	uint8_t	green;
};
#elif MHV_RGB_ORDER == MHV_RGB_ORDER_GRB
struct rgb {
	uint8_t	green;
	uint8_t	red;
	uint8_t	blue;
};
#elif MHV_RGB_ORDER == MHV_RGB_ORDER_GBR
struct rgb {
	uint8_t	green;
	uint8_t	blue;
	uint8_t	red;
};
#elif MHV_RGB_ORDER == MHV_RGB_ORDER_BGR
struct rgb {
	uint8_t	blue;
	uint8_t	green;
	uint8_t	red;
};
#elif MHV_RGB_ORDER == MHV_RGB_ORDER_BRG
struct rgb {
	uint8_t	blue;
	uint8_t	red;
	uint8_t	green;
};
#endif

typedef struct rgb RGB;

}

#endif /* MHV_RGB_H_ */
