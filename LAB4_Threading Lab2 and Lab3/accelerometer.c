#include "accelerometer.h"
#include "lis3dsh.h"
#include "math.h"

#define pi ((float)(3.141592))
#define RAD2DEG(x) ((x) * ((float)(180.00/pi)))

static int ACCEL_NEW_DATA = 0;
/*1 = new data availiable, 0 = not availiable*/


/*Initialize Accelerometer*/
void Accel_Init(void){
	uint8_t InBuffer = 128;
	LIS3DSH_Write(&InBuffer, 35, 1);																/*Enable Interrupt*/
	
	LIS3DSH_InitTypeDef accel_init;
	LIS3DSH_DRYInterruptConfigTypeDef DI_config;
	
	accel_init.Power_Mode_Output_DataRate = LIS3DSH_DATARATE_25;		/*Power down or active mode with output data rate at 25kHz*/
	accel_init.Axes_Enable = LIS3DSH_XYZ_ENABLE;										/*Use xyz-axis for tilt data*/
	accel_init.Continous_Update = LIS3DSH_ContinousUpdate_Disabled;	/*Block Low/High registers of data until all data is read */
	accel_init.AA_Filter_BW = LIS3DSH_AA_BW_50; 										/* Use anti-aliasing filter BW 50 Hz*/
	accel_init.Full_Scale = LIS3DSH_FULLSCALE_2;										/*Use 2g for calubration*/
	accel_init.Self_Test = LIS3DSH_SELFTEST_NORMAL;									/*Do normal self test*/
	
	/*Initialize LIS3DSH*/
	LIS3DSH_Init(&accel_init);
	
	DI_config.Dataready_Interrupt = LIS3DSH_DATA_READY_INTERRUPT_ENABLED;	/*Enable data ready interrupt*/
	DI_config.Interrupt_signal = LIS3DSH_ACTIVE_HIGH_INTERRUPT_SIGNAL;		/*Interrupt Signal Active High */
	DI_config.Interrupt_type = LIS3DSH_INTERRUPT_REQUEST_PULSED;					/*Use Pulse interrupt type instead of latch*/
	
	/*LIS3DSH interrupt configuration*/
	LIS3DSH_DataReadyInterruptConfig(&DI_config);
	
	/*EXTI0 IRQ has priority*/
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);

	/*Enable IRQ for EXTI Line0 Interrupt*/
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

/*Read acceleromenter data in 3 axis*/
void Accel_Read(float *ix, float *iy, float *iz){
	float accel_comp[3];
	/*3-axis data*/
	
	LIS3DSH_ReadACC(accel_comp);
	*ix = accel_comp[0];
	*iy = accel_comp[1];
	*iz = accel_comp[2];
}

/*Calibrate the accelerometer*/
void Accel_Calib(float *ix, float *iy, float *iz){
	const float ACC[4][3] = {{0, 0, 0}, 
													{0, 0, 0}, 
													{0, 0, 0}, 
													{0, 0, 0}};
	
	float x, y, z;
	
	x = ACC[0][0]*(*ix) + ACC[1][0]*(*iy) + ACC[2][0]*(*iz) + ACC[3][0];
	y = ACC[0][1]*(*ix) + ACC[1][1]*(*iy) + ACC[2][1]*(*iz) + ACC[3][1];
	z = ACC[0][2]*(*ix) + ACC[1][2]*(*iy) + ACC[2][2]*(*iz) + ACC[3][2];
													
	*ix = x;
	*iy = y;
	*iz = z;
}

/*Reset the flag*/
void Accel_ClearData(void){
	ACCEL_NEW_DATA = 0;
}

/*Set flag when new data is availiable*/
void Accel_Callback(void){
	ACCEL_NEW_DATA = 1;
}

/*give whether new data is availiable*/
int Accel_NewData(void){
	return ACCEL_NEW_DATA;
}

/*Calculate tilt angle*/
float Accel_TiltAng(float ix, float iy, float iz){
	float angle;
	
	angle = RAD2DEG(atan2f(iy, sqrtf(ix*ix + iz*iz)));

	if(iz < 0)
		angle = ((float)180.0) - angle;
	if(angle < 0)
		angle += (float)360.0;
	return angle;
}



