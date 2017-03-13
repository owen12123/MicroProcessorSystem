#ifndef __adc_H
#define __adc_H
#ifdef __cplusplus
	extern "C"{
#endif
		

#include "stm32f4xx_hal.h"

		
void ADC_initialize(void);
extern ADC_HandleTypeDef ADC_handle;



#ifdef __cplusplus
}

#endif
#endif