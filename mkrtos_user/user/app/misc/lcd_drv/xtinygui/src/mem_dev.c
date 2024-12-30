/**
 * @file mem_dev.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "mem_dev.h"
#include "x_malloc.h"
#include "x_tool.h"
#include "GUI_interface_extern.h"

/*创建一个存储设备*/
p_mem_dev_t MemDevCreate(xrect_t *rect, uint8_t colorDepth)
{
	p_mem_dev_t hMemDev = XMalloc(sizeof(mem_dev_t));

	if (hMemDev == NULL)
	{
		return NULL;
	}
#if USE_MUTL_LAYER
	hMemDev->mem1 = (uint8_t *)XMalloc(rect->w * rect->h * (colorDepth / 8));
	if (hMemDev->mem1 == NULL)
	{
		XFree(hMemDev);
		return NULL;
	}
	hMemDev->mem2 = (uint8_t *)XMalloc(rect->w * rect->h * (colorDepth / 8));
	if (hMemDev->mem2 == NULL)
	{
		XFree(hMemDev->mem1);
		XFree(hMemDev);
		return NULL;
	}
	hMemDev->mem = hMemDev->mem2;
#else
	hMemDev->mem = (uint8_t *)XMalloc(rect->w * rect->h * (colorDepth / 8));
	if (hMemDev->mem == NULL)
	{
		XFree(hMemDev);
		return NULL;
	}
#endif
	hMemDev->rect = *rect;
#if USE_FRAMEBUFFER_MODE
	hMemDev->r_rect = *rect;
#endif
	hMemDev->memSize = rect->w * rect->h * (colorDepth / 8);

	hMemDev->rawPoint.x = rect->x;
	hMemDev->rawPoint.y = rect->y;

	hMemDev->colorDepth = colorDepth;

	return hMemDev;
}
#if USE_MUTL_LAYER
void MemdevSw(p_mem_dev_t hMemdev)
{
	if (hMemdev->mem == hMemdev->mem1)
	{
		hMemdev->mem = hMemdev->mem2;
	}
	else
	{
		hMemdev->mem = hMemdev->mem1;
	}
}
#endif
/*重新设置MemDev的大小，*/
uint8_t MemDevReset(p_mem_dev_t hMemDev, int16_t x, int16_t y, uint16_t w, uint16_t h)
{
	assert(hMemDev);
	if (w == 0 || h == 0)
	{
		return FALSE;
	}
	hMemDev->rect.x = x;
	hMemDev->rect.y = y;
	hMemDev->rect.w = w;
	hMemDev->rect.h = MIN(hMemDev->memSize / w / (hMemDev->colorDepth / 8), h); /*重新设置高度，向下取整*/

	if (hMemDev->rect.h == 0)
	{
		/*高度为零，设置失败*/
		return FALSE;
	}
	return TRUE;
}
void MemDevCopyToLCD(p_mem_dev_t hMemDev)
{
	assert(hMemDev);
	GUIDrawBitmap(hMemDev->rect.x, hMemDev->rect.y, hMemDev->rect.w, hMemDev->rect.h, hMemDev->mem);
}
void *MemDevGetPT(p_mem_dev_t hMemdev)
{
	return hMemdev->mem;
}