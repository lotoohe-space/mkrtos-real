/**
 * @file bitmap_widge.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "paint.h"
#include "bitmap_widge.h"
#include "gui.h"
#include "paint_cut.h"
#include "x_malloc.h"
#include "xwindows.h"

PUBLIC p_bitmap_widget_t BitmapWidgetCreate(int16_t x, int16_t y, int16_t w, int16_t h, p_xbitmap_t hXBitmap)
{
	p_bitmap_widget_t hBitmapWidget = (p_bitmap_widget_t)(XMalloc(sizeof(bitmap_widget_t)));
	if (hBitmapWidget == NULL)
	{
		return NULL;
	}
	WidgetInit((p_widget_base_t)hBitmapWidget, x, y, w, h);

	/*设置三个回调函数*/
	hBitmapWidget->widgeBase.paintFun = (void *)BitmapWidgetPaint;
	hBitmapWidget->widgeBase.moveToFun = (void *)BitmapWidgetMoveTo;
	hBitmapWidget->widgeBase.widgeCallBackFun = (void *)BitmapWidgetCallBack;

	/*设置显示的图片*/
	hBitmapWidget->bitmap = hXBitmap;

	return hBitmapWidget;
}
PUBLIC void BitmapWidgetSetBitmap(p_bitmap_widget_t hObject, p_xbitmap_t hXBitmap)
{
	if (!hObject)
	{
		return;
	}
	/*设置显示的图片*/
	hObject->bitmap = hXBitmap;
	WindowsInvaildRect((p_widget_base_t)hObject, NULL);
}
PUBLIC void BitmapWidgetMoveTo(p_bitmap_widget_t hObject, int16_t x, int16_t y)
{
	WidgetMoveTo((p_widget_base_t)hObject, x, y);
	// if (!hObject) { return; }
	// hObject->widgeBase.rect.x = x;
	// hObject->widgeBase.rect.y = y;
}

PUBLIC void BitmapWidgetSetParentWin(p_bitmap_widget_t hObject, p_win_t hWIN)
{
	if (!hObject)
	{
		return;
	}
	hObject->widgeBase.parentHWIN = hWIN;
}
PUBLIC void BitmapWidgetSetColor(p_bitmap_widget_t hObject, uintColor color)
{
	if (!hObject)
	{
		return;
	}
	hObject->widgeBase.pencil.DrawColor = color;
}
PUBLIC void BitmapWidgetPaint(void *hObject)
{
	p_bitmap_widget_t hBitmapWidget;
	hBitmapWidget = hObject;
	if (!hObject)
	{
		return;
	}
	if (!_GetVisable(hBitmapWidget))
	{
		return;
	}
	// if (!IsGUINeedCut(hBitmapWidget)) { return; }

	if (!DrawSetArea((p_widget_base_t)hBitmapWidget))
	{
		return;
	}

	DrawCutBitmap(hBitmapWidget, (p_xrect_t)hBitmapWidget, hBitmapWidget->bitmap);
	/*恢复绘图区域*/
	DrawResetArea((p_widget_base_t)hBitmapWidget);
}

int8_t BitmapWidgetCallBack(void *hObject, p_msg_t hMsg)
{
	p_widget_base_t hWidgeBase = hObject;
	if (!hWidgeBase || !hMsg)
	{
		return -1;
	}
	if (!_GetVisable(hWidgeBase))
	{
		return -1;
	}
	if (hMsg->msgType == MSG_TOUCH)
	{
		if (_IsDrawCheckPoint(hMsg->msgVal.rect.x, hMsg->msgVal.rect.y, hWidgeBase->pencil.x, hWidgeBase->pencil.y, hWidgeBase->pencil.w, hWidgeBase->pencil.h))
		{
			return 1;
		}
	}
	return -1;
}
