#include "gpio.h"
#include "stm32f4xx.h"

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
#include "cmsis_os.h"
/* USER CODE BEGIN 0 */
/* SEGMENT NUMBER DEFINES ----------------------------------------------------*/
#define s_one 	GPIO_PIN_7|GPIO_PIN_8
#define s_two 	GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_12|GPIO_PIN_9|GPIO_PIN_10
#define s_three GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_12
#define s_four 	GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_12|GPIO_PIN_11
#define s_five 	GPIO_PIN_6|GPIO_PIN_12|GPIO_PIN_9|GPIO_PIN_11|GPIO_PIN_8
#define s_six		GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_12|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
#define s_seven	GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
#define s_eight GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12
#define s_nine 	GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_6|GPIO_PIN_12|GPIO_PIN_11
#define s_zero	GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_6
#define s_dot		GPIO_PIN_5
/* SEGMENT LED DEFINES -------------------------------------------------------*/
#define led1 GPIO_PIN_12
#define led2 GPIO_PIN_13
#define led3 GPIO_PIN_14
#define led4 GPIO_PIN_15
/* SEGMENT BUTTON DEFINES ----------------------------------------------------*/
#define button GPIO_PIN_0

/* SEGMENT DIGIT DEFINES -----------------------------------------------------*/
#define d_0 GPIO_PIN_0
#define d_1 GPIO_PIN_1
#define d_2 GPIO_PIN_2
#define d_3 GPIO_PIN_3

GPIO_InitTypeDef GPIOE_Pins;
GPIO_InitTypeDef GPIOD_Pins;
GPIO_InitTypeDef GPIOC_Pins;

uint8_t LEDtimer=0;
uint8_t LEDTracker=0;
uint8_t overtemp=0;

/*LED DISPLAY FUNCTIONS*/
void LED_display(float temp,uint8_t* counterPtr);
uint16_t number_display(int number);


void segClearAll(void);

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

void GPIO_initialize(void)
{

	/*EALBE GPIO CLOCKS*/
	__HAL_RCC_GPIOE_CLK_ENABLE();
//	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	/*ADDRESSES OF PINS O OARD FOR PRORAM*/
	GPIOE_Pins.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6
	| GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15;
	GPIOC_Pins.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6
	| GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15;
	
	GPIOE_Pins.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOE_Pins.Pull = GPIO_NOPULL;
	GPIOE_Pins.Speed = GPIO_SPEED_FREQ_MEDIUM;
	
	
	GPIOC_Pins.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOC_Pins.Pull = GPIO_NOPULL;
	GPIOC_Pins.Speed = GPIO_SPEED_FREQ_MEDIUM;
	
//	GPIOD_Pins.Pin = led1 | led2 | led3 | led4;
//	GPIOD_Pins.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIOD_Pins.Pull = GPIO_NOPULL;
//	GPIOD_Pins.Speed = GPIO_SPEED_FREQ_MEDIUM;
	
	HAL_GPIO_Init(GPIOE, &GPIOE_Pins);
//	HAL_GPIO_Init(GPIOD, &GPIOD_Pins);
	HAL_GPIO_Init(GPIOC, &GPIOC_Pins);
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

	
	if(overtemp ==1 && counter==0)
	{
		segClearAll();
		osDelay(500);
	}
		
	/*DISPLAY EACH DIGIT USING number_display function*/
	switch(counter)
	{
		case 0:	segClearAll();
						HAL_GPIO_WritePin(GPIOC,s_dot,GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOC,s_eight,GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOC,d_3,GPIO_PIN_SET);
						HAL_GPIO_WritePin(GPIOC,number_display(d3),GPIO_PIN_SET);
						break;

		case 1:	segClearAll();
						HAL_GPIO_WritePin(GPIOC,s_dot,GPIO_PIN_SET);
						HAL_GPIO_WritePin(GPIOC,s_eight,GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOC,d_2,GPIO_PIN_SET);
						HAL_GPIO_WritePin(GPIOC,number_display(d2),GPIO_PIN_SET);
						break;
	
		case 2: segClearAll();
						HAL_GPIO_WritePin(GPIOC,s_dot,GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOC,s_eight,GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOC,d_1,GPIO_PIN_SET);
						HAL_GPIO_WritePin(GPIOC,number_display(d1),GPIO_PIN_SET);
						break;

		case 3:	segClearAll();
						HAL_GPIO_WritePin(GPIOC,s_eight,GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOC,d_0,GPIO_PIN_SET);
						HAL_GPIO_WritePin(GPIOC,number_display(d0),GPIO_PIN_SET);
						HAL_GPIO_WritePin(GPIOC,s_dot,GPIO_PIN_RESET);
						break;
	}
	
						//segClearAll();
						//HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);
						//HAL_GPIO_WritePin(GPIOC,d_0,GPIO_PIN_SET);
						//HAL_GPIO_WritePin(GPIOC,s_eight,GPIO_PIN_RESET);
						//HAL_GPIO_WritePin(GPIOC,d_2,GPIO_PIN_SET);
						//HAL_GPIO_WritePin(GPIOC,number_display(d2),GPIO_PIN_SET);
	
	if(counter <3)
	{
		*counterPtr = counter+1;
	}
	else *counterPtr = 0;
}

void segClearAll(void){
	HAL_GPIO_WritePin(GPIOC, d_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, d_1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, d_2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, d_3, GPIO_PIN_RESET);
}
