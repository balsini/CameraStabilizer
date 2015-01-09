#include "LED.h"

void LED_init()
{
	//Port setup
	GPIO_InitTypeDef ledinit;

	//Peripherial clock setup
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	ledinit.GPIO_Mode = GPIO_Mode_OUT;
	ledinit.GPIO_OType = GPIO_OType_PP;
	ledinit.GPIO_PuPd = GPIO_PuPd_NOPULL;
	ledinit.GPIO_Speed = GPIO_Speed_2MHz;
	ledinit.GPIO_Pin = LED_BLUE | LED_GREEN | LED_ORANGE | LED_RED;

	GPIO_Init(GPIOD, &ledinit);
}

void LED_on(uint16_t pin)
{
	GPIO_SetBits(GPIOD, pin);
}

void LED_off(uint16_t pin)
{
	GPIO_ResetBits(GPIOD, pin);
}

void LED_toggle(uint16_t pin)
{
	GPIO_ToggleBits(GPIOD, pin);
}
