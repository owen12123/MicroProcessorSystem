/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
/* USER CODE BEGIN 0 */
/* SEGMENT NUMBER DEFINES ----------------------------------------------------*/
#define s_one 	GPIO_PIN_2|GPIO_PIN_3
#define s_two 	GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7|GPIO_PIN_5|GPIO_PIN_4
#define s_three GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_7
#define s_four 	GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_7|GPIO_PIN_6
#define s_five 	GPIO_PIN_1|GPIO_PIN_7|GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_3
#define s_six		GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_7|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
#define s_seven	GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
#define s_eight GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
#define s_nine 	GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_7|GPIO_PIN_6
#define s_zero	GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
#define s_dot		GPIO_PIN_13
/* SEGMENT LED DEFINES -------------------------------------------------------*/
#define led1 GPIO_PIN_12
#define led2 GPIO_PIN_13
#define led3 GPIO_PIN_14
#define led4 GPIO_PIN_15
/* SEGMENT BUTTON DEFINES ----------------------------------------------------*/
#define button GPIO_PIN_0

/* SEGMENT DIGIT DEFINES -----------------------------------------------------*/
#define d_0 GPIO_PIN_9
#define d_1 GPIO_PIN_11
#define d_2 GPIO_PIN_10
#define d_3 GPIO_PIN_12

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

GPIO_InitTypeDef GPIOE_Pins;
GPIO_InitTypeDef GPIOA_Pins;
GPIO_InitTypeDef GPIOD_Pins;

uint8_t LEDtimer=0;
uint8_t LEDTracker=0;

/*LED DISPLAY FUNCTIONS*/
void LED_display(float temp,uint8_t* counterPtr);
uint16_t number_display(int number);


void segClearAll(void);

void GPIO_initialize(void)
{

	/*EALBE GPIO CLOCKS*/
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	/*ADDRESSES OF PINS O OARD FOR PRORAM*/
	GPIOE_Pins.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6
	| GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
	
	GPIOE_Pins.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOE_Pins.Pull = GPIO_NOPULL;
	GPIOE_Pins.Speed = GPIO_SPEED_FREQ_MEDIUM;
	
	GPIOA_Pins.Pin = GPIO_PIN_0;
	GPIOA_Pins.Mode = GPIO_MODE_INPUT;
	GPIOA_Pins.Pull = GPIO_NOPULL;
	GPIOA_Pins.Speed = GPIO_SPEED_FREQ_MEDIUM;
	
	GPIOD_Pins.Pin = led1 | led2 | led3 | led4;
	GPIOD_Pins.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOD_Pins.Pull = GPIO_NOPULL;
	GPIOD_Pins.Speed = GPIO_SPEED_FREQ_MEDIUM;
	
	HAL_GPIO_Init(GPIOE, &GPIOE_Pins);
	HAL_GPIO_Init(GPIOA, &GPIOA_Pins);
	HAL_GPIO_Init(GPIOD, &GPIOD_Pins);
}

void LEDdance(void)
{
	if(LEDtimer>250)
	{
		switch(LEDTracker)
		{
			case 0:
				HAL_GPIO_WritePin(GPIOD,led1,GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOD,led2,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD,led3,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD,led4,GPIO_PIN_RESET);
				LEDTracker++;
				break;
			
			case 1:
				HAL_GPIO_WritePin(GPIOD,led2,GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOD,led1,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD,led3,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD,led4,GPIO_PIN_RESET);
				LEDTracker++;
				break;
			
			case 2:
				HAL_GPIO_WritePin(GPIOD,led3,GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOD,led2,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD,led1,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD,led4,GPIO_PIN_RESET);
				LEDTracker++;
				break;
			
			case 3:
				HAL_GPIO_WritePin(GPIOD,led4,GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOD,led2,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD,led3,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD,led1,GPIO_PIN_RESET);
				LEDTracker=0;
				break;
		}
		LEDtimer=0;
	}
}


void LED_display(float temp,uint8_t* counterPtr){
	int counter = (*counterPtr);
	int d0, d1, d2, d3, temperat;
	temp = temp*10;
	/*CONVERT FLOATING TYPE INTO INT FOR MOD*/
	temperat = (int)temp;
	/*USE MOD TO GET EACH DIGIT*/
	d3 = temperat % 10;
	temperat /= 10;
	d2 = temperat % 10;
	temperat /= 10;
	d1 = temperat % 10;
	temperat /= 10;
	d0 = temperat % 10;

	
	/*DISPLAY EACH DIGIT USING number_display function*/
	switch(counter)
	{
		case 0:	segClearAll();
						HAL_GPIO_WritePin(GPIOE,s_dot,GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOE,s_eight,GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOE,d_3,GPIO_PIN_SET);
						HAL_GPIO_WritePin(GPIOE,number_display(d3),GPIO_PIN_SET);
						break;

		case 1:	segClearAll();
						HAL_GPIO_WritePin(GPIOE,s_dot,GPIO_PIN_SET);
						HAL_GPIO_WritePin(GPIOE,s_eight,GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOE,d_2,GPIO_PIN_SET);
						HAL_GPIO_WritePin(GPIOE,number_display(d2),GPIO_PIN_SET);
						break;
	
		case 2: segClearAll();
						HAL_GPIO_WritePin(GPIOE,s_dot,GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOE,s_eight,GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOE,d_1,GPIO_PIN_SET);
						HAL_GPIO_WritePin(GPIOE,number_display(d1),GPIO_PIN_SET);
						break;

		case 3:	segClearAll();
						HAL_GPIO_WritePin(GPIOE,s_eight,GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOE,d_0,GPIO_PIN_SET);
						HAL_GPIO_WritePin(GPIOE,number_display(d0),GPIO_PIN_SET);
						HAL_GPIO_WritePin(GPIOE,s_dot,GPIO_PIN_RESET);
						break;
	}
	if(counter <3)
	{
		*counterPtr = counter+1;
	}
	else *counterPtr = 0;
}

void segClearAll(void){
	HAL_GPIO_WritePin(GPIOE, d_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE, d_1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE, d_2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE, d_3, GPIO_PIN_RESET);
}


/* USER CODE END 1 */

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
     PXX   ------> YYYYYY
		 
		 Continue your GPIO configuration information as abow.

*/

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
