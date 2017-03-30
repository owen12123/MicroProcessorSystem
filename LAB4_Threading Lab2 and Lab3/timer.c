#include "timer.h"

TIM_HandleTypeDef tim4handle;
TIM_HandleTypeDef tim3handle;
TIM_OC_InitTypeDef pwmConf;
TIM_Base_InitTypeDef tim4Config;
uint32_t period = 42000;

/*
Initialize the PWM channels to control LED intensity
*/
void initPWM()
{
	//setting required enums
	tim4Config.Prescaler= (uint32_t)0;
	tim4Config.CounterMode=TIM_COUNTERMODE_UP;
	tim4Config.Period =period;
	
	tim4handle.Instance = TIM4;
	tim4handle.Init = tim4Config;
	tim4handle.Channel = HAL_TIM_ACTIVE_CHANNEL_1;
	
	pwmConf.OCMode = TIM_OCMODE_PWM1;
	pwmConf.Pulse = 0; //start off
	
	/*ENABLE CLOCKS */
	HAL_TIM_PWM_MspInit(&tim4handle);
	HAL_TIM_PWM_Init(&tim4handle);
	HAL_TIM_PWM_ConfigChannel(&tim4handle, &pwmConf, TIM_CHANNEL_1 );
	HAL_TIM_PWM_ConfigChannel(&tim4handle, &pwmConf, TIM_CHANNEL_2 );
	HAL_TIM_PWM_ConfigChannel(&tim4handle, &pwmConf, TIM_CHANNEL_3 );
	HAL_TIM_PWM_ConfigChannel(&tim4handle, &pwmConf, TIM_CHANNEL_4 );
	HAL_TIM_PWM_Start(&tim4handle, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&tim4handle, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&tim4handle, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&tim4handle, TIM_CHANNEL_4);
}

/*Initialize timer3
*/
void initTim3()
{
	//configure timer
	tim3handle.Instance = TIM3;
	tim3handle.Channel = HAL_TIM_ACTIVE_CHANNEL_1;
	tim3handle.Init.ClockDivision=0;
	tim3handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	tim3handle.Init.Prescaler=0;
	tim3handle.Init.Period=42000;
	
	//configure interrupt vector
	HAL_NVIC_SetPriority(TIM3_IRQn, 8,0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
	
	//enable clock
	HAL_TIM_Base_MspInit(&tim3handle);
	
	//start clock
	HAL_TIM_Base_Start_IT(&tim3handle);
	
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	__TIM3_CLK_ENABLE();
}

void setLedIntensity(uint32_t duty, uint32_t channel)
{
	pwmConf.Pulse=duty*tim4Config.Period/100;
	HAL_TIM_PWM_ConfigChannel(&tim4handle, &pwmConf, channel );
	HAL_TIM_PWM_Start(&tim4handle, TIM_CHANNEL_1);
}

//set LED intensity for the pitch LEDs
void setLedIntensityPitch(uint32_t duty)
{
	pwmConf.Pulse=duty*tim4Config.Period/100;
	HAL_TIM_PWM_ConfigChannel(&tim4handle, &pwmConf, TIM_CHANNEL_4 );
	HAL_TIM_PWM_ConfigChannel(&tim4handle, &pwmConf, TIM_CHANNEL_2 );
	HAL_TIM_PWM_Start(&tim4handle, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&tim4handle, TIM_CHANNEL_2);
}

//set LED intensity for the roll LEDs
void setLedIntensityRoll(uint32_t duty)
{
	pwmConf.Pulse=duty*tim4Config.Period/100;
	HAL_TIM_PWM_ConfigChannel(&tim4handle, &pwmConf, TIM_CHANNEL_3 );
	HAL_TIM_PWM_ConfigChannel(&tim4handle, &pwmConf, TIM_CHANNEL_1 );
	HAL_TIM_PWM_Start(&tim4handle, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&tim4handle, TIM_CHANNEL_1);
}

//Initialize low level resources for pwm functions
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim_pwm)
{
	//enable timer clock
	__TIM4_CLK_ENABLE();
	
	//GPIO CONFIG
	__GPIOD_CLK_ENABLE();
	GPIO_InitTypeDef dpins_Init;
	dpins_Init.Pin = GPIO_PIN_12 | GPIO_PIN_13 |GPIO_PIN_14 |GPIO_PIN_15;	//D12 is green LED
	dpins_Init.Mode= GPIO_MODE_AF_PP; //hopefully doesnt burn LEDs xD
	//dpins_Init.Pull= GPIO_NOPULL;
	dpins_Init.Speed= GPIO_SPEED_FREQ_MEDIUM;
	dpins_Init.Alternate = GPIO_AF2_TIM4;
	HAL_GPIO_Init(GPIOD,&dpins_Init);
}
