/**
  ******************************************************************************
  * @file    GPIO/GPIO_IOToggle/main.c 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Main program body
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
#include "main.h"

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_IOToggle
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
GPIO_InitTypeDef  GPIO_InitStructure;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       files (startup_stm32f40_41xxx.s/startup_stm32f427_437xx.s/startup_stm32f429_439xx.s)
       before to branch to application main. 
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */  

  /* GPIOG Peripheral clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

  /* Configure PG6 and PG8 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = LED1_PIN | LED2_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  /* To achieve GPIO toggling maximum frequency, the following  sequence is mandatory. 
     You can monitor PG6 or PG8 on the scope to measure the output signal. 
     If you need to fine tune this frequency, you can add more GPIO set/reset 
     cycles to minimize more the infinite loop timing.
     This code needs to be compiled with high speed optimization option.  */  
  while (1)
  {
    /* Set PG6 and PG8 */
    GPIOG->BSRR = (uint32_t)(LED1_PIN | LED2_PIN);
    /* Reset PG6 and PG8 */
    GPIOG->BSRR = (uint32_t)(uint32_t)((LED1_PIN | LED2_PIN)<< 16);

    /* Set PG6 and PG8 */
    GPIOG->BSRR = (uint32_t)(LED1_PIN | LED2_PIN);
    /* Reset PG6 and PG8 */
    GPIOG->BSRR = (uint32_t)(uint32_t)((LED1_PIN | LED2_PIN)<< 16);

    /* Set PG6 and PG8 */
    GPIOG->BSRR = (uint32_t)(LED1_PIN | LED2_PIN);
    /* Reset PG6 and PG8 */
    GPIOG->BSRR = (uint32_t)(uint32_t)((LED1_PIN | LED2_PIN)<< 16);

    /* Set PG6 and PG8 */
    GPIOG->BSRR = (uint32_t)(LED1_PIN | LED2_PIN);
    /* Reset PG6 and PG8 */
    GPIOG->BSRR = (uint32_t)(uint32_t)((LED1_PIN | LED2_PIN)<< 16);

    /* Set PG6 and PG8 */
    GPIOG->BSRR = (uint32_t)(LED1_PIN | LED2_PIN);
    /* Reset PG6 and PG8 */
    GPIOG->BSRR = (uint32_t)(uint32_t)((LED1_PIN | LED2_PIN)<< 16);;

    /* Set PG6 and PG8 */
    GPIOG->BSRR = (uint32_t)(LED1_PIN | LED2_PIN);
    /* Reset PG6 and PG8 */
    GPIOG->BSRR = (uint32_t)(uint32_t)((LED1_PIN | LED2_PIN)<< 16);

    /* Set PG6 and PG8 */
    GPIOG->BSRR = (uint32_t)(LED1_PIN | LED2_PIN);
    /* Reset PG6 and PG8 */
    GPIOG->BSRR = (uint32_t)(uint32_t)((LED1_PIN | LED2_PIN)<< 16);

    /* Set PG6 and PG8 */
    GPIOG->BSRR = (uint32_t)(LED1_PIN | LED2_PIN);
    /* Reset PG6 and PG8 */
    GPIOG->BSRR = (uint32_t)(uint32_t)((LED1_PIN | LED2_PIN)<< 16);

    /* Set PG6 and PG8 */
    GPIOG->BSRR = (uint32_t)(LED1_PIN | LED2_PIN);
    /* Reset PG6 and PG8 */
    GPIOG->BSRR = (uint32_t)(uint32_t)((LED1_PIN | LED2_PIN)<< 16);

    /* Set PG6 and PG8 */
    GPIOG->BSRR = (uint32_t)(LED1_PIN | LED2_PIN);
    /* Reset PG6 and PG8 */
    GPIOG->BSRR = (uint32_t)(uint32_t)((LED1_PIN | LED2_PIN)<< 16);
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

