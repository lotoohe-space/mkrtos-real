/**
  *******************************************************************************
  * @file    stm324x9i_audio_recorder.h
  * @author  MCD Application Team
  * @version V1.0.5
  * @date    27-January-2022
  * @brief   This file contains all the functions prototypes for the wave recorder
  *          firmware driver stm324x9i_audio_recorder.c.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM324X9I_AUDIO_RECORDER_H
#define __STM324X9I_AUDIO_RECORDER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm324x9i_eval.h"
#include "audio_app_conf.h"
#include "pdm_filter.h"

/** @addtogroup Audio_Utilities
  * @{
  */

/** @addtogroup AUDIO_RECORDER
  * @{
  */

/** @defgroup AUDIO_RECORDER_Exported_Types
  * @{
  */

/**
  * @}
  */



/** @defgroup AUDIO_RECORDER_Exported_Constants
  * @{
  */

/* ----------------------------------------------------------------------------

   All the commented configuration parameters are gathered into audio_app_conf.h 
   file. If you already set them in audio_app_conf.h file, there is no need 
   to reconfigure them here.

-----------------------------------------------------------------------------*/
/* Set the preemption and subpriority values for the audio recorder timer interrupt */
/* #define AUDIO_REC_TIM_PRE_PRIO            1 */
/* #define AUDIO_REC_TIM_SUB_PRIO            0 */

/* Uncomment this define to enable single data processing (just after aquiring from ADC) */
#define AUDIO_REC_USE_DATA_PROC_SINGLE
/* Uncomment this define to enable buffer data processing (just after a full buffer has 
   been filled in by ADC) */
/* #define AUDIO_REC_USE_DATA_PROC_BUFFER */

#define ADC_TIMEOUT_MAX                   1000 /* ~83µs @120MHz */

#define AUDIO_REC_TIM_CH                  ADC_Channel_8
#define AUDIO_REC_TIM_PORT_CLK            RCC_AHB1Periph_GPIOA
#define AUDIO_REC_TIM_PORT                GPIOA
#define AUDIO_REC_TIM_PIN                 GPIO_Pin_10

/* Two channels are used:
   - one channel as input which is connected to I2S SCK in stereo mode 
   - one channel as outupt which divides the frequency on the input
*/
#define AUDIO_REC_TIM_CLK                 RCC_APB1Periph_TIM3
#define AUDIO_REC_TIM                     TIM3
#define AUDIO_REC_TIM_IN_CHANNEL          TIM_Channel_1
#define AUDIO_REC_TIM_OCInit              TIM_OC2Init /* Select channel 4 as output */
#define AUDIO_REC_TIM_GPIO_CLOCK          RCC_AHB1Periph_GPIOC
#define AUDIO_REC_TIM_GPIO                GPIOC
#define AUDIO_REC_TIM_IN_PIN              GPIO_Pin_6
#define AUDIO_REC_TIM_IN_PINSRC           GPIO_PinSource6
#define AUDIO_REC_TIM_OUT_PIN             GPIO_Pin_7
#define AUDIO_REC_TIM_OUT_PINSRC          GPIO_PinSource7
#define AUDIO_REC_TIM_AF                  GPIO_AF_TIM3

/* Audio recorder clock update direction */
#define AUDIO_REC_DIRECTION_DOWN        0
#define AUDIO_REC_DIRECTION_UP          1

//#define REC_FREQ               8000 /* Audio recording frequency in Hz */
  
/******************************************************************************/
/* I2S peripheral configuration defines */
#define AUDIO_REC_I2S                      SPI3
#define AUDIO_REC_I2S_CLK                  RCC_APB1Periph_SPI3
#define AUDIO_REC_I2S_ADDRESS              (uint32_t)(&AUDIO_REC_I2S->DR)
#define AUDIO_REC_I2S_SCK_AF               GPIO_AF_SPI3
#define AUDIO_REC_I2S_SD_AF                5
#define AUDIO_REC_I2S_IRQ                  SPI3_IRQn
#define AUDIO_REC_I2S_GPIO_CLOCK           (RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD)
#define AUDIO_REC_I2S_SCK_PIN              GPIO_Pin_3
#define AUDIO_REC_I2S_SD_PIN               GPIO_Pin_6
#define AUDIO_REC_I2S_SCK_PINSRC           GPIO_PinSource3
#define AUDIO_REC_I2S_SD_PINSRC            GPIO_PinSource6
#define AUDIO_REC_I2S_SCK_GPIO             GPIOB
#define AUDIO_REC_I2S_SD_GPIO              GPIOD

/* DMA Stream definitions */
#define AUDIO_REC_DMA_CLOCK            RCC_AHB1Periph_DMA1
#define AUDIO_REC_DMA_STREAM           DMA1_Stream2
#define AUDIO_REC_DMA_CHANNEL          DMA_Channel_0
#define AUDIO_REC_DMA_IRQ              DMA1_Stream2_IRQn
#define AUDIO_REC_DMA_FLAG_TC          DMA_FLAG_TCIF2
#define AUDIO_REC_DMA_FLAG_HT          DMA_FLAG_HTIF2
#define AUDIO_REC_DMA_FLAG_FE          DMA_FLAG_FEIF2
#define AUDIO_REC_DMA_FLAG_TE          DMA_FLAG_TEIF2
#define AUDIO_REC_DMA_FLAG_DME         DMA_FLAG_DMEIF2
#define AUDIO_REC_DMA_IT_TC            DMA_IT_TCIF2
#define AUDIO_REC_DMA_IT_HT            DMA_IT_HTIF2
#define AUDIO_REC_IRQHandler           DMA1_Stream2_IRQHandler

//#define SPIx_DR_ADDRESS          0x40003C0C
/******************************************************************************/

#define  SAMPLE_RATE_8000                    8000
#define  SAMPLE_RATE_11025                   11025
#define  SAMPLE_RATE_22050                   22050
#define  SAMPLE_RATE_44100                   44100
#define  BITS_PER_SAMPLE_8                   8
#define  WAVE_DUMMY_BYTE                     0xA5   
#define  DAC_DHR8R1_Address                  0x40007410
#define  DAC_DHR12R1_Address                 0x40007408
#define  DAC_DHR12L1_Address                 0x4000740C
#define  DAC_DHR12L2_Address                 0x40007418
/**
  * @}
  */


/** @defgroup AUDIO_RECORDER_Exported_Macros
  * @{
  */
#if !defined (CRITICAL_SECTION_ON)
 #define CRITICAL_SECTION_ON  __NOP
#endif
#if !defined (CRITICAL_SECTION_OFF)
 #define CRITICAL_SECTION_OFF __NOP
#endif
  
/**
  * @}
  */

/** @defgroup AUDIO_RECORDER_Exported_functions
  * @{
  */

uint32_t STM32_AudioRec_Init(uint32_t AudioFreq, uint32_t BitRes, uint32_t ChnlNbr);
uint32_t STM32_AudioRec_DeInit (void);
uint32_t STM32_AudioRec_Start(uint8_t* pbuf, uint32_t size);
uint32_t STM32_AudioRec_Stop(void);
uint32_t STM32_AudioRec_VolumeCtl(uint8_t Volume);
uint32_t STM32_AudioRec_MuteCtl(uint32_t Cmd);
uint32_t STM32_AudioRec_HeaderInit(uint8_t* pHeadBuf);
uint32_t STM32_AudioRec_ClkUpdate(uint32_t Step, uint32_t Direction);
uint32_t STM32_AudioRec_Xfercplt(void);
/* This function should be implemented by the user application.
   It is called into this driver when the current buffer is filled
   to prepare the next buffer pointer and its size. */
uint32_t STM32_AudioRec_Update(uint8_t** pbuf, uint32_t* pSize);

/* Optional data processing functions (Digital Filtering, Encoding ...).
   These functions' prototypes only are declared by the audio recorder module.
   Their implementation may be done (if needed) by the user into application modules. 
   These functions are enabled only when the relative defines:
   AUDIO_REC_USE_DATA_PROC_SINGLE and AUDIO_REC_USE_DATA_PROC_BUFFER are enabled in the
   file stm322xg_eval_audio_conf.h  */
#ifdef AUDIO_REC_USE_DATA_PROC_SINGLE
uint32_t STM32_AudioRec_DataProcessSingle(uint8_t* pdata, uint32_t fmt, uint32_t chnbr);
#endif
#ifdef AUDIO_REC_USE_DATA_PROC_BUFFER
uint32_t STM32_AudioRec_DataProcessBuffer(uint8_t * pbuf, uint32_t size, uint32_t fmt, uint32_t chnbr);
#endif

#ifdef __cplusplus
}
#endif

#endif /*__STM324X9I_AUDIO_RECORDER_H */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
  

