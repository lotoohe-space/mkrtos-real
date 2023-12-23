/**
  ******************************************************************************
  * @file    CRYP/CRYP_AESECBmode/main.h 
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
#include "stm32f4xx.h"
#include <stdio.h>

#if defined (USE_STM324xG_EVAL)
  #include "stm324xg_eval.h"

#elif defined (USE_STM324x7I_EVAL) 
  #include "stm324x7i_eval.h"

#elif defined (USE_STM324x9I_EVAL) 
  #include "stm324x9i_eval.h"

#else
 #error "Please select first the Evaluation board used in your application (in Project Options)"
#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* The size of the plain text in word (PlainData is declared as unsigned word). It must be a multiple of 4.
   AES is a block cipher algorithm. The plain text must be multiple of 128 bits = 16 bytes = 4 words */
#define DATA_SIZE              ((uint32_t)8)

#define CRYP_DIN_REG_ADDR      ((uint32_t)0x50060008)  /* Crypto DIN register address */
#define CRYP_DOUT_REG_ADDR     ((uint32_t)0x5006000C)  /* Crypto DOUT register address */

/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

