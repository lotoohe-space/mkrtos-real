/**
  ******************************************************************************
  * @file    STM324x7i_eval_fmc_sram.h
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    27-January-2022
  * @brief   This file contains all the functions prototypes for the 
  *          STM324x7i_eval_fmc_sram.c driver.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM324x7I_EVAL_FMC_SRAM_H
#define __STM324x7I_EVAL_FMC_SRAM_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STM324x7i_eval.h"

/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup STM324x7I_EVAL
  * @{
  */
  
/** @addtogroup STM324x7I_EVAL_FMC_SRAM
  * @{
  */  

/** @defgroup STM324x7I_EVAL_FMC_SRAM_Exported_Types
  * @{
  */
/**
  * @}
  */
  
/** @defgroup STM324x7I_EVAL_FMC_SRAM_Exported_Constants
  * @{
  */ 
/**
  * @}
  */ 
  
/** @defgroup STM324x7I_EVAL_FMC_SRAM_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM324x7I_EVAL_FMC_SRAM_Exported_Functions
  * @{
  */      

void SRAM_Init(void);
void SRAM_WriteBuffer(uint16_t* pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite);
void SRAM_ReadBuffer(uint16_t* pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead);

#ifdef __cplusplus
}
#endif

#endif /* __STM324x7I_EVAL_FMC_SRAM_H */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */ 

/**
  * @}
  */ 


