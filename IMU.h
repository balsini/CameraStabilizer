#ifndef __IMU_H_
#define __IMU_H_

#include "defines.h"
#include "tm_stm32f4_i2c.h"
#include "tm_stm32f4_mpu6050.h"

#define IMU_CALIBRATION_SAMPLES 256

void IMU_calibration(TM_MPU6050_t * MPU6050_Data);
TM_MPU6050_t IMU_calibrated(TM_MPU6050_t val);

#endif
