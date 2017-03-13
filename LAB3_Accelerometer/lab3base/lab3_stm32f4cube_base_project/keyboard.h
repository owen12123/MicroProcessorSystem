#ifndef __keyboard_H
#define __keyboard_H

#include "stm32f4xx.h"
//global variables


//function prototypes
void initKB(void);
void debugKBPins(void);
void KB_ResetAll(void);
uint8_t handleKeyPress(uint8_t coord1);
uint8_t checkForKey(void);
uint8_t processInput(uint8_t key);
void enableKB(void);
uint8_t calcNum();
uint16_t powten(uint8_t num,int exp);
	
#endif