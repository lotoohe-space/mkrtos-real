/**
 * @file GUI_interface_extern.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "GUI_interface_extern.h"
#include "x_tool.h"
#include "color.h"
#include <u_sys.h>
#include <stdio.h>
// extern uint32_t GetCurrentTimeMsec(void);
void lcd_draw_pixel(int x, int y, uint16_t color);
uint32_t GUIGetTick(void)
{
	// return (uint32_t)GetCurrentTimeMsec();
	return sys_read_tick();
}
void GUIDrawPixel(int16_t x, int16_t y, uintColor color)
{
	lcd_draw_pixel(x, y, color);
	// d_pix(x, y, color);
}
uintColor GUIGetPixel(int16_t x, int16_t y)
{
	// return g_pix(x, y);
	assert(0);
	return 0;
}
extern void DMA2D_GUIDrawRect(int16_t x, int16_t y, uint16_t w, uint16_t h, uintColor color);
void GUIDrawRect(int16_t x, int16_t y, uint16_t w, uint16_t h, uintColor color)
{
	// DMA2D_GUIDrawRect(x, y, w, h, color);
	int16_t i, j;

	for (j = y; j < y + h; j++)
	{
		for (i = x; i < x + w; i++)
		{
			GUIDrawPixel(i, j, color);
		}
	}
}
extern void DMA2D_GUIDrawBitmap(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t *bitmap);
void GUIDrawBitmap(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t *bitmap)
{
	if (w * h < 100)
	{
		int i, j;

		for (j = y; j < y + h; j++)
		{
			for (i = x; i < x + w; i++)
			{
				GUIDrawPixel(i, j, ((uint16_t *)(bitmap))[(i - x) + (j - y) * w]);
			}
		}
	}
	else
	{
		DMA2D_GUIDrawBitmap(x, y, w, h, bitmap);
	}
}
