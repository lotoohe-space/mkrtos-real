/**
  ******************************************************************************
  * @file    CortexM/MPU/stm32_mpu.h 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Header for stm32_mpu.c module
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
#ifndef __STM32_MPU_H
#define __STM32_MPU_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

#define RAM_ADDRESS_START                        (0x20000000UL)
#define RAM_SIZE                                 (0x19UL << 0UL)
#define PERIPH_ADDRESS_START                     (0x40000000)
#define PERIPH_SIZE                              (0x39UL << 0UL)
#define FLASH_ADDRESS_START                      (0x08000000)
#define FLASH_SIZE                               (0x27UL << 0UL)
#define portMPU_REGION_READ_WRITE                (0x03UL << MPU_RASR_AP_Pos)
#define portMPU_REGION_PRIVILEGED_READ_ONLY      (0x05UL << MPU_RASR_AP_Pos)
#define portMPU_REGION_READ_ONLY                 (0x06UL << MPU_RASR_AP_Pos)
#define portMPU_REGION_PRIVILEGED_READ_WRITE     (0x01UL << MPU_RASR_AP_Pos)
#define RAM_REGION_NUMBER                        (0x00UL << MPU_RNR_REGION_Pos)
#define FLASH_REGION_NUMBER                      (0x01UL << MPU_RNR_REGION_Pos)
#define PERIPH_REGION_NUMBER                     (0x02UL << MPU_RNR_REGION_Pos)


/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void MPU_Config(void);
void MPU_AccessPermConfig(void);

#endif /* __STM32_MPU_H */

