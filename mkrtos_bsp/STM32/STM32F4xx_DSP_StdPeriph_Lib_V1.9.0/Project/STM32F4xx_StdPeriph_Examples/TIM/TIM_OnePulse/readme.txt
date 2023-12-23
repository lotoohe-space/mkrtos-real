/**
  @page TIM_OnePulse TIM One Pulse example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    TIM/TIM_OnePulse/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Description of the TIM One Pulse example.
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

This example shows how to use the TIM peripheral to generate a One pulse Mode 
after a Rising edge of an external signal is received in Timer Input pin.

TIM2CLK = SystemCoreClock/2, we want to get TIM2 counter clock at 42 MHz:
- Prescaler = (TIM2CLK / TIM2 counter clock) - 1

SystemCoreClock is set to 168 MHz.

The Autoreload value is 65535 (TIM4->ARR), so the maximum frequency value to 
trigger the TIM4 input is 42000000/65535 = 641 Hz.

The TIM4 is configured as follows: 
The One Pulse mode is used, the external signal is connected to TIM4 CH2 pin (PB.07), 
the rising edge is used as active edge, the One Pulse signal is output 
on TIM4_CH1 (PB.06).

The TIM_Pulse defines the delay value, the delay value is fixed to:
delay =  CCR1/TIM4 counter clock = 16383 / 42000000 = 390 us. 
The (TIM_Period - TIM_Pulse) defines the One Pulse value, the pulse value is fixed to:
One Pulse value = (TIM_Period - TIM_Pulse)/TIM4 counter clock 
                = (65535 - 16383) / 42000000 = 1.170 ms.
                

@par Directory contents 
  
  - TIM/TIM_OnePulse/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - TIM/TIM_OnePulse/stm32f4xx_conf.h     Library Configuration file
  - TIM/TIM_OnePulse/stm32f4xx_it.c       Interrupt handlers
  - TIM/TIM_OnePulse/stm32f4xx_it.h       Interrupt handlers header file
  - TIM/TIM_OnePulse/main.c               Main program
  - TIM/TIM_OnePulse/main.h               Main program header file

 
@par Hardware and Software environment 

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx, STM32F427xx/437xx and 
    STM32F429xx/439xx devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices), STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) and 
    STM324x9I-EVAL RevB (STM32F429xx/STM32F439xx Devices) evaluation boards and 
    can be easily tailored to any other supported device and development board.

  - STM324xG-EVAL/STM32437I-EVAL and STM324x9I-EVAL RevB Set-up 
   - Connect the external signal to the TIM4_CH2 pin (PB.07)
   - Connect the TIM4_CH1 (PB.06) pin to an oscilloscope to monitor the waveform.
      
  
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
