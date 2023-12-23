/**
  ******************************************************************************
  * @file    NVIC/NVIC_IRQMask/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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
#include "stm32f4xx_it.h"
#include "main.h"

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup NVIC_IRQMask
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t uwIndex = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f40xx.s/startup_stm32f427x.s/startup_stm32f429x.s).    */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @brief  This function handles EXTI Lines 15 to 10 interrupts requests.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  if(EXTI_GetITStatus(TAMPER_BUTTON_EXTI_LINE) != RESET)
  {
    if(uwIndex == 0)
    {
      /* Configure the BASEPRI register to 0x40 (Preemption priority = 1). 
         Only IRQ with higher preemption priority than 1 are permitted. 
         This will mask TIM3 and TIM4 IRQ from generation. */
      __set_BASEPRI(0x40);
      uwIndex++;
    }
    else
    {
      /* Configure the BASEPRI register to 0x00 (Preemption priority = 0). 
         When this BASEPRI register is set to 0, it has no effect on the current 
         priority.
         TIM2, TIM3 and TIM4 generation is controlled by NVIC priority registers. */
      __set_BASEPRI(0x00);
      uwIndex = 0;
    }

    /* Clears the TAMPER Button EXTI line pending bit */
    EXTI_ClearITPendingBit(TAMPER_BUTTON_EXTI_LINE);  
  }
} 

/**
  * @brief  This function handles TIM2 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
  /* Clear TIM2 update interrupt */
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  
  /* Toggle LED1 */
  STM_EVAL_LEDToggle(LED1);
}

/**
  * @brief  This function handles TIM3 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
  /* Clear TIM3 update interrupt */
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
  
  /* Toggle LED2 */
  STM_EVAL_LEDToggle(LED2);
}

/**
  * @brief  This function handles TIM4 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM4_IRQHandler(void)
{
  /* Clear TIM4 update interrupt */
  TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
  
  /* Toggle LED3 */
  STM_EVAL_LEDToggle(LED3);
}

/**
  * @}
  */ 

/**
  * @}
  */ 

