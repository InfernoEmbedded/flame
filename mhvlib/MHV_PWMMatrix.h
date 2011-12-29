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

#ifndef MHV_PWMMATRIX_H_
#define MHV_PWMMATRIX_H_

#include <inttypes.h>
#include <MHV_Display_Monochrome_Buffered.h>
#include <MHV_Timer8.h>

/**
 * Create a new PWM Matrix object
 * @param	_mhvObjectName		the variable name of the object
 * @param	_mhvRows			the number of rows
 * @param	_mhvCols			the number of columns
 * @param	_mhvTxElements		the number of elements to make available for async transmission
 * @param	_mhvMatrixDriver	the handler to drive rows & cols
 * @param	_mhvMatrixRowOff	function to call to turn a row off
 * @param	_mhvMatrixColOn		function to call to turn a row on
 * @param	_mhvMatrixColOff	function to call to turn a row off
 */
#define MHV_PWMMATRIX_CREATE(_mhvObjectName, _mhvRows, _mhvCols, _mhvTxElements, _mhvMatrixDriver) \
	uint8_t _mhvObjectName ## FrameBuffer[_mhvRows * _mhvCols]; \
	MHV_TX_BUFFER_CREATE(_mhvObjectName ## TX, _mhvTxElements); \
	MHV_PWMMatrix _mhvObjectName(_mhvRows, _mhvCols, _mhvObjectName ## FrameBuffer, _mhvObjectName ## TX, \
			_mhvMatrixDriver);

enum MHV_PWMMatrix_Mode {
	MHV_PWMMATRIX_MODE_AUTO,
	MHV_PWMMATRIX_MODE_ROWS,
	MHV_PWMMATRIX_MODE_COLS,
	MHV_PWMMATRIX_MODE_INDIVIDUAL
};
typedef enum MHV_PWMMatrix_Mode MHV_PWMMATRIX_MODE;

class MHV_PWMMatrixDriver {
public:
	virtual void rowOn(uint16_t row) =0;
	virtual void rowOff(uint16_t row) =0;
	virtual void colOn(uint16_t col) =0;
	virtual void colOff(uint16_t col) =0;
};

class MHV_PWMMatrix : public MHV_Display_Monochrome_Buffered, public MHV_TimerListener {
private:
	uint16_t				_currentRow;
	uint16_t				_currentCol;
	uint8_t					_currentLevel;
	MHV_PWMMATRIX_MODE		_mode;
	MHV_PWMMatrixDriver		&_driver;

	void tickRow();
	void tickCol();
	void tickPixel();

public:
	MHV_PWMMatrix(uint16_t rowCount, uint16_t colCount, uint8_t *frameBuffer, MHV_RingBuffer &txBuffers,
		MHV_PWMMatrixDriver &driver, MHV_PWMMATRIX_MODE mode = MHV_PWMMATRIX_MODE_AUTO);
	void alarm();
};

#endif /* MHV_PWMMATRIX_H_ */
