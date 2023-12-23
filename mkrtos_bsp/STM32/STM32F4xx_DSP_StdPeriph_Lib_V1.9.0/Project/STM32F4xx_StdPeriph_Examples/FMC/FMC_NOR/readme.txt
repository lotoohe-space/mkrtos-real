/**
  @page FMC_NOR NOR flash memory functionalities use example
  
  @verbatim
  ******************* (C) COPYRIGHT 2016 STMicroelectronics ********************
  * @file    FMC/FMC_NOR/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-20222
  * @brief   Description of the FMC NOR example.
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

This example shows how to perform erase/read/write operations on the M29W128GL 
NOR memory mounted on the STM324x9I-EVAL RevB board. It uses an associate driver 
developed to interface with the indicated NOR flash memory device.

The example follows a set of NOR operations in order to write and verify a data
buffer to the NOR external memory. For more details about NOR commands interface
and functionalities, refer to the NOR memory datasheet.

After correctly reading the manufacturer and device IDs, the program returns to 
NOR read mode and erases the block where the write/read start data exists. A data
buffer of half words is written to the memory. Finally, the data written is read 
back to check its correctness.

The user can choose his own configuration by commenting/uncommenting the defines for  
undesired/desired configurations in the memory associate driver's header file, 
for example, to enable Continuous clock feature, uncomment the define for the 
configuration "FMC_CClock_SyncAsync" in "stm324x9i_eval_fmc_nor.h" file.
  
If the data is read correctly, the LED1 is ON, otherwise the LED2 is ON. 


@par Directory contents                  
 
  - FMC/FMC_NOR/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - FMC/FMC_NOR/stm32f4xx_conf.h     Library Configuration file
  - FMC/FMC_NOR/stm32f4xx_it.c       Interrupt handlers
  - FMC/FMC_NOR/stm32f4xx_it.h       Interrupt handlers header file
  - FMC/FMC_NOR/main.c               Main program
  - FMC/FMC_NOR/main.h               Main program header file
  

@par Hardware and Software environment 

  - This example runs on STM32F429xx/439xx devices.
   
  - This example has been tested with STMicroelectronics STM324x9I-EVAL RevB 
    (STM32F429xx/STM32F439xx Devices) evaluation boards and can be easily 
    tailored to any other supported device and development board.
    
  - STM324x9I-EVAL RevB Set-up 
    - Make sure that the Jumper JP9 is open.


@par How to use it ? 

In order to make the program work, you must do the following:
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain 
 - Select the project workspace related to the STM32F429_439xx device and add 
   the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval.c
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_fmc_nor.c
            
 - Rebuild all files and load your image into target memory
 - Run the example 

 
 */
