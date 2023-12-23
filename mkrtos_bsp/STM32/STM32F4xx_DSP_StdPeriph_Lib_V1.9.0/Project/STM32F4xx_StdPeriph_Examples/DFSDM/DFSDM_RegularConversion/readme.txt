/**
  @page DFSDM Regular Conversion Example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    DFSDM/DFSDM_RegularConversion/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Description of the TEMPLATE example
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

This example describes how to use the DFSDM firmware library to perform a regular
conversion in polling mode.

In this example, we will use DFSDM Channel 0 and filter 0 to convert data coming from
DFSDM_DATIN0.

The calculation of the expected value depends on these parameters :
- Filter Oversampling ratio : 100
- Power of sinc order : 1
- Channel Data shift : 2
- Channel offset : 50

The DFSDM_CKOUT(PC2) is configurated to deliver System clok frequency divided 
by an internal DFSDM divider (5 in our example).

The user have to connect a 3.3v signal in DFSDM DataIn0 (PB1).

Once the conversion is done. A comparison is done and ConversionStatus variable gives 
the conversion status where it is PASSED if transmitted and received data are the same 
otherwise it is FAILED.
 
@par Directory contents

  - DFSDM/DFSDM_RegularConversion/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - DFSDM/DFSDM_RegularConversion/stm32f4xx_conf.h     Library Configuration file
  - DFSDM/DFSDM_RegularConversion/stm32f4xx_it.c       Interrupt handlers
  - DFSDM/DFSDM_RegularConversion/stm32f4xx_it.h       Interrupt handlers header file
  - DFSDM/DFSDM_RegularConversion/main.c               Main program
  - DFSDM/DFSDM_RegularConversion/main.h               Main program header file

@par Hardware and Software environment

  - This example runs on STM32F412xG devices.

  - This example has been tested with STM32F412G-DISCO board RevC and can be 
    easily tailored to any other supported device and development board.
    
  - STM32F412G-DISCO RevA Set-up :
     - Connect 3.3V on PB1 (DFSDM_DataIn0)
     - Connect PC2(DFSDM_CKOUT) on oscilloscope.  

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

 
 */
 