#include "IMU.h"

TM_MPU6050_t TM_MPU6050_CALIB[2];

void IMU_delay(volatile uint32_t number)
{
	while (number--) ;
}

void IMU_calibration(TM_MPU6050_t * MPU6050_Data)
{
	unsigned int i;
	unsigned int index;

	double AX = 0.0,
			AY = 0.0,
			AZ = 0.0,
			GX = 0.0,
			GY = 0.0,
			GZ = 0.0;

	for (i=0; i<IMU_CALIBRATION_SAMPLES; ++i) {
		IMU_delay(0xFFFF);
		TM_MPU6050_ReadAll(MPU6050_Data);
		AX += MPU6050_Data->Accelerometer_X;
		AY += MPU6050_Data->Accelerometer_Y;
		AZ += MPU6050_Data->Accelerometer_Z;
		GX += MPU6050_Data->Gyroscope_X;
		GY += MPU6050_Data->Gyroscope_Y;
		GZ += MPU6050_Data->Gyroscope_Z;
	}

	if (MPU6050_Data->Address == 0xD0)
		index = 0;
	else if(MPU6050_Data->Address == 0xD2)
		index = 1;

	TM_MPU6050_CALIB[index].Accelerometer_X = AX / (double)IMU_CALIBRATION_SAMPLES;
	TM_MPU6050_CALIB[index].Accelerometer_Y = AY / (double)IMU_CALIBRATION_SAMPLES;
	TM_MPU6050_CALIB[index].Accelerometer_Z = AZ / (double)IMU_CALIBRATION_SAMPLES;
	TM_MPU6050_CALIB[index].Gyroscope_X = GX / (double)IMU_CALIBRATION_SAMPLES;
	TM_MPU6050_CALIB[index].Gyroscope_Y = GY / (double)IMU_CALIBRATION_SAMPLES;
	TM_MPU6050_CALIB[index].Gyroscope_Z = GZ / (double)IMU_CALIBRATION_SAMPLES;
}

TM_MPU6050_t IMU_calibrated(TM_MPU6050_t val)
{
	unsigned int index;
	TM_MPU6050_t ret;

	ret = val;

	if (val.Address == 0xD0)
		index = 0;
	else if(val.Address == 0xD2)
		index = 1;

	ret.Accelerometer_X -= TM_MPU6050_CALIB[index].Accelerometer_X;
	ret.Accelerometer_Y -= TM_MPU6050_CALIB[index].Accelerometer_Y;
	ret.Accelerometer_Z -= TM_MPU6050_CALIB[index].Accelerometer_Z;
	ret.Gyroscope_X -= TM_MPU6050_CALIB[index].Gyroscope_X;
	ret.Gyroscope_Y -= TM_MPU6050_CALIB[index].Gyroscope_Y;
	ret.Gyroscope_Z -= TM_MPU6050_CALIB[index].Gyroscope_Z;

	return ret;
}
