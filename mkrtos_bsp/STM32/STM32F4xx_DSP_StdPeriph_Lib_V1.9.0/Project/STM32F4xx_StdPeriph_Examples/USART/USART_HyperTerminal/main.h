/**
  ******************************************************************************
  * @file    USART/USART_HyperTerminal/main.h
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

#if defined (USE_STM324xG_EVAL)
  #include "stm324xg_eval.h"

#elif defined (USE_STM324x7I_EVAL) 
  #include "stm324x7i_eval.h"

#elif defined (USE_STM324x9I_EVAL) 
  #include "stm324x9i_eval.h"

#else
 #error "Please select first the Evaluation board used in your application (in Project Options)"
#endif

/* Exported typedef ----------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
   
/* Definition for USARTx_IRQHANDLER *******************************************/
#if defined (USE_STM324xG_EVAL)
  #define USARTx_IRQHANDLER   USART3_IRQHandler
  
#endif /* USE_STM324xG_EVAL */
   
#if defined (USE_STM324x7I_EVAL)
  #define USARTx_IRQHANDLER   USART3_IRQHandler
  
#endif /* USE_STM324x7I_EVAL */
   
#if defined (USE_STM324x9I_EVAL)
  #define USARTx_IRQHANDLER   USART1_IRQHandler
  
#endif /* USE_STM324x9I_EVAL */
   
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

