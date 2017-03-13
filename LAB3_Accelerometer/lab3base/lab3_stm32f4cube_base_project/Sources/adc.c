#include "adc.h"

ADC_HandleTypeDef ADC_handle;

void ADC_initialize(void){
	ADC_ChannelConfTypeDef channel_config;	//Channel configuration
	
	ADC_handle.Instance = ADC1;
	
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
	//do not need at all
	ADC_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	ADC_handle.Init.ExternalTrigConvEdge = ADC_SOFTWARE_START;
	
	
	/*Initialize global ADC parameters*/
	if(HAL_ADC_Init(&ADC_handle) != HAL_OK){
		printf("Error occured during initialization");
	}
	
	channel_config.Channel = ADC_CHANNEL_16;
	channel_config.Rank = 1;
	channel_config.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	channel_config.Offset = 0;
	
	if(HAL_ADC_ConfigChannel(&ADC_handle, &channel_config) != HAL_OK)
		ADC_ERROR(ADC_CH_CONFIG_FAIL);
}
