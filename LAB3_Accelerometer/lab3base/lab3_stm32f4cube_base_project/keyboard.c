#include "keyboard.h"
#include "stm32f4xx.h"
#include "defines.h"

//global variables
GPIO_InitTypeDef kbPinsOut;
GPIO_InitTypeDef kbPinsIn;
uint32_t KB_counter =0;
uint32_t keyHeld=0;
uint8_t keyPressed =0;

uint8_t lastKey=100;
uint8_t currKey=100;
uint32_t delTimer=0;
uint32_t recurrTimer=0;
uint8_t numDigits=0;

uint8_t keybuffer[3];
uint8_t bufferCounter=0;
uint8_t phase=0;

extern uint32_t KB_counter;
extern uint32_t targetPitch;
extern uint32_t targetRoll;

/*Enables Keyboard*/
void enableKB()
	{
	__HAL_RCC_GPIOE_CLK_ENABLE(); //enable GPIO clock
		initKB();
	}
	
/*Initialize keyboard to scan first set of pins*/
void initKB(){
//setup output pins
	kbPinsOut.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
	kbPinsOut.Mode= GPIO_MODE_OUTPUT_OD;
	kbPinsOut.Pull= GPIO_PULLUP;
	kbPinsOut.Speed= GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOE,&kbPinsOut);
	
	//set all outputs to high
	HAL_GPIO_WritePin(GPIOE, kbPinsOut.Pin, GPIO_PIN_SET);
	
	//setup input pins
	kbPinsIn.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
	kbPinsIn.Mode= GPIO_MODE_INPUT;
	kbPinsIn.Pull= GPIO_PULLDOWN;
	kbPinsIn.Speed= GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOE,&kbPinsIn);
}

/*prints output to screen for debugging*/
void debugKBPins()
{
	uint16_t char1 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11);
	uint16_t char2 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12);
	uint16_t char3 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_13);
	uint16_t char4 = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_14);
	printf("%i, %i, %i, %i \n", char1,char2,char3,char4);
}

/*clears all keyboard pins set to high*/
void KB_ResetAll()
{
	HAL_GPIO_WritePin(GPIOE, kbPinsOut.Pin, GPIO_PIN_RESET);
}


uint8_t checkForKey()
{
	//Check for a keypress sequentially, columns first
	if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11)==1)
	{
		currKey= handleKeyPress(1);
	}
	else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12)==1)
	{
		currKey= handleKeyPress(2);
	}
	else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_13)==1)
	{
		currKey= handleKeyPress(3);
	}
	else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_14)==1)
	{
		currKey= handleKeyPress(4);
	}
	else currKey= (uint8_t)100; //this value indicates no button press detected
	
	
	/*Handling recurrent keys*/
	if( lastKey == currKey)
	{
		if(recurrTimer>KB_RECURR_TIME)
		{
			recurrTimer=0;
			lastKey=currKey;
		}
		else
		{
			recurrTimer++;
			currKey=200;
		}
	}
	else
	{
		lastKey = currKey;
	}
	
		/*Handling of delete case.*/
	if(currKey == 11)
	{
		if(delTimer>=KB_HOLD_TIME)
		{
			delTimer=0; //reset hold timer
			currKey=20; //delete all code
		}
		else 
		{
			delTimer++;
			currKey=11; //delete once
		}
	}
	else if(currKey !=11 && lastKey !=11)
		delTimer=0;
	
	return processInput(currKey);
}

uint8_t handleKeyPress(uint8_t coord1)
{
	
	uint8_t keyVal=100; //number on the keybad to be returned
	
	//reset all output pins for no anomolies
	KB_ResetAll(); 
	
	/*swap other row of pins to output */
	kbPinsOut.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
	kbPinsOut.Mode= GPIO_MODE_OUTPUT_OD;
	kbPinsOut.Pull= GPIO_PULLUP;
	kbPinsOut.Speed= GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOE,&kbPinsOut);
	
	//set all output pins high
	HAL_GPIO_WritePin(GPIOE, kbPinsOut.Pin, GPIO_PIN_SET);
	
	//setup new input pins
	kbPinsIn.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
	kbPinsIn.Mode= GPIO_MODE_INPUT;
	kbPinsIn.Pull= GPIO_PULLDOWN;
	kbPinsIn.Speed= GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOE,&kbPinsIn);
	
	//delay of two systick periods, any less breaks code!
	while(KB_counter<KB_SCANTIME+KB_SCANTIME_OFFSET){} 
		
	if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_7)==1)
	{
		switch(coord1){
			case 1: keyVal=1; break;
			case 2: keyVal=4; break;
			case 3: keyVal=7; break;
			case 4: keyVal=10; break;}
	}
	
	else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_8)==1)
	{
				switch(coord1){
			case 1: keyVal=2; break;
			case 2: keyVal=5;break;
			case 3: keyVal=8;break;
			case 4: keyVal=0;break;}
	}
	
		else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_9)==1)
	{
				switch(coord1){
			case 1: keyVal=3;break;
			case 2: keyVal=6;break;
			case 3: keyVal=9;break;
			case 4: keyVal=11;break;}
	}
	else keyVal = 99;
	/*reset input and output state to detect new keypress*/
	initKB();
	
	return keyVal; 
}

uint8_t processInput(uint8_t key)
{
	//handle delete key
	if(key == 11)
	{
		if(bufferCounter>0)
			bufferCounter--;
		else
		{
			bufferCounter=2;
		}
		printf("delete event. \n");
		keybuffer[2-bufferCounter] = 0;
		
		//recalculate number of inputted digits
		if(numDigits>0)
			numDigits--;
		else
			numDigits=0;
		
		if(keybuffer[0]==0 && keybuffer[1]==0 && keybuffer[2]==0)
		{
			bufferCounter=0;
		}
	}
	
	//handle delete all
	else if(key ==20)
	{
		printf("reset event. \n");
		bufferCounter=0;
		keybuffer[0]=0;
		keybuffer[1]=0;
		keybuffer[2]=0;
		numDigits=0;
		phase=0;
		targetPitch=0;
		targetRoll=0;
	}
	
	else if(key ==10)
	{
		printf("Enter event. \n");
		bufferCounter =0;
		
		if(phase==1)
			phase=0;
		else phase=1;
		
		return calcNum();

	}
	else if(key!=99 && key!=100 && key!=200)
	{
		//add key to buffer
		keybuffer[2-bufferCounter] = key;
		bufferCounter = (bufferCounter+1)%3;
		numDigits++;
		
		//reset buffer after done
		if(bufferCounter >=3)
		{
			bufferCounter=0;
			keybuffer[0]=0;
			keybuffer[1]=0;
			keybuffer[2]=0;
			numDigits=0;
		}
	}
	
	//printf("buffer: %i,%i,%i \n",keybuffer[0],keybuffer[1],keybuffer[2]);
	return 200;
}

//helper that calculates actual value
uint8_t calcNum()
{
	uint16_t num;
	num = keybuffer[0]*powten(10,(int)numDigits-3) + keybuffer[1]*powten(10,(int)numDigits-2) + keybuffer[2]*powten(10,(int)numDigits-1);
	//printf("NUM DIGITS IS: %i \n", numDigits);
	//printf("NUM DIGITS INT IS: %i \n", numDigits);
	//printf("buffer: %i,%i,%i \n",keybuffer[0],keybuffer[1],keybuffer[2]);
	//printf("num is as follows: %i = %i + %i + %i", num, keybuffer[0]*powten(10,(int)numDigits-3),keybuffer[1]*powten(10,(int)numDigits-2), keybuffer[2]*powten(10,(int)numDigits-1) );
	//reset now that calc is done
	printf("angle calculated: %i", num);
	keybuffer[0]=0;
	keybuffer[1]=0;
	keybuffer[2]=0;
	numDigits=0;
	if(num>180)
		return 200;
	else
		return num;
}

uint16_t powten(uint8_t num,int exp)
{
	uint16_t out = 1;
	for(uint8_t i=0; i<exp; i++)
	{
		out = out*num;
	}
	return out;
}