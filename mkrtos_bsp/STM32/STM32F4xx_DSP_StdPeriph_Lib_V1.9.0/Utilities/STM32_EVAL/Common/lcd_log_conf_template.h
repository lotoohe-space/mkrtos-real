/**
  ******************************************************************************
  * @file    lcd_log_conf_template.h
  * @author  MCD Application Team
  * @version V5.1.1
  * @date    13-October-2021
  * @brief   lcd_log configuration template file.
  *          This file should be copied to the application folder and modified 
  *          as follows:
  *            - Rename it to 'lcd_log_conf.h'.
  *            - Update the name of the LCD header file depending on the EVAL board
  *              you are using (see line32 below).
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef  __LCD_LOG_CONF_H__
#define  __LCD_LOG_CONF_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32xxx_eval_lcd.h" /* replace 'stm32xxx' with your EVAL board name, ex: stm3210c_eval_lcd.h */
#include <stdio.h>

/** @addtogroup LCD_LOG
  * @{
  */
  
/** @defgroup LCD_LOG
  * @brief This file is the 
  * @{
  */ 


/** @defgroup LCD_LOG_CONF_Exported_Defines
  * @{
  */ 
/* Comment the line below to disable the scroll back and forward features */
#define LCD_SCROLL_ENABLED

/* Define the LCD default text color */
#define LCD_LOG_DEFAULT_COLOR    LCD_COLOR_WHITE

/* Define the display window settings */
#define YWINDOW_MIN              3
#define YWINDOW_SIZE             12
#define XWINDOW_MAX              50

/* Define the cache depth */
#define CACHE_SIZE               50

/** @defgroup LCD_LOG_CONF_Exported_TypesDefinitions
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup LCD_LOG_Exported_Macros
  * @{
  */ 


/**
  * @}
  */ 

/** @defgroup LCD_LOG_CONF_Exported_Variables
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup LCD_LOG_CONF_Exported_FunctionsPrototype
  * @{
  */ 

/**
  * @}
  */ 


#endif /* __LCD_LOG_H__ */

/**
  * @}
  */ 

/**
  * @}
  */ 

