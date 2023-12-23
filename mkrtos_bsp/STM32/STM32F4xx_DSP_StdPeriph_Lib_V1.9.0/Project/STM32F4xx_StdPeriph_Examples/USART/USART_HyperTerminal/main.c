/**
  ******************************************************************************
  * @file    USART/USART_HyperTerminal/main.c 
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

/** @addtogroup USART_HyperTerminal
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint8_t ubNbrOfDataToTransfer;
extern uint8_t ubNbrOfDataToRead;
extern __IO uint8_t ubTxCounter; 
extern __IO uint16_t uhRxCounter;

/* Private function prototypes -----------------------------------------------*/
static void NVIC_Config(void);
static void USART_Config(void);
  
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
              
  /* NVIC configuration */
  NVIC_Config();
  
  /* USART configuration */
  USART_Config();  
  
  /* Enable the EVAL_COM1 Transmit interrupt: this interrupt is generated when the 
     EVAL_COM1 transmit data register is empty */  
  USART_ITConfig(EVAL_COM1, USART_IT_TXE, ENABLE);
  
  /* Wait until EVAL_COM1 send the TxBuffer */
  while(ubTxCounter < ubNbrOfDataToTransfer)
  {}
  
  /* The software must wait until TC=1. The TC flag remains cleared during all data
     transfers and it is set by hardware at the last frame’s end of transmission*/
  while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET)
  {}
 
  /* Enable the EVAL_COM1 Receive interrupt: this interrupt is generated when the 
     EVAL_COM1 receive data register is not empty */
  USART_ITConfig(EVAL_COM1, USART_IT_RXNE, ENABLE);
  
  /* Wait until EVAL_COM1 receive the RxBuffer */
  while(uhRxCounter < ubNbrOfDataToRead)
  {}

  while (1)
  {
  }
}

/**
  * @brief  Configures the USART Peripheral.
  * @param  None
  * @retval None
  */
static void USART_Config(void)
{
  USART_InitTypeDef USART_InitStructure;
  
/* USARTx configuration ------------------------------------------------------*/
  /* USARTx configured as follows:
        - BaudRate = 9600 baud  
        - Word Length = 8 Bits
        - Two Stop Bit
        - Odd parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_2;
  USART_InitStructure.USART_Parity = USART_Parity_Odd;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM1, &USART_InitStructure);
}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
static void NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EVAL_COM1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
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

