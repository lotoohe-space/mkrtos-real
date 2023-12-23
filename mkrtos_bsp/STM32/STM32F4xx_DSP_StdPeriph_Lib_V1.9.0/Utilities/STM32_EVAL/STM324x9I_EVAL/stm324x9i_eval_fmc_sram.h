/**
  ******************************************************************************
  * @file    stm324x9i_eval_fmc_sram.h
  * @author  MCD Application Team
  * @version V1.0.5
  * @date    27-January-2022
  * @brief   This file contains all the functions prototypes for the 
  *          stm324x9i_eval_fmc_sram.c driver.
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
#ifndef __STM324X9I_EVAL_SRAM_H
#define __STM324X9I_EVAL_SRAM_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm324x9i_eval.h"

/** @defgroup STM324x9I_EVAL_FMC_SRAM_Private_Defines
  * @{
  */

/**
  * @brief  FMC SRAM bank address
  */   
#define SRAM_BANK_ADDR  ((uint32_t)0x64000000)  
  
/**
  * @brief  FMC SRAM Memory Width
  */  
/* #define SRAM_MEMORY_WIDTH    FMC_NORSRAM_MemoryDataWidth_8b  */
#define SRAM_MEMORY_WIDTH    FMC_NORSRAM_MemoryDataWidth_16b

/**
  * @brief  FMC SRAM Burst Access mode
  */  
#define SRAM_BURSTACCESS    FMC_BurstAccessMode_Disable  
/* #define SRAM_BURSTACCESS    FMC_BurstAccessMode_Enable*/

/**
  * @brief  FMC SRAM Write Burst feature
  */  
#define SRAM_WRITEBURST    FMC_WriteBurst_Disable  
/* #define SRAM_WRITEBURST   FMC_WriteBurst_Enable */

/**
  * @brief  FMC SRAM Continuous Clock feature
  */  
#define CONTINUOUSCLOCK_FEATURE    FMC_CClock_SyncOnly 
/* #define CONTINUOUSCLOCK_FEATURE     FMC_CClock_SyncAsync */

/**
  * @}
  */ 

/** @defgroup STM324x9I_EVAL_FMC_SDRAM_Exported_Functions
  * @{
  */ 
void  SRAM_Init(void);
void  SRAM_GPIOConfig(void);
void  SRAM_WriteBuffer(uint16_t* pBuffer, uint32_t uwWriteAddress, uint32_t uwBufferSize);
void  SRAM_ReadBuffer(uint16_t* pBuffer, uint32_t uwReadAddress, uint32_t uwBufferSize);

#ifdef __cplusplus
}
#endif

#endif /* __STM324X9I_EVAL_SRAM_H */

/**
  * @}
  */



