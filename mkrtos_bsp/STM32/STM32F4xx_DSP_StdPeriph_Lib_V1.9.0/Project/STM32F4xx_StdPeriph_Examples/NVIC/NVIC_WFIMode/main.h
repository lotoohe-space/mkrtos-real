/**
  ******************************************************************************
  * @file    NVIC/NVIC_WFIMode/main.h 
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
#ifdef USE_STM324x9I_EVAL 
 #define BUTTON             BUTTON_TAMPER
 #define BUTTON_EXTI_LINE   TAMPER_BUTTON_EXTI_LINE
 #define RCC_AHB1Periph_DMA RCC_AHB1Periph_DMA2
 #define USART_DR_ADDRESS   ((uint32_t)0x40011004)
 #define DMA_Stream         DMA2_Stream5
 #define DMA_Stream_IRQn    DMA2_Stream5_IRQn

#else
 #define BUTTON             BUTTON_KEY
 #define BUTTON_EXTI_LINE   KEY_BUTTON_EXTI_LINE
 #define RCC_AHB1Periph_DMA RCC_AHB1Periph_DMA1
 #define USART_DR_ADDRESS   ((uint32_t)0x40004804)
 #define DMA_Stream         DMA1_Stream1
 #define DMA_Stream_IRQn    DMA1_Stream1_IRQn 
#endif /* USE_STM324x9I_EVAL */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint8_t Buffercmp16(uint16_t *pBuffer1, uint16_t *pBuffer2, uint16_t BufferLength);

#endif /* __MAIN_H */

