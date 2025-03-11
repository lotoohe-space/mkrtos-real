/**
  ******************************************************************************
  * @file    STM32F0xx_IAP/src/menu.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    29-May-2012
  * @brief   Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "ymodem.h"
#include "ymodem_menu.h"
#include "ymodem_util.h"
#include "ymodem_export.h"

/** @addtogroup STM32F0xx_IAP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t JumpAddress;
extern uint32_t UserMemoryMask;
uint32_t FlashProtection = 0;
uint8_t tab_1024[1024] = {0};
uint8_t FileName[FILE_NAME_LENGTH];

/* Private function prototypes -----------------------------------------------*/
void SerialDownload(void);
void SerialUpload(void);


// #warning "avoid errors"
#define APPLICATION_ADDRESS     0x08000000
#define USER_FLASH_SIZE         1024

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Download a file via serial port
  * @param  None
  * @retval None
  */
void SerialDownload(void)
{
    uint8_t Number[10] = {0};
    int32_t Size = 0;

    SerialPutString("Waiting for the file to be sent ... (press 'a' to abort)\n\r");
    Size = Ymodem_Receive(&tab_1024[0]);
    ymodem_recv_end_cb();
    if (Size > 0)
    {
        SerialPutString("\n\n\r Download File Successfully!\n\r--------------------------------\r\n Name: ");
        SerialPutString(FileName);
        Int2Str(Number, Size);
        SerialPutString("\n\r Size: ");
        SerialPutString(Number);
        SerialPutString(" Bytes\r\n");
        SerialPutString("--------------------------------\n");
    }
    else if (Size == -1)
    {
        SerialPutString("\n\n\rThe file size is higher than the allowed space memory!\n\r");
    }
    else if (Size == -2)
    {
        SerialPutString("\n\n\rVerification failed!\n\r");
    }
    else if (Size == -3)
    {
        SerialPutString("\r\n\nAborted by user.\n\r");
    }
    else
    {
        SerialPutString("\n\rFailed to receive the file!\n\r");
    }
}
#if 0
/**
  * @brief  Upload a file via serial port.
  * @param  None
  * @retval None
  */
void SerialUpload(void)
{
    uint8_t status = 0 ; 

    SerialPutString("\n\n\rSelect Receive File\n\r");

    if (GetKey() == CRC16)
    {
        /* Transmit the flash image through ymodem protocol */
        status = Ymodem_Transmit((uint8_t*)APPLICATION_ADDRESS, (const uint8_t*)"UploadedFlashImage.bin", USER_FLASH_SIZE);

        if (status != 0) 
        {
            SerialPutString("\n\rError Occurred while Transmitting File\n\r");
        }
        else
        {
            SerialPutString("\n\rFile uploaded successfully \n\r");
        }
    }
}
#endif
/**
  * @brief  Display the Main Menu on HyperTerminal
  * @param  None
  * @retval None
  */
void Ymodem_Main_Entrance(void)
{
    uint8_t key = 0;

    ymodem_init();
    SerialPutString("\r\n======================================================================");
    SerialPutString("\r\n=              (C) COPYRIGHT 2012 STMicroelectronics                 =");
    SerialPutString("\r\n=                                                                    =");
    SerialPutString("\r\n=     MKRTOS In-Application Programming Application  (Version 1.0.0) =");
    SerialPutString("\r\n=                                                                    =");
    SerialPutString("\r\n=                                   By MCD Application Team          =");
    SerialPutString("\r\n======================================================================");
    SerialPutString("\r\n\r\n");

    while (1)
    {
        SerialPutString("\r\n================== Ymodem Menu =========================\r\n\n");
        SerialPutString("  Download File from PC to Board ------- 1\r\n\n");
        // SerialPutString("  Upload File from Board to PC   ------- 2\r\n\n");
        SerialPutString("  Exit Y-modem                   ------- 2\r\n\n");
        SerialPutString("==========================================================\r\n\n");

        /* Receive key */
        key = GetKey();

        if (key == 0x31)
        {
            /* Download user application in the Flash */
            SerialDownload();
        }
        #if 0
        else if (key == 0x32)
        {
            /* Upload user application from the Flash */
            SerialUpload();
        }
        #endif
        else if (key == 0x32) /* execute the new program */
        {
            SerialPutString("Exit Y-modem ....\r\n");
            break;
        }
        else
        {
            SerialPutString("Invalid Number ! ==> The number should be either 1, 2 or 3\r");
        }
    }
}


/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
