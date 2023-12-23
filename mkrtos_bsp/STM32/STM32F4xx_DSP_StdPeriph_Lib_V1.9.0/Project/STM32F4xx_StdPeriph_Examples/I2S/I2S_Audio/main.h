/**
  ******************************************************************************
  * @file    I2S/I2S_Audio/main.h 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 0 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>

#if defined (USE_STM324xG_EVAL)
  #include "stm324xg_eval.h"
  #include "stm324xg_eval_lcd.h"
  #include "stm324xg_eval_audio_codec.h"

#elif defined (USE_STM324x7I_EVAL) 
  #include "stm324x7i_eval.h"
  #include "stm324x7i_eval_lcd.h"
  #include "stm324x7i_eval_audio_codec.h"

#else
 #error "Please select first the Evaluation board used in your application (in Project Options)"
#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* This is an audio file stored in the Flash memory as a constant table of 16-bit data.
   The audio format should be WAV (raw / PCM) 16-bits, Stereo (sampling rate may be modified) */
extern uint16_t AUDIO_SAMPLE[];

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nTime);

#endif /* __MAIN_H */

