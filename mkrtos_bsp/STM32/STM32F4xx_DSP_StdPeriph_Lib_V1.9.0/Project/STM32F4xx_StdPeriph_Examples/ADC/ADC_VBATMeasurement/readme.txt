/**
  @page ADC_VBATMeasurement ADC VBAT Measurement example

  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    ADC/ADC_VBATMeasurement/readme.txt
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Description of the ADC1 VBAT Measurement example.
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

This example shows how to measure VBAT voltage using ADC1 regular channel 18.
In fact, the measured value corresponds to VBAT/2 for STM32F40xx/STM32F41xx 
devices and to VBAT/4 for STM32F42xx/STM32F43xx devices; as the VBAT voltage 
could be higher than VDDA, to ensure the correct operation of the ADC, the VBAT 
pin is internally connected to a bridge divider by 2 for STM32F40xx/STM32F41xx 
devices and by 4 for STM32F42xx/STM32F43xx devices.

In this example the VBAT/2 voltage for STM32F40xx/STM32F41xx devices or VBAT/4 voltage 
for STM32F42xx/STM32F43xx devices is continuously converted, each time an end
of conversion occurs the DMA transfers, in circular mode, the converted data from 
ADC1 DR register to the uwADCConvertedValue variable.  
However, in real application to prevent any unwanted consumption on the battery,
it is recommended to enable the bridge divider only when needed for ADC
conversion then disable it.

User can display the VBAT voltage on the Eval Board LCD (when the define USE_LCD
is enabled in main.h  file)

In this example, the system clock is 144MHz, APB2 =72MHz and ADC clock = APB2 /2.
So ADCCLK= 36MHz 

Conversion rate of Vbat channel  = Sampling Time + Conversion Time = 15 + 12 cycles
                                 = 27 cycles ==> Conversion Time = 36MHz/27cyc = 1.33Msps.


@par Directory contents 

  - ADC/ADC_VBATMeasurement/system_stm32f4xx.c   STM32F4xx system clock configuration file 
  - ADC/ADC_VBATMeasurement/stm32f4xx_conf.h     Library Configuration file
  - ADC/ADC_VBATMeasurement/stm32f4xx_it.c       Interrupt handlers
  - ADC/ADC_VBATMeasurement/stm32f4xx_it.h       Interrupt handlers header file
  - ADC/ADC_VBATMeasurement/main.c               Main program
  - ADC/ADC_VBATMeasurement/main.h               Main program header file
  
  
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
     - Utilities\STM32_EVAL\STM3240_41_G_EVAL\stm324xg_eval_lcd.c 
     - Utilities\STM32_EVAL\STM3240_41_G_EVAL\stm324xg_eval_fsmc_sram.c
     - Utilities\STM32_EVAL\STM3240_41_G_EVAL\stm324xg_eval_ioe.c 
      
   - If "STM32F427_437xx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x7I_EVAL\stm324x7i_eval.c
     - Utilities\STM32_EVAL\STM324x7I_EVAL\stm324x7i_eval_lcd.c 
     - Utilities\STM32_EVAL\STM324x7I_EVAL\stm324x7i_eval_fmc_sram.c
     - Utilities\STM32_EVAL\STM324x7I_EVAL\stm324x7i_eval_ioe.c
     
   - If "STM32F429_439xx" is selected as default project Add the following files in the project source list:
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval.c
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_lcd.c 
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_fmc_sdram.c
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_ioe8.c   
     - Utilities\STM32_EVAL\STM324x9I_EVAL\stm324x9i_eval_ioe16.c 
            
 - Rebuild all files and load your image into target memory
 - Run the example

 
 */
 