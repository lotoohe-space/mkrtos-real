/**
  @page QSPI_ReadWrite_DMA example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    QSPI/QSPI_ReadWrite_DMA/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Description of the QSPI Read/Write DMA example.
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

This example describes how to use the QSPI firmware library to perform Read/Write DMA operations with 
QSPI FLASH device.

In this example,the QSPI is interfacing with SPANSION S25FL512S FLASH or MICRON MT25QL512AB FLASH memory
and up to user to select the right flash connected on the STM32446E-EVAL/STM32469I-EVAL/STM32412G-DISCO/STM32F413H-DISCO board
by uncommenting the required line in main.h:
- QSPI_FLASH_SPANSION: QSPI FLASH Spansion
- QSPI_FLASH_MICRON: QSPI FLASH Micron

The QSPI is configured in indirect mode for writing and reading operations, and in 
Autopolling mode after each write operation to wait for the end of the command.

This example erase part of the QSPI memory, write data, read data using DMA
and compare the result in a forever loop.

Once all data buffer are received.A comparison is done and TransferStatus variable gives 
the data transfer status where it is PASSED if transmitted and received data are the same 
otherwise it is FAILED.

@note
The QSPI Clock is configured to reach maximum frequency at 90 MHZ.

@par Directory contents 

  - QSPI/QSPI_ReadWrite_DMA/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - QSPI/QSPI_ReadWrite_DMA/stm32f4xx_conf.h     Library Configuration file
  - QSPI/QSPI_ReadWrite_DMA/stm32f4xx_it.c       Interrupt handlers
  - QSPI/QSPI_ReadWrite_DMA/stm32f4xx_it.h       Interrupt handlers header file
  - QSPI/QSPI_ReadWrite_DMA/main.c               Main program
  - QSPI/QSPI_ReadWrite_DMA/main.h               Header for main.c module  

@par Hardware and Software environment 
 
  - This example runs on STM32F446xx , STM32F469/479xx , STM32F412xG and STM32F413/423xx devices.

  - This example has been tested with STMicroelectronics STM32446E-EVAL 
    (STM32F446xx Devices), STM32412G-DISCO (STM32F412xG devices), STM32F413H-DISCO
    (STM32F413_423xx devices) and STM32469I-EVAL(STM32F469xx Devices) evaluation boards RevA
    and can be easily tailored to any other supported device and development board.

@par How to use it ?

In order to make the program work, you must do the following:
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
  
 
 */