/**
  ******************************************************************************
  * @file    stm324x9i_audio_recorder.c
  * @author  MCD Application Team
  * @version V1.0.5
  * @date    27-January-2022
  * @brief   This file contains the main function for audio recording using I2S
  *          and timer on STM324x9i-EVAL evaluation board.
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

/* Includes ------------------------------------------------------------------*/
#include "stm324x9i_audio_recorder.h"
#include "string.h"

/** @addtogroup Audio_Utilities
  * @{
  */

/** @defgroup AUDIO_RECORDER
  * @brief    This file contains the main function for audio recording using I2S
  *           and timer on STM324x9I-EVAL evaluation board.
  * @{
  */

/** @defgroup AUDIO_RECORDER_Private_Types
  * @{
  */
typedef enum
{
  LittleEndian,
  BigEndian
}Endianness;
/**
  * @}
  */

/** @defgroup AUDIO_RECORDER_Private_Defines
  * @{
  */
/**
  * @}
  */

/** @defgroup AUDIO_RECORDER_Private_Macros
  * @{
  */

#define REC_FREQ 16000  
#define MIC_NUM 2
/* PDM buffer input size */
#define INTERNAL_BUFF_SIZE      128*REC_FREQ/16000*MIC_NUM
/* PCM buffer output size */
#define PCM_OUT_SIZE            REC_FREQ/1000*2
#define CHANNEL_DEMUX_MASK      0x55
/**
  * @}
  */

/** @defgroup AUDIO_RECORDER_Private_Variables
  * @{
  */

/* Audio recording Samples format (from 8 to 16 bits) */
uint32_t AudioRecBitRes = 16; 

/* Audio recording number of channels (1 for Mono or 2 for Stereo) */
uint32_t AudioRecChnlNbr = 2;

/* Main buffer pointer for the recorded data storing */
uint8_t* pAudioRecBuf;

/* Current size of the recorded buffer */
uint32_t AudioRecCurrSize = 0; 

/* Number of data currently stored into the audio buffer */
uint32_t AudioRecCounter = 0;

uint8_t AudioRecVolume = 64;
/* Local buffer for temporary operations */
uint8_t* TmpBuff = 0;

uint16_t RecBuf[PCM_OUT_SIZE];
__IO uint32_t Data_Status = 0;
static uint16_t InternalBuffer[INTERNAL_BUFF_SIZE];
static DMA_InitTypeDef DMA_InitStructure;
PDMFilter_InitStruct Filter[2];
uint8_t Channel_Demux[128] = {
    0x00, 0x01, 0x00, 0x01, 0x02, 0x03, 0x02, 0x03,
    0x00, 0x01, 0x00, 0x01, 0x02, 0x03, 0x02, 0x03,
    0x04, 0x05, 0x04, 0x05, 0x06, 0x07, 0x06, 0x07,
    0x04, 0x05, 0x04, 0x05, 0x06, 0x07, 0x06, 0x07,
    0x00, 0x01, 0x00, 0x01, 0x02, 0x03, 0x02, 0x03,
    0x00, 0x01, 0x00, 0x01, 0x02, 0x03, 0x02, 0x03,
    0x04, 0x05, 0x04, 0x05, 0x06, 0x07, 0x06, 0x07,
    0x04, 0x05, 0x04, 0x05, 0x06, 0x07, 0x06, 0x07,
    0x08, 0x09, 0x08, 0x09, 0x0a, 0x0b, 0x0a, 0x0b,
    0x08, 0x09, 0x08, 0x09, 0x0a, 0x0b, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0c, 0x0d, 0x0e, 0x0f, 0x0e, 0x0f,
    0x0c, 0x0d, 0x0c, 0x0d, 0x0e, 0x0f, 0x0e, 0x0f,
    0x08, 0x09, 0x08, 0x09, 0x0a, 0x0b, 0x0a, 0x0b,
    0x08, 0x09, 0x08, 0x09, 0x0a, 0x0b, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0c, 0x0d, 0x0e, 0x0f, 0x0e, 0x0f,
    0x0c, 0x0d, 0x0c, 0x0d, 0x0e, 0x0f, 0x0e, 0x0f
};
uint8_t AppPDM[INTERNAL_BUFF_SIZE*2];
uint32_t j;
__IO uint32_t AUDIODataReady = 0;
extern uint16_t WrBufffer[];
__IO uint32_t ITCounter = 0;
__IO uint32_t AUDIOBuffOffset = 0;
/**
  * @}
  */

/** @defgroup AUDIO_RECORDER_Private_FunctionPrototypes
  * @{
  */
static void STM32_AudioRec_PDMDecoder_Init(uint32_t AudioFreq, uint32_t ChnlNbr);
static void STM32_AudioRec_I2S_Init(void);
static void STM32_AudioRec_TIM_Init(void);
void AUDIO_REC_IRQHandler(void);
/**
  * @}
  */

/** @defgroup AUDIO_RECORDER_Private_Functions
  * @{
  */

/**
  * @brief  Initialize wave recording
  * @param  None
  * @retval None
  */
uint32_t STM32_AudioRec_Init(uint32_t AudioFreq, uint32_t BitRes, uint32_t ChnlNbr)
{
  /* Configure the PDM library */
  STM32_AudioRec_PDMDecoder_Init(AudioFreq, ChnlNbr);
    
  /* Configure the Timer which clocks the MEMS */
  STM32_AudioRec_TIM_Init();
  
  /* Configure the I2S peripheral */
  STM32_AudioRec_I2S_Init();
  
  /* Set the local parameters */
  AudioRecBitRes = BitRes;
  AudioRecChnlNbr = ChnlNbr;

  /* Return 0 if all operations are OK */
  return 0;
}

/**
  * @brief  Denitializes all the resources used by wave recorder
  * @param  None
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t STM32_AudioRec_DeInit (void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Enable the DMA Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = AUDIO_REC_DMA_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = AUDIO_REC_DMA_PRE_PRIO;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = AUDIO_REC_DMA_SUB_PRIO;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Disable AUDIO_REC_TIM */
  TIM_Cmd(AUDIO_REC_TIM, DISABLE);
    
  /* Reset number of samples */
  AudioRecCounter = 0;
  AudioRecCurrSize = 0;
  
  /* Return 0 if all operations are OK */
  return 0;
}

/**
  * @brief  Start audio recording
  * @param  None
  * @retval None
  */
uint32_t STM32_AudioRec_Start(uint8_t* pbuf, uint32_t size)
{
  /* Store the location and size of the audio buffer */
  pAudioRecBuf = pbuf;
  AudioRecCurrSize = size;
  
  /* Enable the Rx buffer not empty interrupt */
  SPI_I2S_ITConfig(AUDIO_REC_I2S, SPI_I2S_IT_RXNE, ENABLE);
    
  /* Enable I2S peripheral: once enabled, clock is generated on I2S_SCK */
  I2S_Cmd(AUDIO_REC_I2S, ENABLE);
  
  /* Return 0 if all operations are OK */
  return 0;
}

/**
  * @brief  Stop audio recording
  * @param  None
  * @retval None
  */
uint32_t STM32_AudioRec_Stop(void)
{
  /* Stop the Transfer on the I2S side: Stop and disable the DMA stream */
  DMA_Cmd(AUDIO_REC_DMA_STREAM, DISABLE);
  
  /* Clear all the DMA flags for the next transfer */
  DMA_ClearFlag(AUDIO_REC_DMA_STREAM, AUDIO_REC_DMA_FLAG_TC |AUDIO_REC_DMA_FLAG_HT | \
                                  AUDIO_REC_DMA_FLAG_FE | AUDIO_REC_DMA_FLAG_TE);
  
  /* Enable I2S RXNE requests to DMA */
  SPI_I2S_DMACmd(AUDIO_REC_I2S, SPI_I2S_DMAReq_Rx, DISABLE);
  
  /* Disable I2S peripheral */
  I2S_Cmd(AUDIO_REC_I2S, DISABLE);
  
  /* Disable AUDIO_REC_TIM */
  TIM_Cmd(AUDIO_REC_TIM, DISABLE);
    
  /* Reset number of samples */
  AudioRecCounter = 0;
  AudioRecCurrSize = 0;
  
  /* Return 0 if all operations are OK */
  return 0;
}

/**
  * @brief  Update the current value of the timer auto reload
  * @param  Step: decrement/increment step
  * @param  Direction: Direction of increment/decrement. Can be 
  *         AUDIO_REC_DIRECTION_DOWN or AUDIO_REC_DIRECTION_UP.
  * @retval None
  */
uint32_t STM32_AudioRec_ClkUpdate(uint32_t Step, uint32_t Direction)
{
  /* If operation is OK, return 0 */
  return 0;
}

/**
  * @brief Controls the current audio volume level. 
  * @param Volume: Volume level to be set in percentage from 0% to 100% (0 for 
  *        Mute and 100 for Max volume level).
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t STM32_AudioRec_VolumeCtl(uint8_t Volume)
{
  /* Call the codec volume control function with converted volume value */
  AudioRecVolume = Volume;
  return 0;
}


/**
  * @brief Enable or disable the MUTE mode by software 
  * @param Command: could be AUDIO_MUTE_ON to mute sound or AUDIO_MUTE_OFF to 
  *        unmute the codec and restore previous volume level.
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t STM32_AudioRec_MuteCtl(uint32_t Cmd)
{ 
  /* Call the Codec Mute function */
  return 0;
}

/**
  * @brief  Initialize the PDM library.
  * @param  AudioFreq: Audio sampling frequency
  * @param  ChnlNbr: Number of audio channels (1: mono; 2: stereo)
  * @retval None
  */
static void STM32_AudioRec_PDMDecoder_Init(uint32_t AudioFreq, uint32_t ChnlNbr)
{ 
  uint32_t i;
  
  /* Enable CRC peripheral to unlock the PDM library */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
  
  for(i = 0; i < ChnlNbr; i++)
  {
    /* Filter LP & HP Init */
    Filter[i].LP_HZ = REC_FREQ/2;
    Filter[i].HP_HZ = 10;
    Filter[i].Fs = REC_FREQ;
    Filter[i].Out_MicChannels = MIC_NUM;
    Filter[i].In_MicChannels = MIC_NUM; 
    PDM_Filter_Init((PDMFilter_InitStruct *)&Filter[i]);
  }  
}

/**
  * @brief  Initialize I2S peripheral.
  * @param  None
  * @retval None
  */
static void STM32_AudioRec_I2S_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  I2S_InitTypeDef I2S_InitStructure;
  
  /* Configure the IRQ channel associated to DMA stream */
  NVIC_InitStructure.NVIC_IRQChannel = AUDIO_REC_DMA_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(AUDIO_REC_I2S_GPIO_CLOCK, ENABLE);
  
  /* Configure I2S GPIOs in Alternate function */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* I2S SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = AUDIO_REC_I2S_SCK_PIN;
  GPIO_Init(AUDIO_REC_I2S_SCK_GPIO, &GPIO_InitStructure);
  GPIO_PinAFConfig(AUDIO_REC_I2S_SCK_GPIO, AUDIO_REC_I2S_SCK_PINSRC, AUDIO_REC_I2S_SCK_AF);
  
  /* I2S SD pin configuration */
  GPIO_InitStructure.GPIO_Pin = AUDIO_REC_I2S_SD_PIN;
  GPIO_Init(AUDIO_REC_I2S_SD_GPIO, &GPIO_InitStructure);
  GPIO_PinAFConfig(AUDIO_REC_I2S_SD_GPIO, AUDIO_REC_I2S_SD_PINSRC, AUDIO_REC_I2S_SD_AF);
  
  /* Enable the SPI clock */
  RCC_APB1PeriphClockCmd(AUDIO_REC_I2S_CLK, ENABLE);
  
  /* SPI configuration */
  SPI_I2S_DeInit(AUDIO_REC_I2S);
  I2S_InitStructure.I2S_AudioFreq = 64000;
  I2S_InitStructure.I2S_Standard = I2S_Standard_LSB;
  I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
  I2S_InitStructure.I2S_CPOL = I2S_CPOL_High;
  I2S_InitStructure.I2S_Mode = I2S_Mode_MasterRx;
  I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
  /* Initialize the I2S peripheral with the structure above */
  I2S_Init(AUDIO_REC_I2S, &I2S_InitStructure);
  
  /* Enable the Rx buffer not empty interrupt */
  SPI_I2S_ITConfig(AUDIO_REC_I2S, SPI_I2S_IT_RXNE, ENABLE);
  
  /* Enable DMA clock */
  RCC_AHB1PeriphClockCmd(AUDIO_REC_DMA_CLOCK, ENABLE);
  
  /* Configure DMA Stream */
  DMA_InitStructure.DMA_Channel = AUDIO_REC_DMA_CHANNEL;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)AUDIO_REC_I2S_ADDRESS;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)InternalBuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = (uint32_t)INTERNAL_BUFF_SIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(AUDIO_REC_DMA_STREAM, &DMA_InitStructure);
  
  /* Enable DMA Stream Transfer Complete interrupt */
  DMA_ITConfig(AUDIO_REC_DMA_STREAM, DMA_IT_TC, ENABLE);
  DMA_ITConfig(AUDIO_REC_DMA_STREAM, DMA_IT_HT, ENABLE);
  /* Enabme DMA request on receiving Data */
  SPI_I2S_DMACmd(AUDIO_REC_I2S, SPI_I2S_DMAReq_Rx, ENABLE);
  
  /* DMA Stream enable */
  DMA_Cmd(AUDIO_REC_DMA_STREAM, ENABLE);
}

/**
  * @brief  Configure TIM as a clock divider by 2.
  *         I2S_SCK is externally connected to TIMx input channel
  * @param  None
  * @retval None
  */
static void STM32_AudioRec_TIM_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_ICInitTypeDef  TIM_ICInitStructure;
  
  /* TIM's GPIO clock enable */
  RCC_AHB1PeriphClockCmd(AUDIO_REC_TIM_GPIO_CLOCK, ENABLE);
  
  /* Configure TIM input channel */
  GPIO_InitStructure.GPIO_Pin =  AUDIO_REC_TIM_IN_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(AUDIO_REC_TIM_GPIO, &GPIO_InitStructure);
  GPIO_PinAFConfig(AUDIO_REC_TIM_GPIO, AUDIO_REC_TIM_IN_PINSRC, AUDIO_REC_TIM_AF);
  
  /* Configure TIM output channel */
  GPIO_InitStructure.GPIO_Pin = AUDIO_REC_TIM_OUT_PIN;
  GPIO_Init(AUDIO_REC_TIM_GPIO, &GPIO_InitStructure);
  GPIO_PinAFConfig(AUDIO_REC_TIM_GPIO, AUDIO_REC_TIM_OUT_PINSRC, AUDIO_REC_TIM_AF);
  
  /* TIM clock enable */
  RCC_APB1PeriphClockCmd(AUDIO_REC_TIM_CLK, ENABLE);
  
  /* Input channel configuration as clock source */
  TIM_ICInitStructure.TIM_Channel = AUDIO_REC_TIM_IN_CHANNEL;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0;
  TIM_ICInit(AUDIO_REC_TIM, &TIM_ICInitStructure);
  
  /* Select external clock mode 1 */
  TIM_ETRClockMode1Config(AUDIO_REC_TIM, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0);
  /* Select Input Channel as input trigger */
  TIM_SelectInputTrigger(AUDIO_REC_TIM, TIM_TS_TI1FP1);
  
  /* Set the TIM period (auto-reload) */
  TIM_SetAutoreload(AUDIO_REC_TIM, 1);
  
  /* PWM Mode configuration: Out Channel */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 1;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  AUDIO_REC_TIM_OCInit(AUDIO_REC_TIM, &TIM_OCInitStructure);
  
  /* TIM enable counter */
  TIM_Cmd(AUDIO_REC_TIM, ENABLE);
}

/**
  * @brief  DMA Audio recorder IRQ hanlder
  * @param  None
  * @retval None
  */
void AUDIO_REC_IRQHandler(void)
{
  uint32_t InOffset, i; 
  uint8_t a, b;

  /* Check if half transfer occured */
  if(DMA_GetITStatus(AUDIO_REC_DMA_STREAM, AUDIO_REC_DMA_IT_HT))
  {
    /* half Transfer interrupt */
    InOffset = 0;
  }
  /* Check if transfer complete occured */
  else if(DMA_GetITStatus(AUDIO_REC_DMA_STREAM, AUDIO_REC_DMA_IT_TC))
  {
    /* Transfer Complete interrupt */
    InOffset=INTERNAL_BUFF_SIZE/2;
  }
  
  /* PDM Demux */
  for(i=0; i<INTERNAL_BUFF_SIZE/2; i++)
  {
    b = (InternalBuffer[InOffset+i]>>8)&0XFF;
    a = (InternalBuffer[InOffset+i]&0XFF);
    AppPDM[(i*2)+1] = Channel_Demux[a & CHANNEL_DEMUX_MASK] | Channel_Demux[b & CHANNEL_DEMUX_MASK] << 4;;
    AppPDM[(i*2)] = Channel_Demux[(a>>1) & CHANNEL_DEMUX_MASK] | Channel_Demux[(b>>1) & CHANNEL_DEMUX_MASK] << 4;
  }
  
  for(i = 0; i < MIC_NUM; i++)
  {
    /* PDM to PCM filter */
    PDM_Filter_64_LSB((u8*)&AppPDM[i], (u16*)&(RecBuf[i]), AudioRecVolume , (PDMFilter_InitStruct *)&Filter[i]);
  }
  /* Copy PCM data in internal buffer */
  memcpy(WrBufffer+(ITCounter * 32), RecBuf, PCM_OUT_SIZE*2);
  
  if(ITCounter == 35)
  {
    AUDIODataReady = 1;
    AUDIOBuffOffset = 0;
    ITCounter++;
  }
  else if(ITCounter == 71)
  {
    AUDIODataReady = 1;
    AUDIOBuffOffset = 1152;
    ITCounter = 0;
  }
  else
  {
    ITCounter++;
  }
  if (InOffset==0)
  {
    DMA_ClearITPendingBit(AUDIO_REC_DMA_STREAM, AUDIO_REC_DMA_IT_HT);
  }
  else
  {
    DMA_ClearITPendingBit(AUDIO_REC_DMA_STREAM, AUDIO_REC_DMA_IT_TC);
  }
}
/**
  * @}
  */

/**
  * @}
  */
/**
  * @}
  */


