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
 *
 * Ported and relicensed with permission under a BSD license from muCSense by Rolfe Schmidt
 * Originally published under GPLv3
 *
 * Copyright (c) Rolfe Schmidt
 * All rights reserved.
 */

#ifndef BESTSPHEREGAUSSNEWTONCALIBRATOR_H_
#define BESTSPHEREGAUSSNEWTONCALIBRATOR_H_

/**
 * BestSphereGaussNewtonCalibrator.h
 * This class implements a constant-space, one-pass streaming algorithm
 * to fit a set if three-dimensional points to an axis-oriented ellipsoid.
 * It is designed to calibrate magnetometers and accelerometers, or other sensors
 * that should take measurements of some constant vector at different orientations.
 * These objects consume a fair bit of memory for a uC, so it is best not to leave them lying around
 *when not in use (e.g. as global variables or memory leaks).
 */

#include <stddef.h> //for size_t
#include <stdint.h>
#include <mhvlib/io.h>
#include <mhvlib/TripleAxisCalibrator.h>

namespace mhvlib {

class BestSphereGaussNewtonCalibrator : public TripleAxisCalibrator {
protected:
	//Final calibration parameters
	float _beta[6];

	//observation summary structures
	float _mu[3]; //sum of all observations in each dimension
	float _mu2[3]; //sum of squares of all observations in each dimension
	float _ipXX[6]; //Symmetric matrix of inner products of observations with themselves
	float _ipX2X[3][3]; //matrix of inner products of squares of observations with the observations
	float _ipX2X2[6]; //Symmetric matrix of inner products of squares of observations with themselves
	uint16_t _observationCount; //The number of observations

	int16_t _obsMin[3]; // Keep track of min and max observation in each dimension to guess parameters
	int16_t _obsMax[3]; // Keep track of min and max observation in each dimension to guess parameters

public:
	BestSphereGaussNewtonCalibrator();
	/**
	 * Add a sample to the calibrator
	 * @param	sample		the sample to send
	 */
	void addObservation(union Int3Axis *sample);

	virtual void calibrate();

private:
	const uint8_t upperTriangularIndex(uint8_t i, uint8_t j);
	void clearObservationMatrices();
	void guessParameters();
	void clearGNMatrices(float JtJ[][6], float JtR[]);
	void computeGNMatrices(float JtJ[][6], float JtR[]);
	void findDelta(float JtJ[][6], float JtR[]);

}; // class BestSphereGaussNewtonCalibrator
} // namespace mhvlib

#endif /* BESTSPHEREGAUSSNEWTONCALIBRATOR_H_ */
