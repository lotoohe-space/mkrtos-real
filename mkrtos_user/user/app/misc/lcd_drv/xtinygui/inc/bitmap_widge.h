/**
 * @file bitmap_widge.h
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include "x_widget.h"
#include "bitmap.h"
#include "xwindows.h"

#define BITMAP_MARK_HEAD(a) BitmapWidget##a

typedef struct
{
	widget_base_t widgeBase;
	p_xbitmap_t bitmap; /*显示的图片*/
} *p_bitmap_widget_t, bitmap_widget_t;

PUBLIC p_bitmap_widget_t BitmapWidgetCreate(int16_t x, int16_t y, int16_t w, int16_t h, p_xbitmap_t hXBitmap);
PUBLIC void BitmapWidgetSetBitmap(p_bitmap_widget_t hObject, p_xbitmap_t hXBitmap);
PUBLIC void BitmapWidgetMoveTo(p_bitmap_widget_t hObject, int16_t x, int16_t y);
PUBLIC void BitmapWidgetSetParentWin(p_bitmap_widget_t hObject, p_win_t hWIN);
PUBLIC void BitmapWidgetSetColor(p_bitmap_widget_t hObject, uintColor color);
PUBLIC void BitmapWidgetPaint(void *hObject);
int8_t BitmapWidgetCallBack(void *hObject, p_msg_t hMsg);
