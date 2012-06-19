/*
 * Copyright (c) 2011, Make, Hack, Void Inc
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
#if 0
/**
 * Enhanced Parallel Port Slave
 */

#ifndef MHV_EPP_H_
#define MHV_EPP_H_

#include <mhvlib/io.h>

namespace mhvlib {

enum class epp_mode : uint8_t {
	INTERRUPT,
	POLLED
};
typedef enum epp_mode EPP_MODE;

class EPP {
private:
		uint8_t				_address;
		EPP_MODE			_mode;

		volatile uint8_t	*_dataDir;
		volatile uint8_t	*_dataInput;
		volatile uint8_t	*_dataOutput;

		uint8_t				_controlPrevious;

		volatile uint8_t	*_controlDir;
		volatile uint8_t	*_controlInput;
		volatile uint8_t	*_controlOutput;

public:
	EPP(EPP_MODE mode,
			MHV_DECLARE_PIN(data),
			MHV_DECLARE_PIN(control));
	void interruptReset();
	void interruptStrobe(bool address);
	void releaseStrobe(bool address);
	void poll();

protected:
	// Subclasses must implement these
	virtual void _reset()=0;
	virtual uint8_t _read(bool address)=0;
	virtual void _write(bool address, uint8_t data)=0;
};

}

#endif /* MHV_EPP_H_ */
#endif
