/**
  ******************************************************************************
  * @file    SPI/SPI_TwoBoards/SPI_DataExchangeInterrupt/stm32f4xx_it.c 
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

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup SPI_DataExchangeInterrupt
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern  __IO uint8_t aRxBuffer[];
extern __IO uint8_t ubRxIndex;
extern __IO uint8_t ubTxIndex;
extern uint8_t aTxBuffer[];
extern __IO uint32_t TimeOut;
__IO uint8_t ubCounter = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
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
  /* Decrement the timeout value */
  if (TimeOut != 0)
  {
    TimeOut--;
  }
    
  if (ubCounter < 10)
  {
    ubCounter++;
  }
  else
  {
    ubCounter = 0;
    STM_EVAL_LEDToggle(LED1);
  }
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f40xx.s/startup_stm32f427x.s).                         */
/******************************************************************************/

/**
  * @brief  This function handles SPI interrupt request.
  * @param  None
  * @retval None
  */
void SPIx_IRQHANDLER(void)
{
  /* SPI in Receiver mode */
  if (SPI_I2S_GetITStatus(SPIx, SPI_I2S_IT_RXNE) == SET)
  {
    if (ubRxIndex < BUFFERSIZE)
    {
      /* Receive Transaction data */
      aRxBuffer[ubRxIndex++] = SPI_I2S_ReceiveData(SPIx);
    }
    else
    {
      /* Disable the Rx buffer not empty interrupt */
      SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, DISABLE);
    }
  }
  /* SPI in Transmitter mode */
  if (SPI_I2S_GetITStatus(SPIx, SPI_I2S_IT_TXE) == SET)
  {
    if (ubTxIndex < BUFFERSIZE)
    {
      /* Send Transaction data */
      SPI_I2S_SendData(SPIx, aTxBuffer[ubTxIndex++]);
    }
    else
    {
      /* Disable the Tx buffer empty interrupt */
      SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_TXE, DISABLE);
    }
  }
}


/**
  * @}
  */ 

/**
  * @}
  */ 

