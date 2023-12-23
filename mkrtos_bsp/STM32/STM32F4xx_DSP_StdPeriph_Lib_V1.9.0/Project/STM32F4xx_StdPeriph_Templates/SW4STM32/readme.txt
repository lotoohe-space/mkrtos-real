/**
  @page sw4stm32 SW4STM32 Project Template for STM32F4xx devices
  
  @verbatim
  ******************** (C) COPYRIGHT 2016 STMicroelectronics *******************
  * @file    readme.txt
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   This sub-directory contains all the user-modifiable files needed to 
  *          create a new project linked with the STM32F4xx Standard Peripherals  
  *          Library and working with RealView Microcontroller Development Kit(MDK-ARM)
  *          software toolchain.
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
 
@par Directory contents


@par How to use it ?
 
 - Open the Project.uvprojx project
 - In the workspace toolbar select the project config:
    - STM32F429_439xx: to configure the project for STM32F429xx/439xx devices.
     @note The needed define symbols for this config are already declared in the
           preprocessor section: USE_STM324x9I_EVAL, STM32F429_439xx, USE_STDPERIPH_DRIVER

    - STM32F427_437xx: to configure the project for STM32F427xx/437xx devices.
     @note The needed define symbols for this config are already declared in the
           preprocessor section: USE_STM324x7I_EVAL, STM32F427_437xx, USE_STDPERIPH_DRIVER
           
     - STM32F40_41xxx: to configure the project for STM32F40/41xxx devices.
     @note The needed define symbols for this config are already declared in the
           preprocessor section: USE_STM324xG_EVAL, STM32F40_41xxx, USE_STDPERIPH_DRIVER
           
     - STM32F401xx: to configure the project for STM32F401xx devices.
     @note The needed define symbols for this config are already declared in the
           preprocessor section: STM32F401xx, USE_STDPERIPH_DRIVER

     - STM32F410xx: to configure the project for STM32F410xx devices.
     @note The needed define symbols for this config are already declared in the
           preprocessor section: STM32F410xx, USE_STDPERIPH_DRIVER

     - STM32F411xx: to configure the project for STM32F411xx devices.
     @note The needed define symbols for this config are already declared in the
           preprocessor section: STM32F411xx, USE_STDPERIPH_DRIVER

     - STM32F412xx: to configure the project for STM32F412xx devices.
     @note The needed define symbols for this config are already declared in the
           preprocessor section: STM32F412xx, USE_STDPERIPH_DRIVER

     - STM32F446xx: to configure the project for STM32F446xx devices.
     @note The needed define symbols for this config are already declared in the
           preprocessor section: STM32F446xx, USE_STDPERIPH_DRIVER

     - STM32F469_479xx: to configure the project for STM32F469xx and STM32F479xx devices.
     @note The needed define symbols for this config are already declared in the
           preprocessor section: STM32F469_479xx, USE_STDPERIPH_DRIVER

     - STM32F413_423xx: to configure the project for STM32F413xx and STM32F423xx devices.
     @note The needed define symbols for this config are already declared in the
           preprocessor section: STM32F413_423xx, USE_STDPERIPH_DRIVER

 - Rebuild all files: Project->Rebuild all target files
 - Load project image: Debug->Start/Stop Debug Session
 - Run program: Debug->Run (F5)
    
 
 */
