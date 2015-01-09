#ifndef __CONTROLLER_H_
#define __CONTROLLER_H_

typedef struct CONTROLLER_t_ {
	double CONTROLLER_INTEGRAL_I;
	double CONTROLLER_INTEGRAL_I_old;
	double CONTROLLER_INTEGRAL_II;
	double CONTROLLER_GAIN_P;
	double CONTROLLER_GAIN_I;
	double CONTROLLER_GAIN_II;
	double CONTROLLER_SATURATION;
	double CONTROLLER_SAMPLING_TIME;
	double in_old;
} CONTROLLER_t;

void CONTROLLER_init(CONTROLLER_t * ctrl,
		double gain_P,
		double gain_I,
		double gain_II,
		double saturation,
		double sampling_time);
double CONTROLLER_out(CONTROLLER_t * ctrl,
		double in);

#endif
