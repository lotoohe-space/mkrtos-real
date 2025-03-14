/**
 * @file switch_button_widge.c
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "switch_button_widge.h"
#include "paint.h"
#include "gui.h"
#include "paint_cut.h"
#include "x_malloc.h"
#include "xwindows.h"
#include "bitmap.h"

/*创建函数*/
PUBLIC p_switch_button_widget_t SWITCH_BUTTON_WIDGE_MARK_HEAD(Create)(int16_t x, int16_t y, int16_t w, int16_t h,
																  const char *onText, const char *offText)
{
	p_switch_button_widget_t hWidge = (p_switch_button_widget_t)(XMalloc(sizeof(switch_button_widget_t)));
	if (hWidge == NULL)
	{
		return NULL;
	}
	TEXT_MARK_HEAD(Init)
	((p_text_widget_t)hWidge, "", x, y, w, h);

	/*设置三个回调函数*/
	_PToHTextWidgeType(hWidge)->baseWidge.paintFun = SWITCH_BUTTON_WIDGE_MARK_HEAD(Paint);
	_PToHTextWidgeType(hWidge)->baseWidge.moveToFun = SWITCH_BUTTON_WIDGE_MARK_HEAD(MoveTo);
	_PToHTextWidgeType(hWidge)->baseWidge.widgeCallBackFun = SWITCH_BUTTON_WIDGE_MARK_HEAD(CallBack);
	// hWidge->widgeBase.widgeCloseFun = SWITCH_BUTTON_WIDGE_MARK_HEAD(Close);

	_PToHTextWidgeType(hWidge)->text = "";
	_PToHTextWidgeType(hWidge)->hFont = (p_font_t)(&fontASCII16_16);
	hWidge->flag = 0;
	hWidge->onText = onText ? onText : "ON";
	hWidge->offText = offText ? offText : "OFF";
	hWidge->onBitmap = NULL;
	hWidge->offBitmap = NULL;

	return hWidge;
}
PUBLIC void SWITCH_BUTTON_WIDGE_MARK_HEAD(SetOnText)(p_switch_button_widget_t hObject, const char *text)
{
	if (!hObject)
	{
		return;
	}
	hObject->onText = text;
	WindowsInvaildRect(WIDGE_PARENT(hObject), (p_xrect_t)hObject);
}
PUBLIC void SWITCH_BUTTON_WIDGE_MARK_HEAD(SetOffText)(p_switch_button_widget_t hObject, const char *text)
{
	if (!hObject)
	{
		return;
	}
	hObject->offText = text;
	WindowsInvaildRect(WIDGE_PARENT(hObject), (p_xrect_t)hObject);
}
PUBLIC void SWITCH_BUTTON_WIDGE_MARK_HEAD(SetOnBitmap)(p_switch_button_widget_t hObject, p_xbitmap_t hBitmap)
{
	if (!hObject)
	{
		return;
	}
	hObject->onBitmap = hBitmap;
	WindowsInvaildRect(WIDGE_PARENT(hObject), (p_xrect_t)hObject);
}
PUBLIC void SWITCH_BUTTON_WIDGE_MARK_HEAD(SetOffBitmap)(p_switch_button_widget_t hObject, p_xbitmap_t hBitmap)
{
	if (!hObject)
	{
		return;
	}
	hObject->offBitmap = hBitmap;
	WindowsInvaildRect(WIDGE_PARENT(hObject), (p_xrect_t)hObject);
}
/*设置点击回调*/
PUBLIC void SWITCH_BUTTON_WIDGE_MARK_HEAD(SetClickBack)(p_switch_button_widget_t hObject, void *arg, ViewClickCallBack viewClickCallBack)
{
	WidgetSetClickBack((p_widget_base_t)hObject, arg, viewClickCallBack);
}
/*设置状态，内部调用的函数*/
PRIVATE void SWITCH_BUTTON_WIDGE_MARK_HEAD(SetStatus)(p_switch_button_widget_t hObject, uint8_t status)
{
	if (!hObject)
	{
		return;
	}
	hObject->flag &= ~0x3;
	hObject->flag |= status & 0x3;
	WindowsInvaildRect(WIDGE_PARENT(hObject), (p_xrect_t)hObject);
}
/*过去状态*/
PUBLIC uint8_t SWITCH_BUTTON_WIDGE_MARK_HEAD(GetStatus)(p_switch_button_widget_t hObject)
{
	if (!hObject)
	{
		return 0;
	}
	return (hObject->flag) & 0x3;
}
/*移动函数*/
PUBLIC void SWITCH_BUTTON_WIDGE_MARK_HEAD(MoveTo)(p_switch_button_widget_t hObject, int16_t x, int16_t y)
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
PUBLIC void SWITCH_BUTTON_WIDGE_MARK_HEAD(SetParentWin)(p_switch_button_widget_t hObject, p_widget_base_t parentWidge)
{
	if (!hObject)
	{
		return;
	}
	_PToHTextWidgeType(hObject)->baseWidge.parentHWIN = parentWidge;
}
/*设置颜色*/
PUBLIC void SWITCH_BUTTON_WIDGE_MARK_HEAD(SetColor)(p_switch_button_widget_t hObject, uintColor color)
{
	if (!hObject)
	{
		return;
	}
	_PToHTextWidgeType(hObject)->baseWidge.pencil.DrawColor = color;
}
/*绘制控件*/
PUBLIC void SWITCH_BUTTON_WIDGE_MARK_HEAD(Paint)(void *hObject)
{
	xpoint_t startPoint;
	p_switch_button_widget_t hWidge;
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

	switch (SWITCH_BUTTON_WIDGE_MARK_HEAD(GetStatus)(hWidge))
	{
	case 0: /*未选择状态*/
	{
		if (!_GET_SW_MODE(hWidge))
		{ /*文本模式*/
			_PToHTextWidgeType(hWidge)->text = hWidge->offText;
			TEXT_MARK_HEAD(GetPOIByTextMode)
			((p_text_widget_t)hWidge, &startPoint);
			/*根据模式确定起点*/
			DrawCutString(hWidge,
						  _PToHTextWidgeType(hWidge)->hFont,
						  _PToHXRECTType(hWidge),
						  &startPoint,
						  (uint8_t *)(_PToHTextWidgeType(hWidge)->text));
		}
		else
		{ /*图片模式*/
			DrawCutBitmap(hWidge, _PToHXRECTType(hWidge), hWidge->offBitmap);
		}
	}
	break;
	case 1: /*点击状态*/
		if (!_GET_SW_MODE(hWidge))
		{ /*文本模式*/
			_PToHTextWidgeType(hWidge)->text = hWidge->onText;
			TEXT_MARK_HEAD(GetPOIByTextMode)
			((p_text_widget_t)hWidge, &startPoint);
			/*根据模式确定起点*/
			DrawCutString(hWidge,
						  _PToHTextWidgeType(hWidge)->hFont,
						  _PToHXRECTType(hWidge),
						  &startPoint,
						  (uint8_t *)(_PToHTextWidgeType(hWidge)->text));
		}
		else
		{ /*图片模式*/
			DrawCutBitmap(hWidge, _PToHXRECTType(hWidge), hWidge->onBitmap);
		}
		break;
	case 2:
		break;
	}
	/*恢复绘图区域*/
	DrawResetArea((p_widget_base_t)hWidge);
}
/*事件回调*/
int8_t SWITCH_BUTTON_WIDGE_MARK_HEAD(CallBack)(void *hObject, p_msg_t hMsg)
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
				if (SWITCH_BUTTON_WIDGE_MARK_HEAD(GetStatus)((p_switch_button_widget_t)hWidgeBase) == 1)
				{
					SWITCH_BUTTON_WIDGE_MARK_HEAD(SetStatus)
					((p_switch_button_widget_t)hWidgeBase, 0);
					if (_PToHWidgeBaseType(hWidgeBase)->viewClickCallBack != NULL)
					{
						_PToHWidgeBaseType(hWidgeBase)->viewClickCallBack(hWidgeBase, hWidgeBase->arg, SWITCH_BUTTON_WIDGE_MARK_HEAD(GetStatus)((p_switch_button_widget_t)hWidgeBase));
					}
				}
				else if (SWITCH_BUTTON_WIDGE_MARK_HEAD(GetStatus)((p_switch_button_widget_t)hWidgeBase) == 0)
				{
					SWITCH_BUTTON_WIDGE_MARK_HEAD(SetStatus)
					((p_switch_button_widget_t)hWidgeBase, 1);
					if (_PToHWidgeBaseType(hWidgeBase)->viewClickCallBack != NULL)
					{
						_PToHWidgeBaseType(hWidgeBase)->viewClickCallBack(hWidgeBase, hWidgeBase->arg, SWITCH_BUTTON_WIDGE_MARK_HEAD(GetStatus)((p_switch_button_widget_t)hWidgeBase));
					}
				}
				break;
			}
			return 0;
		}
	}
	return -1;
}
