/**
 * @file color.h
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#define RGB565_WHITE 0xffff //!< 白色
#define RGB565_BLACK 0x0000 //!< 黑色
#define RGB565_RED 0xf800   //!< 红色
#define RGB565_GREEN 0x07e0 //!< 绿色
#define RGB565_BLUE 0x001f  //!< 蓝色

#define RGB565TORGB888(n565Color) (((((n565Color) & RGB565_RED) >> 8) << 16) |  \
                                   ((((n565Color) & RGB565_GREEN) >> 3) << 8) | \
                                   ((((n565Color) & RGB565_BLUE) << 3))) //!< RGB565转RGB888

#define RGB888T0RGB565(a) (((((a) & 0x00ff0000) >> 19) << 11) | \
                           ((((a) & 0x0000ff00) >> 10) << 5) |  \
                           ((((a) & 0x000000ff) >> 3) << 0)) //!< RGB888转RGB565
#define BGR888T0RGB565(a) (((((a) & 0x00ff0000) >> 19) >> 0) | \
                           ((((a) & 0x0000ff00) >> 10) << 5) | \
                           ((((a) & 0x000000ff) >> 3) << 11)) //!< BGR888转RGB565

#define C565A(a) (((a) & 0x00ff0000) >> 16)
#define C565R(a) ((((a) & RGB565_RED) >> 11) << 3)
#define C565G(a) ((((a) & RGB565_GREEN) >> 5) << 2)
#define C565B(a) ((((a) & RGB565_BLUE) >> 0) << 3)

#define RGB565_GEN(r, g, b) (((((r) >> 3) & 0x1f) << 11) | ((((g) >> 2) & 0x3f) << 5) | (((b) >> 3) & 0x1f))
#define ARGB565_GEN(a, r, g, b) (((a & 0xff) << 16) | RGB565_GEN(r, g, b))

#define _DefaultFrColor RGB565_GEN(80, 80, 80)
#define _DefaultBKColor RGB565_GEN(230, 235, 230)
