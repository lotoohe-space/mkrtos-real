/**
  ******************************************************************************
  * @file    
  * @author  
  * @version 
  * @date    
  * @brief  
  ******************************************************************************
  * @attention
  *
  * 
  ******************************************************************************
  */  
#ifndef _JOYPAD_H_
#define _JOYPAD_H_
/* Includes ------------------------------------------------------------------*/
#include "nes_main.h"

/* define ------------------------------------------------------------------*/
#define JOYPAD_0 	0
#define JOYPAD_1 	1

/* Private typedef -----------------------------------------------------------*/
typedef enum {JOYPADDISABLE = 0, JOYPADENABLE = !JOYPADDISABLE} JoyPadState;

typedef struct{
	JoyPadState state;
	uint8_t  index;	//��ǰ��ȡλ
	uint32_t value;	//JoyPad ��ǰֵ	
}JoyPadType;

/* function ------------------------------------------------------------------*/
void NES_JoyPadInit(void);
void NES_JoyPadReset(void);
void NES_JoyPadDisable(void);
uint8_t NES_GetJoyPadVlaue(int JoyPadNum);
void NES_SetJoyPadValue(int JoyPadNum, int index, int status);

#endif /* _JOYPAD_H_*/
  /**
  * @}
  */


/*******************************END OF FILE***********************************/
