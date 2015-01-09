//basic headers
#include "stm32f4xx.h"
#include <stdio.h>
#include "stm32f4xx_i2c.h"

#include "LED.h"
#include "UART.h"
#include "IMU.h"
#include "CONTROLLER.h"
#include "STEPPER.h"
#include "LPF.h"

#define SAMPLING_PERIOD 0.005

uint32_t SYSTICK_COUNTER_VALUE;
uint32_t SYSTICK_COUNTER_MULTIPLIER;

// delay function
void Soft_Delay(volatile unsigned int number) { while (number--) ; }

TM_MPU6050_t MPU6050_Data[2];
TM_MPU6050_t sensor_data_calibrated[2];
uint8_t IMU_active = 0;
double controller_signal[2];
char str[256];

double sensed_value_old[2] = {0.0, 0.0};
double sensed_value[2] = {0.0, 0.0};

CONTROLLER_t controller[2];

void SysTick_Handler()
{
	//double old_error = 0.0;
	//static uint32_t tmp_counter = 0;
	static uint32_t counter = 0;

	if ((counter = (counter + 1) % SYSTICK_COUNTER_MULTIPLIER) != 0)
		return;

	TM_MPU6050_ReadGyroscope(&MPU6050_Data[0]);
	TM_MPU6050_ReadGyroscope(&MPU6050_Data[1]);

	sensor_data_calibrated[0] = IMU_calibrated(MPU6050_Data[0]);
	sensor_data_calibrated[1] = IMU_calibrated(MPU6050_Data[1]);

	// Error DeadZone
	if (sensor_data_calibrated[0].Gyroscope_Z < 150 &&
			sensor_data_calibrated[0].Gyroscope_Z > -150)
		sensor_data_calibrated[0].Gyroscope_Z = 0;
	if (sensor_data_calibrated[1].Gyroscope_Z < 150 &&
			sensor_data_calibrated[1].Gyroscope_Z > -150)
		sensor_data_calibrated[1].Gyroscope_Z = 0;

	sensed_value[0] = LPF_go(0, sensor_data_calibrated[0].Gyroscope_Z);
	sensed_value[1] = LPF_go(1, sensor_data_calibrated[1].Gyroscope_Z);

	controller_signal[0] = CONTROLLER_out(&controller[0], sensed_value[0]);
	controller_signal[1] = CONTROLLER_out(&controller[1], sensed_value[1]);

	// Set unbuffered mode for stdout (newlib)
	setvbuf(stdout, 0, _IONBF, 0);

	/*
	 * World Real Speed
	 * Gimbal Sensed Speed
	 * Gimbal Filtered Speed
	 * Control Signal
	 */
	/*
	sprintf(str, "%d %d.0 %f %f\r\n",
			sensor_data_calibrated[0].Gyroscope_Z,
			sensor_data_calibrated[1].Gyroscope_Z,
			sensed_value[1],
			controller_signal[1]);
	*/
	sprintf(str, "%d %f %f %d %f %f\r\n",
			sensor_data_calibrated[0].Gyroscope_Z,
			sensed_value[0],
			controller_signal[0],
			sensor_data_calibrated[1].Gyroscope_Z,
			sensed_value[1],
			controller_signal[1]
	);
	printf(str);

	/*
	 * Rotates in one direction,
	 * waits,
	 * rotates in the other direction,
	 * waits,
	 * restarts from the beginning
	 */
	/*
	if ((tmp_counter % 200) == 0)
		STEPPER_go(0, 70);
	if ((tmp_counter % 400) == 0)
		STEPPER_go(0, 0);
	if ((tmp_counter % 600) == 0)
		STEPPER_go(0, -70);
	if ((tmp_counter % 800) == 0)
		STEPPER_go(0, 0);
	tmp_counter = (tmp_counter + 1) % 1000;
	 */

	STEPPER_go(0, controller_signal[0]);
	STEPPER_go(1, controller_signal[1]);
}

int main(void)
{
	//double i;

	RCC_HSEConfig(RCC_HSE_ON);
	while (!RCC_WaitForHSEStartUp()) ;

	// SAMPLING_PERIOD = SYSTICK_COUNTER_MULTIPLIER * SYSTICK_COUNTER_VALUE / SystemCoreClock
	// For a sampling time of 0.5s,
	// systick_counter = 0.5 * 168'000'000

	SYSTICK_COUNTER_MULTIPLIER = 1;

	SYSTICK_COUNTER_VALUE = (SAMPLING_PERIOD * SystemCoreClock) / SYSTICK_COUNTER_MULTIPLIER;

	while (SYSTICK_COUNTER_VALUE > 0xFFFFFF) {
		SYSTICK_COUNTER_MULTIPLIER *= 2;
		SYSTICK_COUNTER_VALUE /= 2;
	}

	CONTROLLER_init(&controller[0],
			0.015,				// Gain proportional
			0.085,				// Gain integral
			0.005,				// Gain integral 2
			1000,				// Saturation
			SAMPLING_PERIOD);
	CONTROLLER_init(&controller[1],
			0.015,				// Gain proportional
			0.1,				// Gain integral
			0.005,				// Gain integral 2
			1000,				// Saturation
			SAMPLING_PERIOD);

	LED_init();
	LPF_init(0);
	LPF_init(1);
	STEPPER_init();
	usart_init();

	printf("MPU6050 Init...\r\n");

	// Initialize MPU6050 sensor 0, address = 0xD0, AD0 pin on sensor is low
	if (TM_MPU6050_Init(&MPU6050_Data[0], TM_MPU6050_Device_0, TM_MPU6050_Accelerometer_16G, TM_MPU6050_Gyroscope_250s) == TM_MPU6050_Result_Ok
			&& TM_MPU6050_Init(&MPU6050_Data[1], TM_MPU6050_Device_1, TM_MPU6050_Accelerometer_16G, TM_MPU6050_Gyroscope_250s) == TM_MPU6050_Result_Ok) {
		/* Display message to user */
		printf("ACTIVE!\r\n");

		IMU_calibration(&MPU6050_Data[0]);
		IMU_calibration(&MPU6050_Data[1]);

		printf("MPU6050 sensors are calibrated!\r\n");

		/* Sensor 1 OK */
		IMU_active = 1;

		LED_on(LED_GREEN | LED_BLUE | LED_ORANGE | LED_RED);

		Soft_Delay(0xFFFFFF);

		LED_off(LED_GREEN | LED_BLUE | LED_ORANGE | LED_RED);

		SysTick_Config(SYSTICK_COUNTER_VALUE) ;
	} else {
		printf("FAIL!\r\n");
	}

	for (;;) ;
}
