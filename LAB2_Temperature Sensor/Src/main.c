/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
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
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "gpio.h"
#include "stm32f4xx_hal_rcc_ex.h"
#include "dma.h"

/* DEFINES -------------------------------------------------------------------*/
#define writePin HAL_GPIO_WritePin

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
#define allpins GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13

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

/* THRESHOLD DEFINES ---------------------------------------------------------*/
#define filterLength 50
#define tempthres 40




/* PROTOTYPES */
/*INITIALIZATION OF ADC AND GPIO*/
//void ADC_initialize(void);
void GPIO_initialize(void);

/*LED DISPLAY FUNCTIONS*/
void LED_display(float temp,uint8_t* counterPtr);
uint16_t number_display(int number);
void segClearAll(void);

/*FILTER*/
void shiftLeft(float* inArray,int size);
void generateCoeffs(int length);
float FIR_filter(float* inData,int size);

/*ALARM*/
uint8_t tempGuard(float temp);

void SystemClock_Config(void);


/*GLOBAL VARIABLES*/
//ADC_HandleTypeDef ADC_handle;
HAL_StatusTypeDef rc;
GPIO_TypeDef GPIO_handle;
uint32_t sensor_val;

/*LED ALARM VARIABLES*/
uint8_t segTracker=0;


/*TEMPERATURE VARIABLES*/
float temp_signal, temp_actual;
float heatData[filterLength];
float coeffs[filterLength];
float heatFiltered = 0;
float temp_f;
int tempMode = -1; //-1 -Celsius		1-Fareheit

/* TIMING VARIABLES */
uint16_t DoADC;
uint8_t segCounter =0;
float goodFIR(float* inData,float* inCoeffs);

int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();
	ADC_initialize();
	GPIO_initialize();

	/* ENABLE ADC CLOCK CYCLE */
	__HAL_RCC_ADC1_CLK_ENABLE();

  /* Infinite loop */
	generateCoeffs(filterLength);
	
	
  while (1)
  {
		/*IF HE TEMPERATURE IS ABOVE CERTAIN NUMBER (40C IN THISCASE), LED DANCE*/
		if(tempGuard(heatFiltered) ==1)
		{
			LEDdance();
		}
		/*IF BUTTON IS CLICKED, DISPLAY FARENHEIT*/
		if(HAL_GPIO_ReadPin(GPIOA,button) == GPIO_PIN_SET)
		{
			tempMode = 1;
		}
		else tempMode =-1;
		
		/*START ADC*/
		HAL_ADC_Start(&ADC_handle);
		if(DoADC > 10)
		{
			
			if(HAL_ADC_PollForConversion(&ADC_handle, 10000) != HAL_OK)
			{
				printf("Error: %d", rc);
			}
			/*DATA RECEIVED FROM SENSOR IS A LINEAR INTEGER REFERS VOLTAGE LEVEL*/
			sensor_val = HAL_ADC_GetValue(&ADC_handle);
			/*GET VOLTAGE SIGNAL FOR TEMPERATURE*/
			temp_signal = sensor_val*(3.6/4096);
			/*GET CELCIUS*/
			temp_actual = (temp_signal - 0.76)*400 + 25;
			//printf("%f \n", temp_actual);
			
			/*FOR FILTERING, MAKE AN ARRAY*/
			shiftLeft(heatData,filterLength);
			heatData[filterLength-1] = temp_actual;
			
			/*FILTER THE DATA*/
			heatFiltered = FIR_filter(heatData,filterLength);
			temp_f = heatFiltered * 1.8 + 32;
			
			DoADC = 0;
		}
		
		if(segCounter > 2)
		{
			if(tempMode ==-1)//CELSIUS
			{
				/*IF BUTTON IS NOT CLICKED, SEND CELSIUS DATA TO DISPLAY*/
				LED_display(heatFiltered,&segTracker);
			}
			/*FARENHEIT TO DISPLAY*/
			else LED_display(temp_f,&segTracker);	//FARENHEIT
			segCounter=0;
		}
		HAL_ADC_Stop(&ADC_handle);
	}
}
/**Board functions
*/


float FIR_filter(float* inData,int size)
{
	float accum=0;
	for(int i=0; i<size;i++)
	{
		accum = accum+ coeffs[i]*inData[i];
	}
	
	return accum;
}

uint8_t tempGuard(float temp)
{
	if(temp>tempthres)
		return 1;
	return 0;
}


uint16_t number_display(int number){
	switch(number){
		case 0: return s_zero;
		case 1: return s_one;
		case 2: return s_two;
		case 3: return s_three;
		case 4: return s_four;
		case 5: return s_five;
		case 6: return s_six;
		case 7: return s_seven;
		case 8: return s_eight;
		case 9: return s_nine;
	}
}

void shiftLeft(float* inArray,int size){
	for(int i=0; i<size-1; i++)
	{
		inArray[i] = inArray[i+1];
	}
}

void generateCoeffs(int length){
	float num = 1.0/(float)length;
	for(int i=0;i<length;i++)
	{
		coeffs[i] = num;
	}
	
	printf("coeffs are %f",num);
}

/* Initizliae ADC condition */


/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	/*FREQUENCY HAS BEEN CHANGED TO 100HZ FROM 1000HZ*/

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
