#ifndef MHV_PID_H_
#define MHV_PID_H_

#include <MHV_io.h>

class MHV_PID {
  protected:
	float	_kP;
    float	_kI;
    float	_kD;

	bool	_reverse;
	bool	_enabled;

    float	_setpoint;
	float	_integral;
	float	_lastInput;
	float	_lastOutput;

	float	outMin;
	float	outMax;

	void clampIntegral();

  public:
	MHV_PID(float setpoint, float kP, float kI, float kD, bool reverse,
			uint16_t min, uint16_t max);
	void setDirection(bool reverse);
	void enable(bool enable);
	float compute(float input);
	void setTuning(float kP, float kI, float kD);
    void setOutputLimits(float, float);
};
#endif

