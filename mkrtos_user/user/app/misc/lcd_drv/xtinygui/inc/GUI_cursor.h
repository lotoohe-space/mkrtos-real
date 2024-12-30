/**
 * @file GUI_cursor.h
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
#include "bitmap.h"
#include "x_tool.h"
#include "widget_define.h"

typedef struct
{
	xbitmap_t GUICursorPTBitmap;
	xpoint_t cursorPOI;			/*游标的位置*/
	uint8_t flag;					/*0bit是否显示 */
} *p_gui_cursor_t, gui_cursor_t;

/*是否显示*/
#define _SET_CURSOR_SHOW(a) _SET_BIT((((p_gui_cursor_t)(a))->flag), 0)
#define _SET_CURSOR_HIDE(a) _CLR_BIT((((p_gui_cursor_t)(a))->flag), 0)
#define _GET_CURSOR_USE_STATUS(a) _GET_BIT((((p_gui_cursor_t)(a))->flag), 0)


uint8_t GUICursorInit(void);
void GUICursorSetPOI(int16_t x, int16_t y);
void GUICurosrUpdate(void);
