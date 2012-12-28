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

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <math.h>
#include <mhvlib/io.h>
#include <string.h>
#include <mhvlib/3AxisCalibration/BestSphereGaussNewtonCalibrator.h>

#define	AXIS_COUNT	3

namespace mhvlib {

/**
 * Constructor
 */
BestSphereGaussNewtonCalibrator::BestSphereGaussNewtonCalibrator(TripleAxisSensor &sensor, Device_TX &output) :
		TripleAxisCalibrator(sensor, output) {
	clearObservationMatrices();
}

CONST uint8_t BestSphereGaussNewtonCalibrator::upperTriangularIndex(uint8_t i, uint8_t j) {
	if (i > j) {
		uint8_t temp = i;
		i = j;
		j = temp;
	}

	return (j * (j + 1)) / 2 + i;
}

/**
 * Add a sample to the calibrator
 * @param	sample		the sample to send
 */
void BestSphereGaussNewtonCalibrator::addObservation(const TRIPLEAXISSENSOR_RAW_READING &sample) {
	_observationCount++;

	int32_t squareObs[AXIS_COUNT];
	for (uint8_t i = 0; i < AXIS_COUNT; i++) {
		squareObs[i] = static_cast<int32_t>(sample.value[i]) * sample.value[i];
	}

	for (uint8_t i = 0; i < AXIS_COUNT; ++i) {
		//Keep track of min and max in each dimension
		_obsMin[i] = (sample.value[i] < _obsMin[i]) ? sample.value[i] : _obsMin[i];
		_obsMax[i] = (sample.value[i] > _obsMax[i]) ? sample.value[i] : _obsMax[i];

		//accumulate sum and sum of squares in each dimension
		_mu[i] += sample.value[i];
		_mu2[i] += squareObs[i];

		//accumulate inner products of the vector of observations and the vector of squared observations.
		for (uint8_t j = 0; j < AXIS_COUNT; ++j) {
			_ipX2X[i][j] += squareObs[i] * static_cast<int32_t>(sample.value[j]);
			if (i <= j) {
				uint8_t idx = upperTriangularIndex(i, j);
				_ipXX[idx] += static_cast<int32_t>(sample.value[i]) * static_cast<int32_t>(sample.value[j]);
				_ipX2X2[idx] += squareObs[i] * static_cast<float>(squareObs[j]);
			}
		}
	}

}

/**
 * Reset observations
 */
void BestSphereGaussNewtonCalibrator::clearObservationMatrices() {
	_observationCount = 0;

	memset(_obsMin, 0, 3 * sizeof(*_obsMin));
	memset(_obsMax, 0, 3 * sizeof(*_obsMax));

	for (uint8_t i = 0; i < 3; ++i) {
		_obsMin[i] =INT16_MAX;
		_obsMax[i] = INT16_MIN;
		_mu[i] = 0.0f;
		_mu2[i] = 0.0f;
		_ipXX[i] = 0.0f;
		_ipX2X2[i] = 0.0f;

		for (uint8_t j = 0; j < 3; ++j) {
			_ipX2X[i][j] = 0.0f;
		}
	}
}

void BestSphereGaussNewtonCalibrator::clearGNMatrices(float JtJ[][6], float JtR[]) {
	for (uint8_t j = 0; j < 6; ++j) {
		JtR[j] = 0.0f;
		for (uint8_t k = 0; k < 6; ++k) {
			JtJ[j][k] = 0.0f;
		}
	}

}

void BestSphereGaussNewtonCalibrator::computeGNMatrices(float JtJ[][6], float JtR[]) {
	float beta2[6]; //precompute the squares of the model parameters
	for (uint8_t i = 0; i < 6; ++i) {
		beta2[i] = pow(_beta[i], 2);
	}

//compute the inner product of the vector of residuals with the constant 1 vector, the vector of
// observations, and the vector of squared observations.
	float r = _observationCount; //sum of residuals
	float rx[3]; //Inner product of vector of residuals with each observation vector
	float rx2[3]; //Inner product of vector of residuals with each square observation vector

//now correct the r statistics
	for (uint8_t i = 0; i < 3; ++i) {
		r -= (beta2[i] * _observationCount + _mu2[i] - 2 * _beta[i] * _mu[i]) / beta2[3 + i];
		rx[i] = _mu[i];
		rx2[i] = _mu2[i];
		for (uint8_t j = 0; j < 3; ++j) {
			rx[i] -= (beta2[j] * _mu[i] + _ipX2X[j][i] - 2 * _ipXX[upperTriangularIndex(i, j)] * _beta[j])
					/ beta2[3 + j];
			rx2[i] -= (beta2[j] * _mu2[i] + _ipX2X2[upperTriangularIndex(i, j)] - 2 * _ipX2X[i][j] * _beta[j])
					/ beta2[3 + j];
		}
	}

	for (uint8_t i = 0; i < 3; ++i) {
		//Compute product of Jacobian matrix with the residual vector
		JtR[i] = 2 * (rx[i] - _beta[i] * r) / beta2[3 + i];
		JtR[3 + i] = 2 * (rx2[i] - 2 * _beta[i] * rx[i] + beta2[i] * r) / (beta2[3 + i] * _beta[3 + i]);

		//Now compute the product of the transpose of the jacobian with itself
		//Start with the diagonal blocks
		for (uint8_t j = i; j < 3; ++j) {
			JtJ[i][j] = JtJ[j][i] = 4
					* (_ipXX[upperTriangularIndex(i, j)] - _beta[i] * _mu[j] - _beta[j] * _mu[i]
							+ _beta[i] * _beta[j] * _observationCount) / (beta2[3 + i] * beta2[3 + j]);
			JtJ[3 + i][3 + j] = JtJ[3 + j][3 + i] = 4
					* (_ipX2X2[upperTriangularIndex(i, j)] - 2 * _beta[j] * _ipX2X[i][j] + beta2[j] * _mu2[i]
							- 2 * _beta[i] * _ipX2X[j][i] + 4 * _beta[i] * _beta[j] * _ipXX[upperTriangularIndex(i, j)]
							- 2 * _beta[i] * beta2[j] * _mu[i] + beta2[i] * _mu2[j] - 2 * beta2[i] * _beta[j] * _mu[j]
							+ beta2[i] * beta2[j] * _observationCount) / pow(_beta[3 + i] * _beta[3 + j], 3);
		}
		//then get the off diagonal blocks
		for (uint8_t j = 0; j < 3; ++j) {
			JtJ[i][3 + j] = JtJ[3 + j][i] = 4
					* (_ipX2X[j][i] - 2 * _beta[j] * _ipXX[upperTriangularIndex(i, j)] + beta2[j] * _mu[i]
							- _beta[i] * _mu2[j] + 2 * _beta[i] * _beta[j] * _mu[j] - _beta[i] * beta2[j] * _observationCount)
					/ (beta2[3 + i] * beta2[3 + j] * _beta[3 + j]);
		}
	}
}

void BestSphereGaussNewtonCalibrator::findDelta(float JtJ[][6], float JtR[]) {
//Solve 6-d matrix equation JtJS*x = JtR
//first put in upper triangular form
//Serial.println("find delta");
	float lambda;

//make upper triangular
	for (uint8_t i = 0; i < 6; ++i) {
		//eliminate all nonzero entries below JS[i][i]

		for (uint8_t j = i + 1; j < 6; ++j) {
			lambda = JtJ[j][i] / JtJ[i][i];
			if (lambda != 0.0) {
				JtR[j] -= lambda * JtR[i];
				for (uint8_t k = i; k < 6; ++k) {
					JtJ[j][k] -= lambda * JtJ[i][k];
				}
			}
		}
	}

//back-substitute
	for (int8_t i = 5; i >= 0; --i) {
		JtR[i] /= JtJ[i][i];
		JtJ[i][i] = 1.0;
		for (int8_t j = 0; j < i; ++j) {
			lambda = JtJ[j][i];
			JtR[j] -= lambda * JtR[i];
			JtJ[j][i] = 0.0;
		}
	}

}

void BestSphereGaussNewtonCalibrator::calculate() {
	guessParameters();
	float JtJ[6][6];
	float JtR[6];
	clearGNMatrices(JtJ, JtR);

	float eps = 0.000000001;
	int num_iterations = 20;
	float change = 100.0;
	while (--num_iterations >= 0 && change > eps) {

		computeGNMatrices(JtJ, JtR);
		findDelta(JtJ, JtR);

		change = JtR[0] * JtR[0] + JtR[1] * JtR[1] + JtR[2] * JtR[2] + JtR[3] * JtR[3] * (_beta[3] * _beta[3])
				+ JtR[4] * JtR[4] * (_beta[4] * _beta[4]) + JtR[5] * JtR[5] * (_beta[5] * _beta[5]);

		uint8_t i;
		if (!isnan(change)) {
			for (i = 0; i < 6; ++i) {
				_beta[i] -= JtR[i];
			}

			if (i >= 3) {
				_beta[i] = fabs(_beta[i]);
			}
		}

		clearGNMatrices(JtJ, JtR);
	}
}

/**
 * Write the offsets and scales to the sensor
 * @param	sensor		the sensor to write to (should be the same sensor that is pushing the sample
 */
void BestSphereGaussNewtonCalibrator::calibrateSensor(TripleAxisSensor &sensor) {
	calculate();

	sensor.setOffsets(_beta[0], _beta[1], _beta[2]);
	sensor.setScale(_beta[3], _beta[4], _beta[5]);
}

void BestSphereGaussNewtonCalibrator::guessParameters() {
	for (uint8_t i = 0; i < 3; ++i) {
		_beta[i] = ((float) (_obsMax[i] + _obsMin[i])) / 2.0;
		_beta[3 + i] = ((float) (_obsMax[i] - _obsMin[i])) / 2.0;
	}
}
} /* namespace mhvlib */
