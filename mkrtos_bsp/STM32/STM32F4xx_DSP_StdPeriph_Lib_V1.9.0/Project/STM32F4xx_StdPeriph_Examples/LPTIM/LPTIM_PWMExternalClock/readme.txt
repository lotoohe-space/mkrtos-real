/**
  @page LPTIM_PWMExternalClock LPTIM PWM External clock example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    LPTIM/LPTIM_PWMExternalClock/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Description of the LPTIM PWM with an External clock example.
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

This example describes how to configure and use LPTIM to generate a PWM at the
lowest power consumption, using an external counter clock.

The Autorelaod equal to 999 so the output frequency (OutputFrequency) will
be equal to the external counter clock (InputFrequency) divided by (999+1).

  OutputFrequency = InputFrequency / (Autoreload + 1)
                  = InputFrequency / 1000

Pulse value equal to 499 and the duty cycle (DutyCycle) is computed as follow:

  DutyCycle = 1 - [(PulseValue + 1)/ (Autoreload + 1)]
  DutyCycle = 50%

To minimize the power consumption, after starting generating the PWM signal,
the MCU enters in STOP mode. Note that GPIOs are configured in Low Speed to
lower the consumption.

User push-button pin (PC.13)is configured as input with external interrupt (External line 13),
falling edge. When User push-button is pressed, wakeup event is generated and PWM signal
generation is stopped and LED2 is on.

@par Directory contents  

  - LPTIM/LPTIM_PWMExternalClock/stm32f4xx_conf.h        Library Configuration file
  - LPTIM/LPTIM_PWMExternalClock/stm32f4xx_it.h          Interrupt handlers header file
  - LPTIM/LPTIM_PWMExternalClock/main.h                  Header for main.c module  
  - LPTIM/LPTIM_PWMExternalClock/stm32f4xx_it.c          Interrupt handlers
  - LPTIM/LPTIM_PWMExternalClock/main.c                  Main program
  - LPTIM/LPTIM_PWMExternalClock/system_stm32f4xx.c      STM32F4xx system source file


@par Hardware and Software environment

  - This example runs on STM32F410xx devices.
    
  - This example has been tested with STMicroelectronics STM32F410xx-Nucleo Rev C
    board and can be easily tailored to any other supported device
    and development board.   
	
  - Connect a clock signal(10kHz) to PB.05 (connected to D4 (pin 29 in CN10 connector)).
  - Connect PC.01 to an oscilloscope (connected to A4 (pin 36 in CN8) to monitor 
    the LPTIM output waveform(10Hz PWM)

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred tool chain 
 - Rebuild all files and load your image into target memory
 - Run the example 

 
 */
