/**
  ******************************************************************************
  * @file    CortexM/MPU/stm32_mpu.c 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Access rights configuration using Cortex-M3 MPU regions.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32_mpu.h"

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup CortexM4_MPU
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ARRAY_ADDRESS_START    (0x20002000UL)
#define ARRAY_SIZE             (0x09UL << 0UL)
#define ARRAY_REGION_NUMBER    (0x03UL << MPU_RNR_REGION_Pos) 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if defined ( __CC_ARM   )
 uint8_t PrivilegedReadOnlyArray[32] __attribute__((at(0x20002000)));

#elif defined ( __ICCARM__ )
 #pragma location=0x20002000
 __no_init uint8_t PrivilegedReadOnlyArray[32];

#elif defined   (  __GNUC__  )
 uint8_t PrivilegedReadOnlyArray[32] __attribute__((section(".ROarraySection")));
#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Configures the main MPU regions.
  * @param  None
  * @retval None
  */
void MPU_Config(void)
{
  /* Disable MPU */
  MPU->CTRL &= ~MPU_CTRL_ENABLE_Msk;
  
  /* Configure RAM region as Region N°0, 8kB of size and R/W region */
  MPU->RNR  = RAM_REGION_NUMBER;
  MPU->RBAR = RAM_ADDRESS_START;
  MPU->RASR = RAM_SIZE | portMPU_REGION_READ_WRITE;
  
  /* Configure FLASH region as REGION N°1, 1MB of size and R/W region */
  MPU->RNR  = FLASH_REGION_NUMBER;
  MPU->RBAR = FLASH_ADDRESS_START;
  MPU->RASR = FLASH_SIZE | portMPU_REGION_READ_WRITE;
  
  /* Configure Peripheral region as REGION N°2, 0.5GB of size, R/W and Execute
  Never region */
  MPU->RNR  = PERIPH_REGION_NUMBER;  
  MPU->RBAR = PERIPH_ADDRESS_START;
  MPU->RASR = PERIPH_SIZE |portMPU_REGION_READ_WRITE | MPU_RASR_XN_Msk;
  
  /* Enable the memory fault exception */
  SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
   
  /* Enable MPU */
  MPU->CTRL |= MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_ENABLE_Msk;
}

/**
  * @brief  This function configure the access right using Cortex-M3 MPU regions.
  * @param  None
  * @retval None
  */
void MPU_AccessPermConfig(void)
{ 
  /* Configure region for PrivilegedReadOnlyArray as REGION N°3, 32byte and R 
     only in privileged mode */
  MPU->RNR  = ARRAY_REGION_NUMBER;
  MPU->RBAR |= ARRAY_ADDRESS_START;
  MPU->RASR |= ARRAY_SIZE | portMPU_REGION_PRIVILEGED_READ_ONLY;
  
  /* Read from PrivilegedReadOnlyArray. This will not generate error */
  (void)PrivilegedReadOnlyArray[0];
  
  /* Uncomment the following line to write to PrivilegedReadOnlyArray. This will
     generate error */
  /* PrivilegedReadOnlyArray[0] = 'e'; */
}

/**
  * @}
  */

/**
  * @}
  */

