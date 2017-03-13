#ifndef __accelerometer_H
#define __accelerometer_H

void Accel_Init(void);
void Accel_Read(float *ix, float *iy, float*iz);
void Accel_Calib(float *ix, float *iy, float *iz);
float Accel_TiltAng(float ix, float iy, float iz);
void Accel_ClearData(void);
void Accel_Callback(void);
int Accel_NewData(void);
	
	
	
#endif