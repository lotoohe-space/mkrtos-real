/**
  ******************************************************************************
  * @file    NVIC/NVIC_WFIMode/stm32f4xx_it.c 
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

/** @addtogroup NVIC_WFIMode
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t ubTestStatus = 0;
uint16_t aSrcBuffer[10] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};
extern uint16_t aDstBuffer[10];
extern __IO uint32_t uwLowPowerMode;

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


#ifdef USE_STM324x9I_EVAL
/**
  * @brief  This function handles DMA2 Stream5 interrupt request.
  * @param  None
  * @retval None
  */
void DMA2_Stream5_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA2_Stream5, DMA_IT_TCIF5))
  {
    DMA_ClearITPendingBit(DMA2_Stream5, DMA_IT_TCIF5);

    /* Check the received buffer */
    ubTestStatus = Buffercmp16(aSrcBuffer, aDstBuffer, 10);

    if(ubTestStatus == 0)
    {
      STM_EVAL_LEDToggle(LED2);
    }
    else
    {
      STM_EVAL_LEDToggle(LED3);
    }
  }
}
#else
/**
  * @brief  This function handles DMA1 Stream1 interrupt request.
  * @param  None
  * @retval None
  */
void DMA1_Stream1_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA1_Stream1, DMA_IT_TCIF1))
  {
    DMA_ClearITPendingBit(DMA1_Stream1, DMA_IT_TCIF1);

    /* Check the received buffer */
    ubTestStatus = Buffercmp16(aSrcBuffer, aDstBuffer, 10);

    if(ubTestStatus == 0)
    {
      STM_EVAL_LEDToggle(LED2);
    }
    else
    {
      STM_EVAL_LEDToggle(LED3);
    }
  }
}
#endif /* USE_STM324x9I_EVAL */

/**
  * @brief  This function handles External lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  if(EXTI_GetITStatus(BUTTON_EXTI_LINE) != RESET)
  {
    EXTI_ClearITPendingBit(BUTTON_EXTI_LINE);
  
    uwLowPowerMode = 1;
  }  
}

/**
  * @}
  */ 

/**
  * @}
  */ 

