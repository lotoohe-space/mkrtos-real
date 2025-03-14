/**
 * @file x_types.h
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include "config.h"
#include <stdint.h>
#include <stddef.h>
#include <u_types.h>

typedef struct
{
	uint8_t a;
	uint16_t r : 5;
	uint16_t g : 6;
	uint16_t b : 5;
} *p_argb565_t, argb565_t;

typedef struct
{
	uint16_t r : 5;
	uint16_t g : 6;
	uint16_t b : 5;
} *p_rgb565_t, rgb565_t;

typedef struct
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} *p_rgb_t, rgb_t;

typedef struct
{
	uint8_t a;
	uint8_t r;
	uint8_t g;
	uint8_t b;
} *p_argb_t, argb_t;

#if GUI_COLOR_DEEP <= 8
typedef uint8_t uintColor;
#elif GUI_COLOR_DEEP > 8 && GUI_COLOR_DEEP <= 16
typedef uint32_t uintColor; /*Argb rgb 16bit*/
#elif GUI_COLOR_DEEP >= 24
typedef uint32_t uintColor;
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE (!TRUE)
#endif

#define BOOL uint8_t


#define PUBLIC
#define PRIVATE static

#ifdef WIN32
#define _INLINE_ _inline
#else
#define _INLINE_ inline
#endif
