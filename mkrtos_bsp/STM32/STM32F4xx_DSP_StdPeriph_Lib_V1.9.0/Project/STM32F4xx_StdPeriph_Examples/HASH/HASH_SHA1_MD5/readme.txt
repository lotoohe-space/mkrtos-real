/**
  @page HASH_SHA1_MD5  HASH digest calculation using SHA1 and MD5 example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    HASH/HASH_SHA1_MD5/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Description of the digest calculation using SHA1 and MD5 example.
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

This example shows how to use the HASH peripheral to hash data using SHA-1 
and MD5 Algorithms.

For this example, no DMA is used for data transfer.

The SHA-1 message digest result is a 160 bit data and the MD5 message digest result 
is a 128 bit data.

The SHA-1 and MD5 digests can be displayed on a PC HyperTerminal using the USART.


@par Directory contents 
  
  - HASH/HASH_SHA1_MD5/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - HASH/HASH_SHA1_MD5/stm32f4xx_conf.h     Library Configuration file
  - HASH/HASH_SHA1_MD5/stm32f4xx_it.c       Interrupt handlers
  - HASH/HASH_SHA1_MD5/stm32f4xx_it.h       Interrupt handlers header file
  - HASH/HASH_SHA1_MD5/main.c               Main program
  - HASH/HASH_SHA1_MD5/main.h               Main program header file


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
