/**
  ******************************************************************************
  * @file    NVIC/NVIC_IRQPriority/main.c 
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

/** @addtogroup NVIC_IRQPriority
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint8_t ubPreemptionOccurred = 0; 
__IO uint8_t ubPreemptionPriorityValue = 0; 

/* Private function prototypes -----------------------------------------------*/
static void NVIC_Config(void);
static void Delay(__IO uint32_t nCount);

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
  
  /* NVIC configuration ------------------------------------------------------*/
  NVIC_Config();     
  
  /* Initialize LEDs mounted on EVAL board */       
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  
  /* Initialize the KEY/Tamper and Wakeup buttons mounted on EVAL board */  
  STM_EVAL_PBInit(BUTTON_KEY_TAMPER, BUTTON_MODE_EXTI);
  STM_EVAL_PBInit(BUTTON_WAKEUP, BUTTON_MODE_EXTI); 

  /* Configure the SysTick Handler Priority: Preemption priority and subpriority */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), !ubPreemptionPriorityValue, 0));

  while (1)
  {
    if(ubPreemptionOccurred != 0)
    {
      /* Toggle LED1 */
      STM_EVAL_LEDToggle(LED1);
      
      /* Insert delay Time */
      Delay(0x5FFFF);
      
      /* Toggle LED2 */
      STM_EVAL_LEDToggle(LED2);
      
      Delay(0x5FFFF);
      
      /* Toggle LED3 */
      STM_EVAL_LEDToggle(LED3);
      
      Delay(0x5FFFF);
      
      /* Toggle LED4 */
      STM_EVAL_LEDToggle(LED4);
      
      Delay(0x5FFFF); 
    }
  }
}

/**
  * @brief  Configures the NVIC interrupts.
  * @param  None
  * @retval None
  */
static void NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure the preemption priority and subpriority:
     - 1 bits for pre-emption priority: possible value are 0 or 1 
     - 3 bits for subpriority: possible value are 0..7
     - Lower values gives higher priority  
   */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* Enable the WAKEUP_BUTTON_EXTI_IRQn Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = WAKEUP_BUTTON_EXTI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ubPreemptionPriorityValue;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the KEY_BUTTON_EXTI_IRQn/TAMPER_BUTTON_EXTI_IRQn Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = BUTTON_EXTI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
static void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
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

