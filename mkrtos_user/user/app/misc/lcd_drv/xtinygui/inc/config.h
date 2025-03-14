/**
 * @file config.h
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#define GUI_COLOR_DEEP (16) /*color deep*/

#define USE_ALPHA 1   /*是否开启透明通道,开启透明通道会降低运行速度*/
#define USE_MEM_DEV 1 /*是否开启存储设备*/
#define USE_CURSOR 1  /*是否开启游标*/
#define USE_GPU 1     /*是否支持GPU*/

#if USE_MEM_DEV
#define USE_FRAMEBUFFER_MODE 0
#define USE_MUTL_LAYER 0
#endif

/*显示屏的大小*/
#define LCD_SCREEN_W 480
#define LCD_SCREEN_H 800

#if USE_FRAMEBUFFER_MODE == 0
#define LCD_CACHE_LINE (LCD_SCREEN_H) /*缓冲区行数*/
#endif
