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
#include "nes_main.h"

uint8 JoyPadCR; //$4016 joypad ¿ØÖÆ¼Ä´æÆ÷
uint8 JoyPadIndex_0, JoyPadIndex_1;
uint32 JoyPadValue_0, JoyPadValue_1;

void NES_JoyPadInit(void)
{
    JoyPadIndex_0 = 0;
	JoyPadValue_0 = 3 << 18;
    JoyPadIndex_1 = 0;
	JoyPadValue_1 = 3 << 18;
}

  /**
  * @}
  */


/*******************************END OF FILE***********************************/
