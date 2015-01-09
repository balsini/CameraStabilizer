#ifndef __UART_H_
#define __UART_H_

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"

void usart_init();
inline void putcUART(char c);
void printUART(char * msg);

#endif
