/**
  ******************************************************************************
  * File Name          : ADC.c
  * Description        : This file provides code for the configuration
  *                      of the ADC instances.
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
#include "adc.h"

ADC_HandleTypeDef ADC_handle;
//ADC_HandleTypeDef ADC_handle;

/*IF ERROR OCCURS DURING ADC IS OPERAING, DISPLAY*/
void ADC_ERROR(uint16_t error_code){
	switch(error_code){
		case ADC_INIT_FAIL:
			printf("Error in initializing ADC");
			break;
		case ADC_CH_CONFIG_FAIL:
			printf("Error in ADC channel configuration");
			break;
		case ADC_MULTIMODE_FAIL:
			printf("Error in ADC multimode");
			break;
	}
}

void ADC_initialize(void){
	ADC_ChannelConfTypeDef channel_config;	//CHANNEL CONFIGURAION
	
	ADC_handle.Instance = ADC1;
	__ADC1_CLK_ENABLE();
		
	ADC_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
	/*ivision by 8 for prescalar*/
	ADC_handle.Init.Resolution = ADC_RESOLUTION_12B;
	/*12 bit for resolution*/
	ADC_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	ADC_handle.Init.ScanConvMode = DISABLE;
	ADC_handle.Init.ContinuousConvMode = DISABLE;
	ADC_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	ADC_handle.Init.DMAContinuousRequests = DISABLE;
	ADC_handle.Init.NbrOfConversion = 1;
	ADC_handle.Init.DiscontinuousConvMode = DISABLE;

	
	/*Initialize global ADC parameters*/
	if(HAL_ADC_Init(&ADC_handle) != HAL_OK)
		ADC_ERROR(ADC_INIT_FAIL);
	
	channel_config.Channel = ADC_CHANNEL_TEMPSENSOR;
	channel_config.Rank = 1;
	channel_config.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	channel_config.Offset = 0;
	
	if(HAL_ADC_ConfigChannel(&ADC_handle, &channel_config) != HAL_OK)
		ADC_ERROR(ADC_CH_CONFIG_FAIL);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
	__ADC1_CLK_ENABLE();
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
