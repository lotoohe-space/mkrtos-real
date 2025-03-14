/**
 * @file mem_dev.h
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include "x_types.h"
#include "widget_define.h"
#include <assert.h>
#include <config.h>
typedef struct
{
	xrect_t rect; /*存储设备的大小区域*/
#if USE_FRAMEBUFFER_MODE
	xrect_t r_rect; /*真实的大小*/
#endif
	xpoint_t rawPoint; /*原始起始位置*/
	uint8_t *mem;	   /*内存空间*/
#if USE_MUTL_LAYER
	uint8_t *mem1;
	uint8_t *mem2;
#endif
	uint32_t memSize; /*内存大小*/

	uint8_t colorDepth; /*颜色深度*/
} *p_mem_dev_t, mem_dev_t;

static inline void MemDevDrawPT(p_mem_dev_t hMemDev, int16_t x, int16_t y, uintColor color)
{
	assert(hMemDev);
#if USE_FRAMEBUFFER_MODE
	((uint16_t *)(hMemDev->mem))[x + y * hMemDev->r_rect.w] = (uint16_t)color;
#else
	((uint16_t *)(hMemDev->mem))[x + y * hMemDev->rect.w] = (uint16_t)color;
#endif
}
static inline uintColor MemDevReadPT(p_mem_dev_t hMemDev, int16_t x, int16_t y)
{
	assert(hMemDev);
#if USE_FRAMEBUFFER_MODE
	return ((uint16_t *)(hMemDev->mem))[x + y * hMemDev->r_rect.w];
#else
	return ((uint16_t *)(hMemDev->mem))[x + y * hMemDev->rect.w];
#endif
}

p_mem_dev_t MemDevCreate(xrect_t *rect, uint8_t colorDepth);
#if USE_MUTL_LAYER
void MemdevSw(p_mem_dev_t hMemdev);
#endif
uint8_t MemDevReset(p_mem_dev_t hMemDev, int16_t x, int16_t y, uint16_t w, uint16_t h);
void MemDevDrawPT(p_mem_dev_t hMemDev, int16_t x, int16_t y, uintColor color);
uintColor MemDevReadPT(p_mem_dev_t hMemDev, int16_t x, int16_t y);
void MemDevCopyToLCD(p_mem_dev_t hMemDev);
void *MemDevGetPT(p_mem_dev_t hMemdev);
