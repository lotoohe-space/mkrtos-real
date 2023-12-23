/**
  ******************************************************************************
  * @file    CEC/CEC_DataExchange/main.h 
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
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define CEC_MAX_PAYLOAD                  16

/* Uncomment the line below if you use the CEC peripheral as a Device 1 */
//#define DEVICE_1   
/* Uncomment the line below if you use the CEC peripheral as a Device 2 */ 
#define DEVICE_2 
#define DEVICE_ADDRESS_1               0x01  /* CEC device 1 address   */
#define DEVICE_ADDRESS_2               0x03  /* CEC device 2 address   */

#define CEC_INITIATOR_LSB_POS         ((uint32_t) 4)
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

