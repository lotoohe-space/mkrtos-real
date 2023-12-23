/**
  ******************************************************************************
  * @file    I2S/I2S_TwoBoards/I2S_DataExchangePolling/main.h
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

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#if defined (USE_STM324xG_EVAL)
  #include "stm324xg_eval.h"

#elif defined (USE_STM324x7I_EVAL) 
  #include "stm324x7i_eval.h"

#else
 #error "Please select first the Evaluation board used in your application (in Project Options)"
#endif

/* Exported typedef -----------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Exported define ------------------------------------------------------------*/
/* Uncomment the line below if you will use the I2S peripheral as a Master */
#define I2S_MASTER 
/* Uncomment the line below if you will use the I2S peripheral as a Slave */
/* #define I2S_SLAVE */

/* I2Sx Communication boards Interface */
#define I2Sx                           SPI2
#define I2Sxext                        I2S2ext
#define I2Sx_CLK                       RCC_APB1Periph_SPI2
#define I2Sx_CLK_INIT                  RCC_APB1PeriphClockCmd

#define I2Sx_WS_PIN                    GPIO_Pin_12
#define I2Sx_WS_GPIO_PORT              GPIOB
#define I2Sx_WS_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define I2Sx_WS_SOURCE                 GPIO_PinSource12
#define I2Sx_WS_AF                     GPIO_AF_SPI2

#define I2Sx_CK_PIN                    GPIO_Pin_13
#define I2Sx_CK_GPIO_PORT              GPIOB
#define I2Sx_CK_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define I2Sx_CK_SOURCE                 GPIO_PinSource13
#define I2Sx_CK_AF                     GPIO_AF_SPI2

#define I2Sx_SD_PIN                    GPIO_Pin_15
#define I2Sx_SD_GPIO_PORT              GPIOB
#define I2Sx_SD_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define I2Sx_SD_SOURCE                 GPIO_PinSource15
#define I2Sx_SD_AF                     GPIO_AF_SPI2

#define I2Sxext_SD_PIN                 GPIO_Pin_2
#define I2Sxext_SD_GPIO_PORT           GPIOI
#define I2Sxext_SD_GPIO_CLK            RCC_AHB1Periph_GPIOI
#define I2Sxext_SD_SOURCE              GPIO_PinSource2
#define I2Sxext_SD_AF                  GPIO_AF_SPI3

#define TX_MASTER_BUFFERSIZE           (countof(aTxMasterBuffer) - 1)
#define RX_MASTER_BUFFERSIZE           TX_MASTER_BUFFERSIZE

#define TX_SLAVE_BUFFERSIZE           (countof(aTxSlaveBuffer) - 1)
#define RX_SLAVE_BUFFERSIZE           TX_SLAVE_BUFFERSIZE

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

