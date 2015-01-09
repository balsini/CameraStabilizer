#include "CONTROLLER.h"

void CONTROLLER_init(CONTROLLER_t * ctrl,
		double gain_P,
		double gain_I,
		double gain_II,
		double saturation,
		double sampling_time)
{
	ctrl->CONTROLLER_INTEGRAL_I = 0.0;
	ctrl->CONTROLLER_INTEGRAL_I_old = 0.0;
	ctrl->CONTROLLER_INTEGRAL_II = 0.0;
	ctrl->CONTROLLER_GAIN_P = gain_P;
	ctrl->CONTROLLER_GAIN_I = gain_I;
	ctrl->CONTROLLER_GAIN_II = gain_II;
	ctrl->CONTROLLER_SATURATION = saturation;
	ctrl->CONTROLLER_SAMPLING_TIME = sampling_time;
	ctrl->in_old = 0.0;
}

double CONTROLLER_out(CONTROLLER_t * ctrl, double in)
{
	register double ret;

	////////////////////////
	//  Integrals update  //
	////////////////////////

	// Integral
	// y(n) = y(n-1) + T * (x(n) + x(n-1)) / 2
	ctrl->CONTROLLER_INTEGRAL_I =					// y(n) =
			ctrl->CONTROLLER_INTEGRAL_I +			//   y(n-1) +
			ctrl->CONTROLLER_SAMPLING_TIME *		//   T *
			(in + ctrl->in_old) /					//   (x(n) + x(n-1)) /
			2.0;									//   2

	// Integral - Integral
	ctrl->CONTROLLER_INTEGRAL_II =
			ctrl->CONTROLLER_INTEGRAL_II +
			ctrl->CONTROLLER_SAMPLING_TIME *
			(ctrl->CONTROLLER_INTEGRAL_I + ctrl->CONTROLLER_INTEGRAL_I_old) /
			2.0;

	ctrl->in_old = in;
	ctrl->CONTROLLER_INTEGRAL_I_old = ctrl->CONTROLLER_INTEGRAL_I;

	/////////////////////////
	//  Controller Output  //
	/////////////////////////

	ret = ctrl->CONTROLLER_GAIN_P * in +								// Proportional
			ctrl->CONTROLLER_GAIN_I * ctrl->CONTROLLER_INTEGRAL_I +		// Integral
			ctrl->CONTROLLER_GAIN_II * ctrl->CONTROLLER_INTEGRAL_II;	// Integral - Integral

	// Saturation
	if (ret > ctrl->CONTROLLER_SATURATION)
		ret = ctrl->CONTROLLER_SATURATION;
	else if (ret < - ctrl->CONTROLLER_SATURATION)
		ret = - ctrl->CONTROLLER_SATURATION;

	return ret;
}
