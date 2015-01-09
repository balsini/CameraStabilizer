#include "LPF.h"

#define NZEROS 3
#define NPOLES 3

#define CIRCULAR_BUFFER_SIZE 4


double filter_coeff[] = {
		4.55360527e+03,
		1.00000000e+00,
		3.00000000e+00,
		3.00000000e+00,
		1.00000000e+00,
		7.77638560e-01,
		-2.52823122e+00,
		2.74883581e+00,
		-1.00000000e+00
};


typedef struct LPF_DATA_t_ {
	double xv[NZEROS+1];
	double yv[NPOLES+1];
	unsigned char head;
} LPF_DATA_t;

LPF_DATA_t lpf_d[2];

void LPF_init(unsigned char index)
{
	lpf_d[index].head = 0;
}

double LPF_go(unsigned char index, double in)
{
	register unsigned char i0 = lpf_d[index].head;
	register unsigned char i1 = (lpf_d[index].head + 1) % CIRCULAR_BUFFER_SIZE;
	register unsigned char i2 = (lpf_d[index].head + 2) % CIRCULAR_BUFFER_SIZE;
	register unsigned char i3 = (lpf_d[index].head + 3) % CIRCULAR_BUFFER_SIZE;

	lpf_d[index].head = (lpf_d[index].head + 1) % CIRCULAR_BUFFER_SIZE;

	lpf_d[index].xv[i3] = in / filter_coeff[0];
	lpf_d[index].yv[i3] =
			filter_coeff[1] * lpf_d[index].xv[i0] +
			filter_coeff[2] * lpf_d[index].xv[i1] +
			filter_coeff[3] * lpf_d[index].xv[i2] +
			filter_coeff[4] * lpf_d[index].xv[i3] +
			filter_coeff[5] * lpf_d[index].yv[i0] +
			filter_coeff[6] * lpf_d[index].yv[i1] +
			filter_coeff[7] * lpf_d[index].yv[i2];

	return lpf_d[index].yv[i3];
}
