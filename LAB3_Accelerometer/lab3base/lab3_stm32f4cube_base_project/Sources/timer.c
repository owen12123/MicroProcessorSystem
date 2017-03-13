#include "timer.h"

void initTimer()
{
	TIM_Base_InitTypeDef tim4Config;
	tim4Config.Prescaler= (uint32_t)0;
	tim4Config.CounterMode=TIM_COUNTERMODE_UP;
	tim4Config.Period =(uint32_t)1600;
	
	TIM_HandleTypeDef tim4handle;
	tim4handle.Instance = TIM4;
	tim4handle.Init = tim4Config;
	//tim4handle.Channel = HAL_TIM_ACTIVE_CHANNEL_1|HAL_TIM_ACTIVE_CHANNEL_2;
	
	TIM_OC_InitTypeDef pwmConf;
	pwmConf.OCMode = TIM_OCMODE_PWM1;
	pwmConf.Pulse = 800;
	
	/*ENABLE CLOCKS */
	HAL_TIM_PWM_MspInit(&tim4handle);
	HAL_TIM_PWM_Init(&tim4handle);
	HAL_TIM_PWM_ConfigChannel(&tim4handle, &pwmConf, TIM_CHANNEL_1 );
	HAL_TIM_PWM_ConfigChannel(&tim4handle, &pwmConf, TIM_CHANNEL_2 );
	
	HAL_TIM_PWM_Start(&tim4handle,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&tim4handle,TIM_CHANNEL_2);
	
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim_pwm)
{
	__TIM4_CLK_ENABLE();
	
	//GPIO CONFIG
	__GPIOD_CLK_ENABLE();
	GPIO_InitTypeDef dpins_Init;
	dpins_Init.Pin = GPIO_PIN_12 | GPIO_PIN_13;	//D12 is green LED
	dpins_Init.Mode= GPIO_MODE_AF_PP; //hopefully doesnt burn LEDs xD
	//dpins_Init.Pull= GPIO_NOPULL;
	dpins_Init.Speed= GPIO_SPEED_FREQ_MEDIUM;
	dpins_Init.Alternate = GPIO_AF2_TIM4;
	HAL_GPIO_Init(GPIOD,&dpins_Init);
	
}
