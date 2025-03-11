/**
  ******************************************************************************
  * @file    STM32F0xx_IAP/inc/common.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    29-May-2012
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __YMODEM_UTIL_H__
#define __YMODEM_UTIL_H__

/* Includes ------------------------------------------------------------------*/
#include "common.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Constants used by Serial Command Line Mode */
#define CMD_STRING_SIZE       128

/* Exported macro ------------------------------------------------------------*/
/* Common routines */
#define IS_AF(c)             ((c >= 'A') && (c <= 'F'))
#define IS_af(c)             ((c >= 'a') && (c <= 'f'))
#define IS_09(c)             ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)        IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)        IS_09(c)
#define CONVERTDEC(c)        (c - '0')

#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)        (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

#define SerialPutString(x)   Serial_PutString((uint8_t*)(x))

/* Exported functions ------------------------------------------------------- */
void Int2Str(uint8_t* str,int32_t intnum);
uint32_t Str2Int(uint8_t *inputstr,int32_t *intnum);
uint32_t GetIntegerInput(int32_t * num);
uint8_t GetKey(void);
void SerialPutChar(uint8_t c);
void Serial_PutString(uint8_t *s);
void GetInputString(uint8_t * buffP);



#endif  /* __YMODEM_UTIL_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
