#ifndef FIRFILTER_H
#define FIRFILTER_H

typedef struct FIR_State{
	float q;
	float r;
	float x;
	float p;
	float k;
}FIRState;

extern int FIRfilter_asm(float *OutputAray, float *Filterdata, float *InputArray, int length);

#endif