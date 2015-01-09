#ifndef __STEPPER_H_
#define __STEPPER_H_

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"

void STEPPER_init();
void STEPPER_go(uint8_t stepper, double error);

#endif
