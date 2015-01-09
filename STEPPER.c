#include "STEPPER.h"

#include "LED.h"

#define DEAD_ZONE 10.0
#define TIMER_MAX_PERIOD 32752.0

void STEPPER_delay(volatile uint32_t number)
{
	while (number--) ;
}

/**
 * @brief	Setup the GPIOx port as out of PWM on selected Pin.
 * @param	GPIOx: where x can be (A..I) to select the GPIO peripheral.
 * @param	Pin: specifies the port bit to be initialized for out of PWM signal.
 * 			Refer STM32F407xx Datasheet for Alternate function mapping (Table 9)
 * @retval None
 */
void InitializeGPIO(GPIO_TypeDef* GPIOx, uint32_t Pin)
{
	GPIO_InitTypeDef sg;

	sg.GPIO_Pin   = Pin;
	sg.GPIO_Mode  = GPIO_Mode_AF;		// PWM is Alternate function Mode
	sg.GPIO_Speed = GPIO_Speed_100MHz;
	sg.GPIO_OType = GPIO_OType_PP;
	sg.GPIO_PuPd  = GPIO_PuPd_DOWN;

	GPIO_Init(GPIOx, &sg);
}

/**
 * @brief  Initialize PWM channel 4 of TIMx
 * @param	TIMx: where x can be  1 to 14 to select the TIM peripheral.
 * @param  ValueToOn: the value of TIMx starting from which PWM out will be 1
 * @retval None
 */
void InitializePWMChannel4(TIM_TypeDef* TIMx, uint32_t ValueToOn)
{
	TIM_OCInitTypeDef sOC;

	sOC.TIM_OCMode = TIM_OCMode_PWM1;
	sOC.TIM_Pulse = ValueToOn;
	sOC.TIM_OutputState = TIM_OutputState_Enable;
	sOC.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC4Init(TIMx, &sOC);
	TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);
}

/**
 * @brief  Initialize PWM channel 1 of TIMx
 * @param	TIMx: where x can be  1 to 14 to select the TIM peripheral.
 * @param  ValueToOn: the value of TIMx starting from which PWM out will be 1
 * @retval None
 */
void InitializePWMChannel2(TIM_TypeDef* TIMx, uint32_t ValueToOn)
{
	TIM_OCInitTypeDef sOC;

	sOC.TIM_OCMode = TIM_OCMode_PWM1;
	sOC.TIM_Pulse = ValueToOn;
	sOC.TIM_OutputState = TIM_OutputState_Enable;
	sOC.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC2Init(TIMx, &sOC);
	TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);
}

/**
 * @brief  Initialize timer TIMx as counter with the specified Period and Prescaler.
 * @param	TIMx: where x can be  1 to 14 to select the TIM peripheral.
 * @param  Period: the length of period will be n+1 because the counting begin from 0
 * @param  PrescaleDiv: Divides the clock at PrescaleDiv+1.
 * 			If PrescaleDiv=0 then SrcClock not changed.
 * @retval None
 */
void InitializeTimer(TIM_TypeDef* TIMx, uint16_t PrescaleDiv, uint16_t Period)
{
	TIM_TimeBaseInitTypeDef sT;

	sT.TIM_Prescaler   = PrescaleDiv;
	sT.TIM_CounterMode = TIM_CounterMode_Up;// Count to up
	sT.TIM_Period      = Period;
	sT.TIM_ClockDivision = TIM_CKD_DIV1;	// No pre-division
	sT.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIMx, &sT);

	TIM_Cmd(TIMx, ENABLE);
}

void STEPPER_init()
{
	/////////////
	// MOTOR 1 //
	/////////////

	// Initialize GPIO
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	// GPIOD Periph clock enable
	InitializeGPIO(GPIOB, GPIO_Pin_11 | GPIO_Pin_7);

	// Initialize TIM4
	// prescaler*period = 168*1000*1000 (for SysClk = 168 MHz clock of PWM will be 1 Hz)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 	// TIM4 Periph clock enable
	InitializeTimer(TIM2, 10000, 0);

	// Initialize PWM Chanel4
	// 50% duty factor = 1400
	InitializePWMChannel4(TIM2, 1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_TIM2);

	/////////////
	// MOTOR 2 //
	/////////////

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	InitializeTimer(TIM4, 10000, 0);

	InitializePWMChannel2(TIM4, 1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4);

	////////////////
	// DIRECTIONS //
	////////////////

	//Port setup
	GPIO_InitTypeDef motordirection;

	//Peripherial clock setup
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	motordirection.GPIO_Mode = GPIO_Mode_OUT;
	motordirection.GPIO_OType = GPIO_OType_PP;
	motordirection.GPIO_PuPd = GPIO_PuPd_UP;
	motordirection.GPIO_Speed = GPIO_Speed_2MHz;
	motordirection.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;

	GPIO_Init(GPIOE, &motordirection);
}

void STEPPER_go(uint8_t stepper, double error)
{
	register uint32_t period;

	switch (stepper) {
	case 1: // STEPPER 1
		if (error >= 0) { // Positive error: clockwise
			LED_off(LED_RED);
			LED_on(LED_GREEN);
			GPIO_ResetBits(GPIOE, GPIO_Pin_8);
		} else {
			error = -error;
			LED_off(LED_GREEN);
			LED_on(LED_RED);
			GPIO_SetBits(GPIOE, GPIO_Pin_8);
		}

		if (error >= DEAD_ZONE) {
			period = TIMER_MAX_PERIOD / error;
			if (period == 0)
				period = 1;
			TIM_SetAutoreload(TIM4, period);
			if (TIM_GetCounter(TIM4) > period)
				TIM_SetCounter(TIM4, period);
		} else {
			LED_off(LED_RED);
			LED_off(LED_GREEN);
			TIM_SetAutoreload(TIM4, 0);
		}
		break;
	case 0:
		// STEPPER 0
		if (error < 0) {
			error = -error;
			LED_off(LED_BLUE);
			LED_on(LED_ORANGE);
			GPIO_ResetBits(GPIOE, GPIO_Pin_7);
		} else {
			LED_off(LED_ORANGE);
			LED_on(LED_BLUE);
			GPIO_SetBits(GPIOE, GPIO_Pin_7);
		}

		if (error > DEAD_ZONE) {
			period = TIMER_MAX_PERIOD / error;
			if (period == 0)
				period = 1;
			TIM_SetAutoreload(TIM2, period);
			if (TIM_GetCounter(TIM2) > period)
				TIM_SetCounter(TIM2, period);
		} else {
			LED_off(LED_ORANGE);
			LED_off(LED_BLUE);
			TIM_SetAutoreload(TIM2, 0);
		}
		break;
	default: break;
	}
}
