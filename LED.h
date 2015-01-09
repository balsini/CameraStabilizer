#ifndef __LED_H_
#define __LED_H_

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

#define LED_BLUE	0x8000
#define LED_RED		0x4000
#define LED_ORANGE	0x2000
#define LED_GREEN	0x1000

void LED_init();
void LED_on(uint16_t pin);
void LED_off(uint16_t pin);
void LED_toggle(uint16_t pin);

#endif
