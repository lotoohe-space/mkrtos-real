/**
  @page CortexM_Mode_Privilege CortexM Mode Privilege example
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    CortexM4/Mode_Privilege/readme.txt 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Description of the CortexM Mode Privilege example.
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

This example shows how to modify CortexM4 Thread mode privilege access and stack.
CortexM4 Thread mode is entered on Reset, and can be entered as a result of an
exception return.  

The associated program is used to:

1)Switch the Thread mode stack from Main stack to Process stack

2)Switch the Thread mode from Privileged to Unprivileged

3)Switch the Thread mode from Unprivileged back to Privileged

To monitor the stack used and the privileged or unprivileged access level of code
in Thread mode, a set of variables is available within the program. It is also
possible to use the 'Cortex register' window of the debugger.


@par Directory contents 
 
  - CortexM/Mode_Privilege/system_stm32f4xx.c   STM32F4xx system clock configuration file
  - CortexM/Mode_Privilege/stm32f4xx_conf.h     Library Configuration file
  - CortexM/Mode_Privilege/stm32f4xx_it.c       Interrupt handlers
  - CortexM/Mode_Privilege/stm32f4xx_it.h       Interrupt handlers header file
  - CortexM/Mode_Privilege/main.c               Main program
  
          
@par Hardware and Software environment

  - This example runs on STM32F405xx/407xx, STM32F415xx/417xx and STM32F427xx/437xx 
    devices.
    
  - This example has been tested with STMicroelectronics STM324xG-EVAL (STM32F40xx/
    STM32F41xx Devices) and STM32437I-EVAL (STM32F427xx/STM32F437xx Devices) evaluation 
    boards and can be easily tailored to any other supported device and development board.


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
             
 - Rebuild all files and load your image into target memory
 - Run the example

 
 */
