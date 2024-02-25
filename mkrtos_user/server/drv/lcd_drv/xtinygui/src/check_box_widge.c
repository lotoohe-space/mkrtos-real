/**
 * @file check_box_widge.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "check_box_widge.h"
#include "paint.h"
#include "gui.h"
#include "paint_cut.h"
#include "x_malloc.h"
#include "xwindows.h"
#include "bitmap.h"

const uint8_t checkBoxDefuImg0[] = {
	________, ________, // 0
	________, ______XX, // 1
	________, ______XX, // 2
	________, _____XX_, // 3
	________, _____XX_, // 4
	________, ____XX__, // 5
	________, ____XX__, // 6
	________, ___XX___, // 7
	_XX_____, ___XX___, // 8
	__XX____, __XX____, // 9
	___XX___, __XX____, // 10
	____XX__, _XX_____, // 11
	_____XX_, _XX_____, // 12
	______XX, XX______, // 13
	_______X, X_______, // 14
	_______X, X_______, // 15

};

/*创建函数*/
PUBLIC p_checkbox_widget_t CheckBoxWidgetCreate(int16_t x, int16_t y, int16_t w, int16_t h, const char *text)
{
	p_checkbox_widget_t hWidge = (p_checkbox_widget_t)(XMalloc(sizeof(checkbox_widget_t)));
	if (hWidge == NULL)
	{
		return NULL;
	}
	WidgetInit((p_widget_base_t)hWidge, x, y, w, h);

	/*设置三个回调函数*/
	_PToHTextWidgeType(hWidge)->baseWidge.paintFun = (void *)CheckBoxWidgetPaint;
	_PToHTextWidgeType(hWidge)->baseWidge.moveToFun = (void *)CheckBoxWidgetMoveTo;
	_PToHTextWidgeType(hWidge)->baseWidge.widgeCallBackFun = (void *)CheckBoxWidgetCallBack;
	// hWidge->widgeBase.widgeCloseFun = CheckBoxWidgetClose);

	_PToHTextWidgeType(hWidge)->text = text;
	_PToHTextWidgeType(hWidge)->hFont = (p_font_t)&fontASCII16_16;
	hWidge->flag = 0;

	/*初始化图片*/
	InitBitmap(&(hWidge->checkedImg), (uint8_t *)checkBoxDefuImg0, 16, 16, 1);

	return hWidge;
}
/*设置点击回调*/
void CheckBoxWidgetSetClickBack(p_checkbox_widget_t hObject, void *arg, ViewClickCallBack viewClickCallBack)
{
	WidgetSetClickBack((p_widget_base_t)hObject, arg, viewClickCallBack);
	/*if (!hObject) { return; }
	hObject->viewClickCallBack = viewClickCallBack;*/
}
/*设置状态，内部调用的函数*/
PRIVATE void CheckBoxWidgetSetStatus(p_checkbox_widget_t hObject, uint8_t status)
{
	if (!hObject)
	{
		return;
	}
	hObject->flag = status & 0x3;
	WindowsInvaildRect(WIDGE_PARENT(hObject), (p_xrect_t)hObject);
}
/*过去状态*/
PUBLIC uint8_t CheckBoxWidgetGetStatus(p_checkbox_widget_t hObject)
{
	if (!hObject)
	{
		return 0;
	}
	return (hObject->flag) & 0x3;
}
/*移动函数*/
PUBLIC void CheckBoxWidgetMoveTo(p_checkbox_widget_t hObject, int16_t x, int16_t y)
{
	if (!hObject)
	{
		return;
	}
	_PToHTextWidgeType(hObject)->baseWidge.rect.x = x;
	_PToHTextWidgeType(hObject)->baseWidge.rect.y = y;
	// WindowsInvaildRect(hObject->widgeBase.parentHWIN, (p_xrect_t)hObject);
}
/*设置父窗口*/
PUBLIC void CheckBoxWidgetSetParentWin(p_checkbox_widget_t hObject, p_widget_base_t parentWidge)
{
	if (!hObject)
	{
		return;
	}
	_PToHTextWidgeType(hObject)->baseWidge.parentHWIN = parentWidge;
}
/*设置颜色*/
PUBLIC void CheckBoxWidgetSetColor(p_checkbox_widget_t hObject, uintColor color)
{
	if (!hObject)
	{
		return;
	}
	_PToHTextWidgeType(hObject)->baseWidge.pencil.DrawColor = color;
}
/*绘制控件*/
PUBLIC void CheckBoxWidgetPaint(void *hObject)
{
	uint16_t drawW;
	xrect_t xRect;
	// float progressing;
	p_checkbox_widget_t hWidge;
	hWidge = hObject;
	if (!hObject)
	{
		return;
	}
	if (!_GetVisable(hWidge))
	{
		return;
	}
	// if (!IsGUINeedCut(hWidge)) { return; }

	if (!DrawSetArea((p_widget_base_t)hWidge))
	{
		return;
	}

	_PToHTextWidgeType(hObject)->baseWidge.pencil.DrawColor = _PToHTextWidgeType(hObject)->baseWidge.pencil.DrawFrColor;

	drawW = WIDGE_H(hWidge);

	/*画边框*/
	xRect.x = WIDGE_X(hWidge);
	xRect.y = WIDGE_Y(hWidge);
	xRect.w = drawW;
	xRect.h = 1;
	DrawCutRect(hWidge, &xRect);

	xRect.x = WIDGE_X(hWidge);
	xRect.y = WIDGE_Y(hWidge) + WIDGE_H(hWidge) - 1;
	xRect.w = drawW;
	xRect.h = 1;
	DrawCutRect(hWidge, &xRect);

	xRect.x = WIDGE_X(hWidge);
	xRect.y = WIDGE_Y(hWidge);
	xRect.w = 1;
	xRect.h = WIDGE_H(hWidge);
	DrawCutRect(hWidge, &xRect);

	xRect.x = WIDGE_X(hWidge) + drawW - 1;
	xRect.y = WIDGE_Y(hWidge);
	xRect.w = 1;
	xRect.h = WIDGE_H(hWidge);
	DrawCutRect(hWidge, &xRect);

	/*画勾*/
	xRect.x = WIDGE_X(hWidge) + 1;
	xRect.y = WIDGE_Y(hWidge) + 1;
	xRect.w = drawW - 2;
	xRect.h = WIDGE_H(hWidge) - 2;
	switch (CheckBoxWidgetGetStatus(hWidge))
	{
	case 0: /*未选择状态*/
	{
		_PToHTextWidgeType(hObject)->baseWidge.pencil.DrawColor = _PToHTextWidgeType(hObject)->baseWidge.pencil.DrawBkColor;
		DrawCutRect(hWidge, &xRect);
		xpoint_t xPoint;
		xPoint.x = 0;
		xPoint.y = 0;
		xrect_t xRect;
		xRect.x = WIDGE_X(hWidge) + drawW;
		xRect.y = WIDGE_Y(hWidge);
		xRect.w = WIDGE_W(hWidge) - drawW;
		xRect.h = WIDGE_H(hWidge);
		// DrawCutChar(hWidge, hWidge->hFONTF, &xRect, &xPoint, 'S');
		DrawCutString(hWidge, _PToHTextWidgeType(hObject)->hFont, &xRect, &xPoint, _PToHTextWidgeType(hObject)->text);
	}
	break;
	case 1: /*点击状态*/
		DrawCutBitmap(hWidge, &xRect, &(hWidge->checkedImg));
		xpoint_t xPoint;
		xPoint.x = 0;
		xPoint.y = 0;
		xrect_t xRect;
		xRect.x = WIDGE_X(hWidge) + drawW;
		xRect.y = WIDGE_Y(hWidge);
		xRect.w = WIDGE_W(hWidge) - drawW;
		xRect.h = WIDGE_H(hWidge);
		// DrawCutChar(hWidge, hWidge->hFONTF, &xRect, &xPoint, 'S');
		DrawCutString(hWidge, _PToHTextWidgeType(hObject)->hFont, &xRect, &xPoint, _PToHTextWidgeType(hObject)->text);
		break;
	case 2:
		break;
	}
	/*恢复绘图区域*/
	DrawResetArea((p_widget_base_t)hWidge);
}
/*事件回调*/
int8_t CheckBoxWidgetCallBack(void *hObject, p_msg_t hMsg)
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

		if (_IsDrawCheckPointR(hMsg->msgVal.rect.x, hMsg->msgVal.rect.y, &(hWidgeBase->rect)))
		{
			switch (hMsg->msgID)
			{
			case MSG_TOUCH_MOVE:
				break;
			case MSG_TOUCH_PRESS:

				break;
			case MSG_TOUCH_RELEASE:
				if (CheckBoxWidgetGetStatus((p_checkbox_widget_t)hWidgeBase) == 1)
				{
					CheckBoxWidgetSetStatus((p_checkbox_widget_t)hWidgeBase, 0);
					if (_PToHWidgeBaseType(hWidgeBase)->viewClickCallBack != NULL)
					{
						_PToHWidgeBaseType(hWidgeBase)->viewClickCallBack(hWidgeBase, hWidgeBase->arg, CheckBoxWidgetGetStatus((p_checkbox_widget_t)hWidgeBase));
					}
				}
				else if (CheckBoxWidgetGetStatus((p_checkbox_widget_t)hWidgeBase) == 0)
				{
					CheckBoxWidgetSetStatus((p_checkbox_widget_t)hWidgeBase, 1);
					if (_PToHWidgeBaseType(hWidgeBase)->viewClickCallBack != NULL)
					{
						_PToHWidgeBaseType(hWidgeBase)->viewClickCallBack(hWidgeBase, hWidgeBase->arg, CheckBoxWidgetGetStatus((p_checkbox_widget_t)hWidgeBase));
					}
				}
				break;
			}
			return 0;
		}
	}
	return -1;
}
