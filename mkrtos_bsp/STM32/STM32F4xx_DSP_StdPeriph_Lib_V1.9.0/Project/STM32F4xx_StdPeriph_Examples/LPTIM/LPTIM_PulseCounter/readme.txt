/**
  @page LPTIM_PulseCounter Low power timer pulse counter example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    LPTIM/LPTIM_PulseCounter/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Description of the LPTIM Pulse counter example
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

This example describes how to configure and use LPTIM to count pulses.

To reduce power consumption, MCU enters stop mode after starting counting. Each
time the counter reachs the maximum value (Period/Autoreload), an interruption
is generated, the MCU  woke up from stop mode and LED2 toggles the last state.
  
In this example Period value is set to 1000, so each time the counter counts
(1000 + 1) rising edges on LPTIM1_IN1 pin, an interrupt is generated and LED2
toggles.

In this example the internal clock provided to the LPTIM1 is LSI (32 Khz),
so the external input is sampled with LSI clock. In order not to miss any event,
the frequency of the changes on the external Input1 signal should never exceed the
frequency of the internal clock provided to the LPTIM1 (LSI for the
present example).

@Note
After wake up from stop mode, the system clock is driven by HSI, it is monitored
on MCO2 (PC9)
@par Directory contents  

  - LPTIM/LPTIM_PulseCounter/stm32f4xx_hal_conf.h    Library Configuration file
  - LPTIM/LPTIM_PulseCounter/stm32f4xx_it.h          Interrupt handlers header file
  - LPTIM/LPTIM_PulseCounter/main.h                  Header for main.c module  
  - LPTIM/LPTIM_PulseCounter/stm32f4xx_it.c          Interrupt handlers
  - LPTIM/LPTIM_PulseCounter/main.c                  Main program
  - LPTIM/LPTIM_PulseCounter/system_stm32f4xx.c      STM32L4xx system source file


@par Hardware and Software environment

  - This example runs on STM32F410xx devices.
    
  - This example has been tested with STMicroelectronics STM32F410xx-Nucleo Rev C
    board and can be easily tailored to any other supported device
    and development board.

  - Generate pulses on PB5 (pin 29 in CN10 connector). (Connect a square waveform 1kHz)
  
  - Connect MCO2 Pin (PC9) to an oscilloscope

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example 

 
 */
