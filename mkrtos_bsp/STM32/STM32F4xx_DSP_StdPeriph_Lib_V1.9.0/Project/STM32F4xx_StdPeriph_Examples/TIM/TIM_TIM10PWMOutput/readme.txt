/**
  @page TIM_TIM10PWMOutput TIM10 PWM Output example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    TIM/TIM_TIM10PWMOutput/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Description of the TIM PWM Output example.
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

This example shows how to configure the TIM peripheral in PWM (Pulse Width Modulation) 
mode.

The TIM10CLK frequency is set to SystemCoreClock(Hz), to get TIM10 counter
clock at 21 MHz the Prescaler is computed as following:
   - Prescaler = (TIM10CLK / TIM10 counter clock) - 1
   
SystemCoreClock is set to 168 MHz for STM32F4xx Devices.

The TIM10 is running at 31.530 KHz: TIM10 Frequency = TIM10 counter clock/(ARR + 1)
                                                    = 21 MHz / 666 = 31.530 KHz

The TIM10 CCR1 register value is equal to 333, so the TIM10 Channel 1 generates a 
PWM signal with a frequency equal to 30 KHz and a duty cycle equal to 50%:

TIM10 Channel1 duty cycle = (TIM10_CCR1/ TIM10_ARR + 1)* 100 = 50%

The PWM waveform can be displayed using an oscilloscope.


@par Directory contents 
  
  - TIM/TIM_TIM10PWMOutput/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - TIM/TIM_TIM10PWMOutput/stm32f4xx_conf.h     Library Configuration file
  - TIM/TIM_TIM10PWMOutput/stm32f4xx_it.c       Interrupt handlers
  - TIM/TIM_TIM10PWMOutput/stm32f4xx_it.h       Interrupt handlers header file
  - TIM/TIM_TIM10PWMOutput/main.c               Main program  


@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) and 
    STM324x9I-EVAL RevB (STM32F429xx/STM32F439xx Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.

  - STM324xG-EVAL/STM32437I-EVAL and STM324x9I-EVAL RevB Set-up
    - Connect the following pin to an oscilloscope PF.06: (TIM10_CH1)
    

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
