/**
  @page CRC_Example CRC Calculation example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    CRC/CRC_Example/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Description of the CRC Calculation example.
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

This example shows how to use CRC (Cyclic Redundancy Check) calculation unit to 
get a CRC code of a given buffer of data word(32-bit), based on a fixed generator
polynomial(0x4C11DB7).


@par Directory contents 
  
  - CRC/CRC_Example/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - CRC/CRC_Example/stm32f4xx_conf.h     Library Configuration file
  - CRC/CRC_Example/stm32f4xx_it.c       Interrupt handlers
  - CRC/CRC_Example/stm32f4xx_it.h       Interrupt handlers header file
  - CRC/CRC_Example/main.c               Main program
  - CRC/CRC_Example/main.h               Main program header file
  
         
@par Hardware and Software environment

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) and 
    STM324x9I-EVAL RevB (STM32F429xx/STM32F439xx Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.


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
