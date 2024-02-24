/**
 * @file progress_widge.c
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "progress_widge.h"
#include "paint.h"
#include "gui.h"
#include "paint_cut.h"
#include "x_malloc.h"
#include "xwindows.h"

/*创建函数*/
PUBLIC p_progress_widget_t PROGRESS_MARK_HEAD(Create)(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t maxVal)
{
	p_progress_widget_t hWidge = (p_progress_widget_t)(XMalloc(sizeof(progress_widget_t)));
	if (hWidge == NULL)
	{
		return NULL;
	}
	WidgetInit((p_widget_base_t)hWidge, x, y, w, h);

	/*设置三个回调函数*/
	hWidge->widgeBase.paintFun = PROGRESS_MARK_HEAD(Paint);
	hWidge->widgeBase.moveToFun = PROGRESS_MARK_HEAD(MoveTo);
	hWidge->widgeBase.widgeCallBackFun = PROGRESS_MARK_HEAD(CallBack);

	hWidge->maxVal = maxVal;
	hWidge->currentVal = maxVal / 2;

	return hWidge;
}

/*移动函数*/
PUBLIC void PROGRESS_MARK_HEAD(MoveTo)(p_progress_widget_t hObject, int16_t x, int16_t y)
{
	WidgetMoveTo((p_widget_base_t)hObject, x, y);
	// if (!hObject) { return; }
	// hObject->widgeBase.rect.x = x;
	// hObject->widgeBase.rect.y = y;
}
/*设置父窗口*/
PUBLIC void PROGRESS_MARK_HEAD(SetParentWin)(p_progress_widget_t hObject, p_win_t hWIN)
{
	WidgetSetParentWin((p_widget_base_t)hObject, hWIN);
	// if (!hObject) { return; }
	// hObject->widgeBase.parentHWIN = hWIN;
}
/*设置颜色*/
PUBLIC void PROGRESS_MARK_HEAD(SetColor)(p_progress_widget_t hObject, uintColor color)
{
	WidgetSetColor((p_widget_base_t)hObject, color);
	// if (!hObject) { return; }
	// hObject->widgeBase.pencil.DrawColor = color;
}

PUBLIC void PROGRESS_MARK_HEAD(SetProgressVal)(p_progress_widget_t hObject, uint16_t val)
{
	if (!hObject)
	{
		return;
	}
	hObject->currentVal = MIN(val, hObject->maxVal);
	WindowsInvaildRect((p_widget_base_t)hObject, NULL);
}

/*绘制控件*/
PRIVATE void PROGRESS_MARK_HEAD(Paint)(void *hObject)
{
	uintColor color;
	xrect_t xRect;
	float progressing;
	p_progress_widget_t hWidge;
	hWidge = hObject;
	if (!hObject)
	{
		return;
	}
	if (!_GetVisable(hWidge))
	{
		return;
	}
	// if (!isGUINeedCut(hWidge)) { return; }

	if (!DrawSetArea((p_widget_base_t)hWidge))
	{
		return;
	}

	progressing = (hWidge->currentVal / (float)(hWidge->maxVal));
	/*绘制前半截*/
	xRect.x = hWidge->widgeBase.rect.x;
	xRect.y = hWidge->widgeBase.rect.y;
	xRect.w = (uint16_t)(hWidge->widgeBase.rect.w * progressing);
	xRect.h = hWidge->widgeBase.rect.h;
	DrawCutRect(hWidge, &xRect);
	/*绘制后半截*/
	color = hWidge->widgeBase.pencil.DrawColor;
	hWidge->widgeBase.pencil.DrawColor = hWidge->widgeBase.pencil.DrawBkColor;

	xRect.x = hWidge->widgeBase.rect.x + (uint16_t)(hWidge->widgeBase.rect.w * progressing);
	xRect.w = hWidge->widgeBase.rect.w - (uint16_t)(hWidge->widgeBase.rect.w * progressing);
	DrawCutRect(hWidge, &xRect);
	hWidge->widgeBase.pencil.DrawColor = color;
	/*恢复绘图区域*/
	DrawResetArea((p_widget_base_t)hWidge);
}
/*事件回调*/
PRIVATE int8_t PROGRESS_MARK_HEAD(CallBack)(void *hObject, p_msg_t hMsg)
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
			return 0;
		}
	}
	return -1;
}
