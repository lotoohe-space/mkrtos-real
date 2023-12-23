/**
  ******************************************************************************
  * @file    ADC/ADC_DualModeInterleaved/main.h 
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
  #include "stm324xg_eval_lcd.h"

#elif defined (USE_STM324x7I_EVAL) 
  #include "stm324x7i_eval.h"
  #include "stm324x7i_eval_lcd.h"

#elif defined (USE_STM324x9I_EVAL) 
  #include "stm324x9i_eval.h"
  #include "stm324x9i_eval_lcd.h"

#else
 #error "Please select first the Evaluation board used in your application (in Project Options)"
#endif

/* Private define ------------------------------------------------------------*/
/* Used to display the ADC converted value on LCD */
#define USE_LCD
  /* If you are not using the LCD, you can monitor the converted values by adding
     the variable "uhADCDualConvertedValue" to the debugger watch window */
     
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#if defined (USE_STM324xG_EVAL)
  #define ADC_CHANNEL              ADC_Channel_12
  #define ADC1_2_CHANNEL_GPIO_CLK  RCC_AHB1Periph_GPIOC
  #define GPIO_PIN                 GPIO_Pin_2
  #define GPIO_PORT                GPIOC  
  #define DMA_CHANNELx             DMA_Channel_0
  #define DMA_STREAMx              DMA2_Stream0
  #define ADC_CDR_ADDRESS          ((uint32_t)0x40012308)

#elif defined (USE_STM324x7I_EVAL)
  #define ADC_CHANNEL              ADC_Channel_12
  #define ADC1_2_CHANNEL_GPIO_CLK  RCC_AHB1Periph_GPIOC
  #define GPIO_PIN                 GPIO_Pin_2
  #define GPIO_PORT                GPIOC
  #define DMA_CHANNELx             DMA_Channel_0
  #define DMA_STREAMx              DMA2_Stream0
  #define ADC_CDR_ADDRESS          ((uint32_t)0x40012308)

#else 
  #define ADC_CHANNEL              ADC_Channel_12
  #define ADC1_2_CHANNEL_GPIO_CLK  RCC_AHB1Periph_GPIOC
  #define GPIO_PIN                 GPIO_Pin_2
  #define GPIO_PORT                GPIOC
  #define DMA_CHANNELx             DMA_Channel_0
  #define DMA_STREAMx              DMA2_Stream0
  #define ADC_CDR_ADDRESS          ((uint32_t)0x40012308)

#endif
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

