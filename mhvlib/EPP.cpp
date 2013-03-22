/*
 * Copyright (c) 2012, Make, Hack, Void Inc
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
#include <mhvlib/EPP.h>

/**
 * An Enhanced Parallel Port Device
 *
 * Data Port
 * 	Bit		Definition
 * 	0-7		Data0-7 (in/out)
 *
 * Control Port
 *  Bit		Definition
 *  0		Data strobe		(in, active low)
 *  1		Address strobe	(in, active low)
 *  2		Write 			(in, active low)
 *  3		Interrupt		(out, active on positive edge)
 *  4		Wait			(out, active high)
 *  5		Reset			(in, active low)
 *
 * Interrupt pins (to be connected to interrupts if interrupt mode is used)
 * Data Strobe		interrupt, active on both edges, should call interruptStrobe(false), releaseStrobe(false)
 * Address Strobe	interrupt, active on both edges, should call interruptStrobe(true), releaseStrobe(true)
 * Reset			interrupt, active on rising edge, should call interruptReset()
 */

#define MHV_EPP_DATA_STROBE		0
#define MHV_EPP_ADDRESS_STROBE	1
#define MHV_EPP_WRITE			2
#define MHV_EPP_INTERRUPT		3
#define MHV_EPP_WAIT			4
#define MHV_EPP_RESET			5

#define MHV_EPP_DATA 	_dataDir, _dataInput, _dataOutput
#define MHV_EPP_CONTROL	_controlDir, _controlInput, _controlOutput

/**
 * Create a new Enhanced Parallel Port Slave
 *
 * @param		mode		the mode of the port
 * @param		data		Pin0 of the data port (we will use the full port)
 * @param		control		the port register for the control pins
 */
EPP::EPP(EPPMode mode,
		MHV_DECLARE_PIN(data),
		MHV_DECLARE_PIN(control)) {
	_address = 0;
	_mode = mode;

	_dataDir = dataDir;
	_dataInput = dataInput;
	_dataOutput = dataOutput;

	_controlDir = controlDir;
	_controlInput = controlInput;
	_controlOutput = controlOutput;

// Set up the pin directions
	*_dataDir = 0;
	*_controlDir = (*_controlDir & 0xc0) | 0x27;

// Set up the pin pullups for inputs, initial states for outputs
	*_dataOutput = 0;
	*_controlOutput = (*_controlOutput & 0xc0) | 0x00;
	_controlPrevious = *_controlInput;
}

/**
 * Poll the EPP device to look for new data
 */
void EPP::poll() {
	uint8_t control = *_controlInput;
	uint8_t changed = control ^ _controlPrevious;

	if (changed & _BV(MHV_EPP_DATA_STROBE)) {
		if (control & _BV(MHV_EPP_DATA_STROBE)) {
			releaseStrobe(false);
		} else {
			interruptStrobe(false);
		}
	} else if (changed & _BV(MHV_EPP_ADDRESS_STROBE)) {
		if (control & _BV(MHV_EPP_ADDRESS_STROBE)) {
			releaseStrobe(true);
		} else {
			interruptStrobe(true);
		}
	} else if ((changed & _BV(MHV_EPP_RESET)) &&
			(control & _BV(MHV_EPP_RESET))) {
		interruptReset();
	}

	_controlPrevious = control;
}

/**
 * A reset interrupt has been triggered
 */
void EPP::interruptReset() {
	_reset();
}

/**
 * A strobe has been triggered
 * @param	address	true if it is the address strobe, false if it is the data strobe
 */
void EPP::interruptStrobe(bool address) {
	if (mhv_pinRead(MHV_EPP_CONTROL, MHV_EPP_WRITE, 0)) {
// Read cycle (host is reading from us), write is active low

// Set data port to output
		*_dataDir = 0x00;

// Write the data to the host
		*_dataOutput = _read(address++);
	} else {
// Write cycle (host is writing to us)

// Set data port to input
		*_dataDir = 0xff;

// Read the data from the host
		_write(address++, *_dataOutput);
	}

// Raise wait
	mhv_pinOn(MHV_EPP_CONTROL, MHV_EPP_WAIT, 0);
}

/**
 * A strobe has been released
 * @param	address	true if it is the address strobe, false if it is the data strobe
 */
void EPP::releaseStrobe(bool address) {
// Drop wait
	mhv_pinOff(MHV_EPP_CONTROL, MHV_EPP_WAIT, 0);
}

#endif
