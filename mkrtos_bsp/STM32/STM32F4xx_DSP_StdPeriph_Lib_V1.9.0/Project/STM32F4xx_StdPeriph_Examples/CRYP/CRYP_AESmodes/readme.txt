/**
  @page CRYP_AESmodes  Encrypt and Decrypt data using AES Algo in chaining modes and all key sizes
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    CRYP/CRYP_AESmodes/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Description of the CRYP AES Algorithm in all modes and all key 
  *          sizes example.
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
  @endverbatim

@par Example Description 

This example shows how to use the CRYPTO peripheral to encrypt and decrypt data 
using AES in chaining modes (ECB, CBC, CTR) and all key sizes (128, 192, 256) 
Algorithm.

For this example, a firmware is used to interface with the Crypto peripheral
in polling mode.

This example behaves as follows:

1. ECB mode :
   - AES128 ECB Encryption
   - AES192 ECB Encryption
   - AES256 ECB Encryption

   - AES128 ECB Decryption
   - AES192 ECB Decryption
   - AES256 ECB Decryption

2. CBC mode :
   - AES128 CBC Encryption
   - AES192 CBC Encryption
   - AES256 CBC Encryption

   - AES128 CBC Decryption
   - AES192 CBC Decryption
   - AES256 CBC Decryption

3. CTR mode :
   - AES128 CTR Encryption
   - AES192 CTR Encryption
   - AES256 CTR Encryption

   - AES128 CTR Decryption
   - AES192 CTR Decryption
   - AES256 CTR Decryption

The Plain data, encrypted data and decrypted data can be displayed on a PC
HyperTerminal using the USART.

The USARTx is configured as follows:
    - BaudRate = 115200 baud  
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled


@par Directory contents 
  
  - CRYP/CRYP_AESmodes/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - CRYP/CRYP_AESmodes/stm32f4xx_conf.h     Library Configuration file
  - CRYP/CRYP_AESmodes/stm32f4xx_it.c       Interrupt handlers
  - CRYP/CRYP_AESmodes/stm32f4xx_it.h       Interrupt handlers header file
  - CRYP/CRYP_AESmodes/main.c               Main program
  - CRYP/CRYP_AESmodes/main.h               Main program header file


@par Hardware and Software environment 

  - This example runs on STM32F415xx/417xx, STM32F437xx and STM32F439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F41xx 
    Devices), STM32437I-EVAL (STM32F437xx Devices) and STM32439I-EVAL RevB 
    (STM32F439xx Devices) evaluation boards and can be easily tailored 
    to any other supported device and development board.

  - STM324xG-EVAL and STM32437I-EVAL Set-up 
    - Connect a null-modem female/female RS232 cable between the DB9 connector 
      CN16 (USART3) and PC serial port.
      @note Make sure that jumper JP22 is in position 1-2
      
  - STM32439I-EVAL RevB Set-up 
    - Connect a null-modem female/female RS232 cable between the DB9 connector 
      CN8 (USART1) and PC serial port.
      @note Make sure that jumper JP7 is in position 1-2
 
  - Hyperterminal configuration:
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - BaudRate = 115200 baud
    - flow control: None


@par How to use it ? 

In order to make the program work, you must do the following:
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain
 - Select the project workspace related to the used device 
   - If "STM32F40_41xxx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM3240_41_G_EVAL\stm324xg_eval.c 
      
   - If "STM32F427_437xx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x7I_EVAL\stm324x7i_eval.c
     
   - If "STM32F429_439xx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval.c
       
 - Rebuild all files and load your image into target memory
 - Run the example
  
 
 */
