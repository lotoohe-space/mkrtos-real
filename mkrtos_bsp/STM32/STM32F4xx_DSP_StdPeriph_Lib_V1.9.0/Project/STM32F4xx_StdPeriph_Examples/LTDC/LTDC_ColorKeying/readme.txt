/**
  @page LTDC_ColorKeying LTDC Color Keying example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    LTDC/LTDC_ColorKeying/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Description of the LTDC Color Keying example.
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

This example shows how to enable and use the color keying functionality.

After LCD initialization, LCD layer 1 and color keying configuration, an image 
is displayed on LCD.
Color keying is enabled/disabled on the image when the tamper button is pressed.
      
 Color Keying :
 --------------
 If the Color Keying is enabled, all pixels are compared to the color key. 
 If they match the programmed RGB value, all channels (ARGB) of that pixel 
 are set to 0.


@par Directory contents
    
  - LTDC/LTDC_ColorKeying/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - LTDC/LTDC_ColorKeying/stm32f4xx_conf.h     Library Configuration file
  - LTDC/LTDC_ColorKeying/stm32f4xx_it.c       Interrupt handlers
  - LTDC/LTDC_ColorKeying/stm32f4xx_it.h       Interrupt handlers header file
  - LTDC/LTDC_ColorKeying/main.c               Main program
  - LTDC/LTDC_ColorKeying/main.h               Main program header file


@par Hardware and Software environment
  
  - This example runs on and STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM32429I-EVAL 
    (STM32F429xx/STM32F439xx Devices) evaluation boards and can be easily 
    tailored to any other supported device and development board.
  
  - This example has been tested with STMicroelectronics STM324x9I-EVAL RevB 
    board which includes the MB1046 LCD board.
 

@par How to use it ?

In order to make the program work, you must do the following:
 - Copy all source files from this example folder to the template folder under
   Project\STM32F4xx_StdPeriph_Templates
 - Open your preferred toolchain
  - Select STM32F429_439xx workspace and Add the following files in the project 
    source list :
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval.c
     
 - Rebuild all files and load your image into target memory
 - Run the example
  
 
 */
                                   