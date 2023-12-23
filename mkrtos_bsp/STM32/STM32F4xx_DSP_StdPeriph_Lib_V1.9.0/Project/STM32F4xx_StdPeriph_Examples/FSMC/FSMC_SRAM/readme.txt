/**
  @page FSMC_SRAM SRAM memory layered access example
  
  @verbatim
  ******************* (C) COPYRIGHT 2016 STMicroelectronics ********************
  * @file    FSMC/FSMC_SRAM/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Description of the FSMC SRAM example.
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

This example shows how to configure the FSMC to drive the SRAM memory mounted 
on STM3240_41_G_EVAL or STM32437I_EVAL or STM324x9I_EVAL board. It uses an  
associate driver developed to interface with the indicated SRAM device.

After initializing the device, user can perform read/write operations on it. 
A data buffer is written to the SRAM memory, then read back and checked to verify 
its correctness.

To enable the continuous clock feature featured, the burst access mode must be 
also enabled in the used FSMC SRAM bank. The continuous clock will be automatically 
enabled in the FSMC SRAM bank 1.

If the data is read correctly from SRAM, the LED1 is ON, otherwise the LED2 is ON.


@par Directory contents
                       
 - FSMC/FSMC_SRAM/system_stm32f4xx.c   STM32F4xx system clock configuration file
 - FSMC/FSMC_SRAM/stm32f4xx_conf.h     Library Configuration file
 - FSMC/FSMC_SRAM/stm32f4xx_it.c       Interrupt handlers
 - FSMC/FSMC_SRAM/stm32f4xx_it.h       Interrupt handlers header file
 - FSMC/FSMC_SRAM/main.c               Main program
 - FSMC/FSMC_SRAM/main.h               Main program header file

      
@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx and STM32F415xx/417xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices) evaluation boards and can be easily tailored to any 
    other supported device and development board.


@par How to use it ? 

In order to make the program work, you must do the following:
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain 
 - Select the project workspace related to the STM32F40_41xxx device and add 
   the following files in the project source list: 
   - Utilities\STM32_EVAL\STM3240_41_G_EVAL\stm324xg_eval.c
   - Utilities\STM32_EVAL\STM3240_41_G_EVAL\stm324xg_eval_fsmc_sram.c      
 - Rebuild all files and load your image into target memory
 - Run the example

 
 */
