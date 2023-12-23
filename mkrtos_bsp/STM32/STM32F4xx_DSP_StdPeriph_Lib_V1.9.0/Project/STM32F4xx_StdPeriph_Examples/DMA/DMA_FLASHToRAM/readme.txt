/**
  @page DMA_FLASHToRAM DMA FLASH To RAM example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    DMA/DMA_FLASHToRAM/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Description of the DMA FLASH to RAM example.
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

This example shows how to use a DMA channel to transfer a word data buffer from 
FLASH memory to embedded SRAM memory.

DMA2 Stream0 channel0 is configured to transfer the contents of a 32-word data 
buffer stored in Flash memory to the reception buffer declared in RAM.

The start of transfer is triggered by software. DMA2 Stream0 channel0 memory-to-memory
transfer is enabled. Source and destination addresses incrementing is also enabled.
The transfer is started by setting the Channel enable bit for DMA2 Stream0 channel0.
At the end of the transfer a Transfer Complete interrupt is generated since it
is enabled. The Transfer Complete Interrupt pending bit is then cleared. 
When the DMA transfer is completed the DMA Stream is disabled by hardware.
The main application can check on the Stream Enable status to detect the end of
transfer or can also check on the number of remaining transfers which should be 
equal to 0 at the end of the transfer. 
A comparison between the source and destination buffers is done to check that all 
data have been correctly transferred.

STM32 Eval board's LEDs can be used to monitor the transfer status:
 - LED1 is ON when the program starts.
 - LED2 is ON when the configuration phase is done and the transfer is started.
 - LED3 is ON when the transfer is complete (into the Transfer Complete interrupt
   routine)
 - LED4 is ON when the comparison result between source buffer and destination 
   buffer is passed.

It is possible to select a different Stream and/or channel for the DMA transfer
example by modifying defines values in the file main.h.
Note that only DMA2 Streams are able to perform Memory-to-Memory transfers.

There are different options to check on the DMA end of transfer:
1)Use DMA Transfer Complete interrupt.
2)Use DMA enable state (the DMA stream is disabled by hardware when transfer 
  is complete).
3)Use DMA Stream transfer counter value (the counter value is decremented when
  transfer is ongoing and is equal to 0 at the transfer end).
4)Use DMA Transfer Complete flag (polling mode).

In this example methods 1, 2 and 3 are provided (you can select between method 2 
and 3 by uncommenting relative code in waiting loop in the main.c file).


@par Directory contents 

  - DMA/DMA_FLASHToRAM/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - DMA/DMA_FLASHToRAM/stm32f4xx_conf.h     Library Configuration file
  - DMA/DMA_FLASHToRAM/stm32f4xx_it.c       Interrupt handlers
  - DMA/DMA_FLASHToRAM/stm32f4xx_it.h       Interrupt handlers header file
  - DMA/DMA_FLASHToRAM/main.c               Main program
  - DMA/DMA_FLASHToRAM/main.h               Main program header file
  
     
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
