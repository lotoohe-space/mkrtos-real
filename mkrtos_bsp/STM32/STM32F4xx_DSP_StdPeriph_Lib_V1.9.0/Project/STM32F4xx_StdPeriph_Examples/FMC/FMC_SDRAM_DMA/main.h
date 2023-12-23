/**
  ******************************************************************************
  * @file    FMC/FMC_SDRAM_DMA/main.h
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
#include "stm324x9i_eval_fmc_sdram.h"

/* Private defines -----------------------------------------------------------*/
/* DMA configurations for transfer with FMC */
#define DMA_STREAM               DMA2_Stream0
#define DMA_CHANNEL              DMA_Channel_0
#define DMA_STREAM_CLOCK         RCC_AHB1Periph_DMA2 
#define DMA_STREAM_IRQ           DMA2_Stream0_IRQn
#define DMA_IT_TCIF              DMA_IT_TCIF0
#define DMA_FLAG_TCIF            DMA_FLAG_TCIF0
#define DMA_STREAM_IRQHANDLER    DMA2_Stream0_IRQHandler

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

