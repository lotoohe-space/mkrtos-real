/**
  ******************************************************************************
  * @file    I2C/I2C_IOExpander/main.h 
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
#include "stm324x9i_eval.h"
#include "stm324x9i_eval_lcd.h"
#include "stm324x9i_eval_ioe8.h"
#include "stm324x9i_eval_ioe16.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line below if you will use the Buttons in polling mode */
/* #define BUTTON_POLLING_MODE */
/* Uncomment the line below if you will use the Buttons in interrupt mode */
#define BUTTON_INTERRUPT_MODE     

/* Uncomment the line below if you will use the IOE in polling mode */
#define IOE_POLLING_MODE
/* Uncomment the line below if you will use the IOE in interrupt mode */
/* #define IOE_INTERRUPT_MODE */

#ifdef BUTTON_POLLING_MODE
  #define BUTTON_MODE  BUTTON_MODE_GPIO
#else
  #define BUTTON_MODE  BUTTON_MODE_EXTI
#endif 	/* BUTTON_POLLING_MODE */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nTime);

#endif /* __MAIN_H */

