/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "gpio.h"
#include "stm32f4xx_hal_rcc_ex.h"
#include "cmsis_os.h" 
#include "keyboard.h"
#include "defines.h"

/* DEFINES -------------------------------------------------------------------*/
#define writePin HAL_GPIO_WritePin

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

/* THRESHOLD DEFINES ---------------------------------------------------------*/
#define filterLength 50
#define tempthres 40



/** THREADIFICATION
*/

void temp_thread (void const *argument);                 // thread function
osThreadId tid_Thread_temp;                              // thread id
osThreadDef(temp_thread, osPriorityNormal, 1, 0);

/*----------------------------------------------------------------------------
 *      Create the thread within RTOS context
 *---------------------------------------------------------------------------*/
int start_Thread_temp (void) {

  tid_Thread_temp = osThreadCreate(osThread(temp_thread ), NULL); // Start LED_Thread
  if (!tid_Thread_temp) return(-1); 
  return(0);
}

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
uint16_t overheatCounter;
float prevTemp;

/* TIMING VARIABLES */
uint16_t DoADC;
uint8_t segCounter =0;
float goodFIR(float* inData,float* inCoeffs);

//Keyboard handling
uint8_t tempKeyVal;
extern uint32_t KB_counter;
extern void 				switchThread();
extern uint8_t overtemp;

void temp_thread(void const *argument)
{

  /* Configure the system clock */
	ADC_initialize();
	GPIO_initialize();

	/* ENABLE ADC CLOCK CYCLE */
	__HAL_RCC_ADC1_CLK_ENABLE();

  /* Infinite loop */
	generateCoeffs(filterLength);
	
	
  while (1)
  {
		//keyboard handling
		if(KB_counter>KB_SCANTIME)
			{
				tempKeyVal = checkForKey();
				KB_counter=0; //reset flag
				
			if(tempKeyVal ==250)
			{
				switchThread();
			}
			}
		
		
		/*IF HE TEMPERATURE IS ABOVE CERTAIN NUMBER (40C IN THISCASE), LED DANCE*/
		overtemp = tempGuard(heatFiltered);
			
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
				//printf("Error: %d", rc);
			}
			/*DATA RECEIVED FROM SENSOR IS A LINEAR INTEGER REFERS VOLTAGE LEVEL*/
			sensor_val = HAL_ADC_GetValue(&ADC_handle);
			/*GET VOLTAGE SIGNAL FOR TEMPERATURE*/
			temp_signal = sensor_val*(3.0/4096);
			/*GET CELCIUS*/
			temp_actual = (temp_signal - 0.76)*400 + 25;
			//printf("%f , %f \n", temp_actual, temp_signal);
			
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
				
				if(overheatCounter<500)
				{
					LED_display(heatFiltered,&segTracker);
				}
				else if(overheatCounter<1000 && overheatCounter>500)
				{
					segClearAll();
				}
				else 
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
	{
		if(overheatCounter>1000)
		{
			overheatCounter=0;
		}

	}
	else overheatCounter=3000;
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
	
	//printf("coeffs are %f",num);
}



/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/