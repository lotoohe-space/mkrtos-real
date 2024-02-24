/**
 * @file radio_button_widge.c
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "radio_button_widge.h"
#include "paint.h"
#include "gui.h"
#include "paint_cut.h"
#include "x_malloc.h"
#include "xwindows.h"
#include "bitmap.h"

static const uint8_t radioButtonImg0[] = {
	_______X, X_______, // 0
	_____XXX, XXX_____, // 1
	___XXXXX, XXXXX___, // 2
	__XXXXXX, XXXXXX__, // 3
	_XXXXXXX, XXXXXXX_, // 4
	_XXXXXXX, XXXXXXX_, // 5
	XXXXXXXX, XXXXXXXX, // 6
	XXXXXXXX, XXXXXXXX, // 7
	XXXXXXXX, XXXXXXXX, // 8
	XXXXXXXX, XXXXXXXX, // 9
	_XXXXXXX, XXXXXXX_, // 10
	_XXXXXXX, XXXXXXX_, // 11
	__XXXXXX, XXXXXX__, // 12
	___XXXXX, XXXXX___, // 13
	_____XXX, XXX_____, // 14
	_______X, X_______, // 15
};
static const uint8_t radioButtonImg1[] = {
	________, ________, // 0
	________, ________, // 1
	______XX, XX______, // 2
	_____XXX, XXX_____, // 3
	____XXXX, XXXX____, // 4
	___XXXXX, XXXXX___, // 5
	___XXXXX, XXXXX___, // 6
	__XXXXXX, XXXXXX__, // 7
	__XXXXXX, XXXXXX__, // 8
	___XXXXX, XXXXX___, // 9
	___XXXXX, XXXXX___, // 10
	____XXXX, XXXX____, // 11
	_____XXX, XXX_____, // 12
	______XX, XX______, // 13
	________, ________, // 14
	________, ________, // 15
};
/*创建函数*/
PUBLIC p_radio_button_widget_t RADIO_BUTTON_MARK_HEAD(Create)(int16_t x, int16_t y, int16_t w, int16_t h, const char *text)
{
	p_radio_button_widget_t hWidge = (p_radio_button_widget_t)(XMalloc(sizeof(radio_button_widget_t)));
	if (hWidge == NULL)
	{
		return NULL;
	}
	WidgetInit((p_widget_base_t)hWidge, x, y, w, h);

	/*设置三个回调函数*/
	_PToHTextWidgeType(hWidge)->baseWidge.paintFun = RADIO_BUTTON_MARK_HEAD(Paint);
	_PToHTextWidgeType(hWidge)->baseWidge.moveToFun = RADIO_BUTTON_MARK_HEAD(MoveTo);
	_PToHTextWidgeType(hWidge)->baseWidge.widgeCallBackFun = RADIO_BUTTON_MARK_HEAD(CallBack);
	// hWidge->widgeBase.widgeCloseFun = RADIO_BUTTON_MARK_HEAD(Close);

	_PToHTextWidgeType(hWidge)->text = text;
	_PToHTextWidgeType(hWidge)->hFont = (p_font_t)&fontASCII16_16;
	hWidge->flag = 0;

	_SET_GROUP_CTRL(hWidge);
	/*初始化图片*/
	InitBitmap(&(hWidge->checkedImg), (uint8_t *)radioButtonImg0, 16, 16, 1);

	return hWidge;
}
/*设置点击回调*/
void RADIO_BUTTON_MARK_HEAD(SetClickBack)(p_radio_button_widget_t hObject, void *arg, ViewClickCallBack viewClickCallBack)
{
	WidgetSetClickBack((p_widget_base_t)hObject, arg, viewClickCallBack);
	/*if (!hObject) { return; }
	hObject->viewClickCallBack = viewClickCallBack;*/
}
/*设置状态，内部调用的函数*/
PUBLIC void RADIO_BUTTON_MARK_HEAD(SetStatus)(p_radio_button_widget_t hObject, uint8_t status)
{
	if (!hObject)
	{
		return;
	}
	hObject->flag = status & 0x3;
	if (status == TRUE)
	{
		GROUP_MARK_HEAD(GroupProcess)
		((p_group_widget_t)(WIDGE_PARENT(hObject)), (p_widget_base_t)hObject);
	}
	WindowsInvaildRect(WIDGE_PARENT(hObject), (p_xrect_t)hObject);
}
/*过去状态*/
PUBLIC uint8_t RADIO_BUTTON_MARK_HEAD(GetStatus)(p_radio_button_widget_t hObject)
{
	if (!hObject)
	{
		return 0;
	}
	return (hObject->flag) & 0x3;
}
/*移动函数*/
PUBLIC void RADIO_BUTTON_MARK_HEAD(MoveTo)(p_radio_button_widget_t hObject, int16_t x, int16_t y)
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
PUBLIC void RADIO_BUTTON_MARK_HEAD(SetParentWin)(p_radio_button_widget_t hObject, p_widget_base_t parentWidge)
{
	if (!hObject)
	{
		return;
	}
	_PToHTextWidgeType(hObject)->baseWidge.parentHWIN = parentWidge;
}
/*设置颜色*/
PUBLIC void RADIO_BUTTON_MARK_HEAD(SetColor)(p_radio_button_widget_t hObject, uintColor color)
{
	if (!hObject)
	{
		return;
	}
	_PToHTextWidgeType(hObject)->baseWidge.pencil.DrawColor = color;
}
/*绘制控件*/
PUBLIC void RADIO_BUTTON_MARK_HEAD(Paint)(void *hObject)
{
	uint16_t drawW;
	xrect_t xRect;
	// float progressing;
	p_radio_button_widget_t hWidge;
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

	drawW = 16;

	/*画勾*/
	xRect.x = WIDGE_X(hWidge);
	xRect.y = WIDGE_Y(hWidge);
	xRect.w = drawW;
	xRect.h = WIDGE_H(hWidge);
	switch (RADIO_BUTTON_MARK_HEAD(GetStatus)(hWidge))
	{
	case 0: /*未选择状态*/
	{
		hWidge->checkedImg.pixels = (uint8_t *)radioButtonImg0;
		_PToHTextWidgeType(hObject)->baseWidge.pencil.DrawFrColor = 0xFFFF;
		DrawCutBitmap(hWidge, &xRect, &(hWidge->checkedImg));

		xpoint_t xPoint;
		xPoint.x = 0;
		xPoint.y = 0;
		xrect_t xRect;
		xRect.x = WIDGE_X(hWidge) + drawW;
		xRect.y = WIDGE_Y(hWidge);
		xRect.w = WIDGE_W(hWidge) - drawW;
		xRect.h = WIDGE_H(hWidge);
		_PToHTextWidgeType(hObject)->baseWidge.pencil.DrawFrColor = 0x0000;
		DrawCutString(hWidge, _PToHTextWidgeType(hObject)->hFont, &xRect, &xPoint, _PToHTextWidgeType(hObject)->text);
	}
	break;
	case 1: /*点击状态*/
		hWidge->checkedImg.pixels = (uint8_t *)radioButtonImg1;
		_PToHTextWidgeType(hObject)->baseWidge.pencil.DrawFrColor = 0x0000;
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
		DrawCutString(hWidge, _PToHTextWidgeType(hObject)->hFont, &xRect, &xPoint,
					  (uint8_t *)(_PToHTextWidgeType(hObject)->text));
		break;
	case 2:
		break;
	}
	/*恢复绘图区域*/
	DrawResetArea((p_widget_base_t)hWidge);
}
/*事件回调*/
int8_t RADIO_BUTTON_MARK_HEAD(CallBack)(void *hObject, p_msg_t hMsg)
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
				if (RADIO_BUTTON_MARK_HEAD(GetStatus)((p_radio_button_widget_t)hWidgeBase) == 1)
				{
					RADIO_BUTTON_MARK_HEAD(SetStatus)
					((p_radio_button_widget_t)hWidgeBase, 0);
					if (_PToHWidgeBaseType(hWidgeBase)->viewClickCallBack != NULL)
					{
						_PToHWidgeBaseType(hWidgeBase)->viewClickCallBack(hWidgeBase, hWidgeBase->arg, RADIO_BUTTON_MARK_HEAD(GetStatus)((p_radio_button_widget_t)hWidgeBase));
					}
				}
				else if (RADIO_BUTTON_MARK_HEAD(GetStatus)((p_radio_button_widget_t)hWidgeBase) == 0)
				{
					RADIO_BUTTON_MARK_HEAD(SetStatus)
					((p_radio_button_widget_t)hWidgeBase, 1);
					if (_PToHWidgeBaseType(hWidgeBase)->viewClickCallBack != NULL)
					{
						_PToHWidgeBaseType(hWidgeBase)->viewClickCallBack(hWidgeBase, hWidgeBase->arg, RADIO_BUTTON_MARK_HEAD(GetStatus)((p_radio_button_widget_t)hWidgeBase));
					}
				}
				break;
			}
			return 0;
		}
	}
	return -1;
}
