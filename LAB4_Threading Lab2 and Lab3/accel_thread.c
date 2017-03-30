/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "supporting_functions.h"
#include "lis3dsh.h"
#include "accelerometer.h"
#include "gpio.h"
#include "timer.h"
#include "keyboard.h"
#include "defines.h"
#include "math.h"
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX

#define PI 3.14159265358979323846

/* Private variables ---------------------------------------------------------*/
float AccValues[3];
float UE_X = 50.2312;
float UE_Y = 19.2176;
float UE_Z = 55.2099;
float CX = 1.0418;
float CY = 1.0024;
float CZ = 0.9848;
float calib_axel[3];
float FIR_kernel[5] = {0.07189881319994462, 0.14379762639988902, 0.1797391907381051, 0.14379762639988902, 0.07189881319994462};
float FIR_inputx[5] = {0, 0, 0, 0, 0};
float FIR_inputy[5] = {0, 0, 0, 0, 0};
float FIR_inputz[5] = {0, 0, 0, 0, 0};
float current_axel_x;
float current_axel_y;
float current_axel_z;

int currentKeyVal;
uint8_t accelReadFlag = 0;
extern uint8_t keyPressed;
extern 	TIM_HandleTypeDef tim4handle;
extern uint32_t KB_counter;
extern uint8_t phase;
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config	(void);
void getCalibAccel(float* calibValues);
float getPitch();
float getRoll();
float rad2deg(float rad);
uint32_t judgeDuty(uint32_t target, float current); //calculates distance from target andgle and returns duty cycle
float filter(float* coeff, float* Input, float new_variable);

float currPitch;
float currRoll;
uint32_t targetPitch=0;
uint32_t targetRoll=0;
extern uint32_t period;


/**???????*/
extern void setLedIntensity(uint32_t duty, uint32_t channel);
extern void setLedIntensityPitch(uint32_t duty);
extern void setLedIntensityRoll(uint32_t duty);
extern void switchThread();

/** THREAD FUNCTIONALITY
*/

void accel_thread(void const *argument);               // thread function
osThreadId tid_Thread_accel;                              // thread id
osThreadDef(accel_thread, osPriorityNormal, 1, 0);

int start_Thread_accel (void) {

  tid_Thread_accel = osThreadCreate(osThread(accel_thread ), NULL); // Start LED_Thread
  if (!tid_Thread_accel) return(-1); 
  return(0);
}

void accel_thread(void const *argument)
{	
  /* MCU Configuration----------------------------------------------------------*/
	float LAMA[3];
	//int interrupt = 0;
  /* Initialize all configured peripherals */
	initPWM();
	//initTim3();
	Accel_Init();
	//enableKB();
	osDelay(100);

	while (1){
		//LIS3DSH_ReadACC(LAMA);
		
		//keyboard handling
		if(KB_counter>KB_SCANTIME)
		{
			currentKeyVal = checkForKey();
			KB_counter=0; //reset flag
			//printf("1\n");
			if(currentKeyVal ==250)
			{
				switchThread();
			}
			if(currentKeyVal<=180)
			{
				if(phase==0)
				{
					targetPitch=currentKeyVal;
				}
				else
				{
					targetRoll=currentKeyVal;
				}
			}
		}
		
		if(accelReadFlag == 1)
		{
			accelReadFlag=0;
			LIS3DSH_ReadACC(AccValues);
			getCalibAccel(calib_axel);
			calib_axel[0] = filter(FIR_kernel, FIR_inputx, calib_axel[0]);
			calib_axel[1] = filter(FIR_kernel, FIR_inputy, calib_axel[1]);
			calib_axel[2] = filter(FIR_kernel, FIR_inputz, calib_axel[2]);
			currPitch = rad2deg(getPitch());
			currRoll = rad2deg(getRoll());
			if(calib_axel[2]<=0)
			{
				currPitch=180.0-currPitch;
				currRoll=180.0-currRoll;
			}
			setLedIntensityPitch(judgeDuty(targetPitch, currPitch));
			setLedIntensityRoll(judgeDuty(targetRoll, currRoll));
		}
}
}

float filter(float* coeff, float* Input, float new_variable){
	Input[0] = Input[1];
	Input[1] = Input[2];
	Input[2] = Input[3];
	Input[3] = Input[4];
	Input[4] = new_variable;
	return coeff[0]*Input[0] + coeff[1]*Input[1] + coeff[2]*Input[2] + coeff[3]*Input[3] + coeff[4]*Input[4];
}

float getPitch()
{
	return atan(calib_axel[1]/(sqrt(calib_axel[0]*calib_axel[0]+calib_axel[2]*calib_axel[2])));
}

float getRoll()
{
	return atan(calib_axel[0]/(sqrt(calib_axel[1]*calib_axel[1]+calib_axel[2]*calib_axel[2])));
}

float rad2deg(float rad)
{
	return rad*180/PI;
}
void getCalibAccel(float* calibValues)
{
	calibValues[0] = (AccValues[0] - UE_X)*CX;
	calibValues[1] = (AccValues[1] + UE_Y)*CY;
	calibValues[2] = (AccValues[2] - UE_Z)*CZ;
}

uint32_t judgeDuty(uint32_t target, float current)
{
	float rval;
	rval = 100.0*((fabsf((float)target-(float)current))/180.0);
	return (uint32_t) rval;
}

