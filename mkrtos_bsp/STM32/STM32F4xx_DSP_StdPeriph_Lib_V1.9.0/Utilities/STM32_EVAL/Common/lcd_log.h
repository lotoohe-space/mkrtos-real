/**
  ******************************************************************************
  * @file    lcd_log.h
  * @author  MCD Application Team
  * @version V5.1.1
  * @date    13-October-2021
  * @brief   header for the lcd_log.c file
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
#ifndef  __LCD_LOG_H__
#define  __LCD_LOG_H__

/* Includes ------------------------------------------------------------------*/

#include "lcd_log_conf.h"

/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup Common
  * @{
  */

/** @addtogroup LCD_LOG
  * @{
  */
  
/** @defgroup LCD_LOG
  * @brief 
  * @{
  */ 


/** @defgroup LCD_LOG_Exported_Defines
  * @{
  */ 
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/** These value can be changed by user */

#ifdef LCD_SCROLL_ENABLED
 #define     LCD_CACHE_DEPTH     (YWINDOW_SIZE + CACHE_SIZE)
#else
 #define     LCD_CACHE_DEPTH     YWINDOW_SIZE
#endif

#if defined(USE_STM324x9I_EVAL)
  #define LCD_PIXEL_WIDTH         0x1E0    /* 480 */
  #define LCD_PIXEL_HEIGHT        0x110    /* 272 */
#else
  #define LCD_PIXEL_WIDTH         0x0140   /* 320 */
  #define LCD_PIXEL_HEIGHT        0x00F0   /* 240 */
#endif


/**
  * @}
  */ 

/** @defgroup LCD_LOG_Exported_Types
  * @{
  */ 
typedef struct _LCD_LOG_line
{
  uint8_t  line[XWINDOW_MAX];
  uint16_t color;

}LCD_LOG_line;

/**
  * @}
  */ 

/** @defgroup LCD_LOG_Exported_Macros
  * @{
  */ 
#define  LCD_ErrLog(...)    LCD_LineColor = Red;\
                            printf("ERROR: ") ;\
                            printf(__VA_ARGS__);\
                            LCD_LineColor = LCD_LOG_DEFAULT_COLOR

#define  LCD_UsrLog(...)    LCD_LineColor = LCD_LOG_DEFAULT_COLOR;\
                            printf(__VA_ARGS__);\


#define  LCD_DbgLog(...)    LCD_LineColor = Cyan;\
                            printf(__VA_ARGS__);\
                            LCD_LineColor = LCD_LOG_DEFAULT_COLOR
/**
  * @}
  */ 

/** @defgroup LCD_LOG_Exported_Variables
  * @{
  */ 
extern uint16_t LCD_LineColor;
/**
  * @}
  */ 

/** @defgroup LCD_LOG_Exported_FunctionsPrototype
  * @{
  */ 
void LCD_LOG_Init(void);
void LCD_LOG_DeInit(void);
void LCD_LOG_SetHeader(uint8_t *Title);
void LCD_LOG_SetFooter(uint8_t *Status);
void LCD_LOG_ClearTextZone(void);
#ifdef LCD_SCROLL_ENABLED
 ErrorStatus LCD_LOG_ScrollBack(void);
 ErrorStatus LCD_LOG_ScrollForward(void);
#endif
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

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */  

