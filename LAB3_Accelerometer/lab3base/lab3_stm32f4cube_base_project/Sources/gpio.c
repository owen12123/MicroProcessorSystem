#include "gpio.h"
#include "stm32f4xx.h"

GPIO_InitTypeDef GPIOE_Pins;

void GPIO_Init(void){
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__GPIOE_CLK_ENABLE();
	
	GPIOE_Pins.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOE_Pins.Pull = GPIO_NOPULL;
	GPIOE_Pins.Speed = GPIO_SPEED_FREQ_MEDIUM;

	GPIO_InitTypeDef epins_Init;
	epins_Init.Pin = GPIO_PIN_0;
	epins_Init.Mode= GPIO_MODE_IT_RISING;
	epins_Init.Speed= GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOE,&epins_Init);
	
	HAL_NVIC_SetPriority(EXTI0_IRQn, 10,0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}
