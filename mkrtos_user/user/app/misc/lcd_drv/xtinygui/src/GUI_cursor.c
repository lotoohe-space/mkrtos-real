/**
 * @file GUI_cursor.c
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "GUI_cursor.h"
#include "color.h"
#include "GUI_interface_extern.h"
#include "paint.h"
#include "x_malloc.h"
#include "gui.h"
/* 7*12 */
const uint32_t GUICursorPT[] = {
	0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0x00000000, 0x00FFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF,
	0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0xFFFFFFFF,
	0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00000000,
	0x00000000, 0x00FFFFFF, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00FFFFFF, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF,
	0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000};

/*游标*/
static gui_cursor_t GUICursor = {0};

/*GUI游标初始化*/
uint8_t GUICursorInit(void)
{
	InitBitmap(&GUICursor.GUICursorPTBitmap, (const uint8_t *)GUICursorPT,
		 7, 12, 32);
	_SET_CURSOR_SHOW(&GUICursor);
	return TRUE;
}

/*
设置光标位置
位置没有改变则不需要操作
1.绘制之前保存的背景
2.重新设置当前位置和上一次的位置
3.然后保存新位置的背景到缓存
*/
void GUICursorSetPOI(int16_t x, int16_t y)
{
	GUIInvailRect(&(xrect_t){
		.x = GUICursor.cursorPOI.x,
		.y = GUICursor.cursorPOI.y,
		.w = GUICursor.GUICursorPTBitmap.w,
		.h = GUICursor.GUICursorPTBitmap.h,
	});
	GUICursor.cursorPOI.x = x;
	GUICursor.cursorPOI.y = y;
}
void GUICurosrUpdate(void)
{
	int16_t i, j;
	uint32_t *pixels = (uint32_t *)(GUICursor.GUICursorPTBitmap.pixels);

	for (i = 0; i < GUICursor.GUICursorPTBitmap.h; i++)
	{
		for (j = 0; j < GUICursor.GUICursorPTBitmap.w; j++)
		{
			uint32_t color = pixels[(i)*GUICursor.GUICursorPTBitmap.w + j];
			if (!(color & 0xff000000))
			{
				// printf("x:%d, y:%d\n", j + GUICursor.cursorPOI.x, i + GUICursor.cursorPOI.y);
				DrawPixel(BGR888T0RGB565(color), j + GUICursor.cursorPOI.x, i + GUICursor.cursorPOI.y);
			}
		}
	}
}

/*设置游标类型*/
void GUICursorSetType(uint8_t type)
{
}

/*显示游标*/
void GUICursorShow(void)
{
	_SET_CURSOR_SHOW(&GUICursor);
}

/*隐藏显示*/
void GUICursorHide(void)
{
	_SET_CURSOR_HIDE(&GUICursor);
}
