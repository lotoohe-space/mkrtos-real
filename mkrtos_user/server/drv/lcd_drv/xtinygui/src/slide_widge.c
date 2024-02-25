/**
 * @file slide_widge.c
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "slide_widge.h"
#include "paint_cut.h"
#include "x_malloc.h"

#define SLIDE_DEFAULT_MAX_VAL 100

PUBLIC p_slide_widget_t SLIDE_MARK_HEAD(Create)(int16_t x, int16_t y, int16_t w, int16_t h)
{
	p_slide_widget_t hWidgeBase = (p_slide_widget_t)(XMalloc(sizeof(slide_widget_t)));
	if (hWidgeBase == NULL)
	{
		return NULL;
	}
	WidgetInit((p_widget_base_t)hWidgeBase, x, y, w, h);

	/*设置三个回调函数*/
	_PToHWidgeBaseType(hWidgeBase)->paintFun = SLIDE_MARK_HEAD(Paint);
	//_PToHWidgeBaseType(hWidgeBase)->moveToFun = SLIDE_MARK_HEAD(MoveTo);
	_PToHWidgeBaseType(hWidgeBase)->widgeCallBackFun = SLIDE_MARK_HEAD(CallBack);
	//_PToHWidgeBaseType(hWidgeBase)->widgeCloseFun = SLIDE_MARK_HEAD(Close);
	//_PToHWidgeBaseType(hWidgeBase)->widgeResizeFun = SLIDE_MARK_HEAD(Resize);

	hWidgeBase->baseWidge.pencil.DrawColor = RGB565_BLACK;
	hWidgeBase->baseWidge.pencil.DrawFrColor = _DefaultFrColor;
	hWidgeBase->baseWidge.pencil.DrawBkColor = RGB565_GEN(230, 235, 230);

	hWidgeBase->maxVal = SLIDE_DEFAULT_MAX_VAL;
	hWidgeBase->currentVal = 50;

	return hWidgeBase;
}

/*重绘函数*/
PUBLIC void SLIDE_MARK_HEAD(Paint)(void *hObject)
{
	p_slide_widget_t hWidgeBase;
	hWidgeBase = hObject;
	if (!hObject)
	{
		return;
	}
	if (!_GetVisable(hWidgeBase))
	{
		return;
	}
	// if (!IsGUINeedCut(hWidgeBase)) { return; }

	if (!DrawSetArea((p_widget_base_t)hWidgeBase))
	{
		return;
	}

	uint16_t barH;
	uint16_t slideBlockH;
	uint16_t slideBlockW;
	/*得到相对坐标*/
	int16_t x, y;
	uint16_t w, h;

	int16_t drawY;
	int16_t posBCK;

	barH = 6;
	slideBlockH = WIDGE_H(hWidgeBase);
	slideBlockW = 8;

	x = CDE2OPPOSITE_X(hWidgeBase);
	y = CDE2OPPOSITE_Y(hWidgeBase);
	w = WIDGE_W(hWidgeBase);
	h = WIDGE_H(hWidgeBase);

	/*横栏的位置*/
	drawY = (h - barH) >> 1;
	/*块的位置*/
	posBCK = (int16_t)((w - slideBlockW) *
					 ((float)(hWidgeBase->currentVal) / (float)(hWidgeBase->maxVal)));
	/*中间的条*/
	xrect_t drawRect;
	drawRect.x = WIDGE_X(hWidgeBase);
	drawRect.y = WIDGE_Y(hWidgeBase) + drawY;
	drawRect.w = posBCK;
	drawRect.h = barH;
	hWidgeBase->baseWidge.pencil.DrawColor = hWidgeBase->baseWidge.pencil.DrawFrColor;
	DrawCutRect(hWidgeBase, &drawRect);

	drawRect.x = WIDGE_X(hWidgeBase) + posBCK + slideBlockW;
	drawRect.y = WIDGE_Y(hWidgeBase) + drawY;
	drawRect.w = w - posBCK - slideBlockW;
	drawRect.h = barH;
	hWidgeBase->baseWidge.pencil.DrawColor = hWidgeBase->baseWidge.pencil.DrawFrColor;
	DrawCutRect(hWidgeBase, &drawRect);
	/*上面的条*/
	drawRect.x = WIDGE_X(hWidgeBase);
	drawRect.y = WIDGE_Y(hWidgeBase);
	drawRect.w = posBCK;
	drawRect.h = drawY;
	hWidgeBase->baseWidge.pencil.DrawColor = hWidgeBase->baseWidge.pencil.DrawBkColor;
	DrawCutRect(hWidgeBase, &drawRect);

	drawRect.x = WIDGE_X(hWidgeBase) + posBCK + slideBlockW;
	drawRect.y = WIDGE_Y(hWidgeBase);
	drawRect.w = w - posBCK - slideBlockW;
	drawRect.h = drawY;
	hWidgeBase->baseWidge.pencil.DrawColor = hWidgeBase->baseWidge.pencil.DrawBkColor;
	DrawCutRect(hWidgeBase, &drawRect);

	/*下面的条*/
	drawRect.x = WIDGE_X(hWidgeBase);
	drawRect.y = WIDGE_Y(hWidgeBase) + drawY + barH;
	drawRect.w = posBCK;
	drawRect.h = drawY;
	hWidgeBase->baseWidge.pencil.DrawColor = hWidgeBase->baseWidge.pencil.DrawBkColor;
	DrawCutRect(hWidgeBase, &drawRect);

	drawRect.x = WIDGE_X(hWidgeBase) + posBCK + slideBlockW;
	drawRect.y = WIDGE_Y(hWidgeBase) + drawY + barH;
	drawRect.w = w - posBCK - slideBlockW;
	drawRect.h = drawY;
	hWidgeBase->baseWidge.pencil.DrawColor = hWidgeBase->baseWidge.pencil.DrawBkColor;
	DrawCutRect(hWidgeBase, &drawRect);

	/*中间的条*/
	drawRect.x = WIDGE_X(hWidgeBase) + posBCK;
	drawRect.y = WIDGE_Y(hWidgeBase);
	drawRect.w = slideBlockW;
	drawRect.h = WIDGE_H(hWidgeBase);
	hWidgeBase->baseWidge.pencil.DrawColor = hWidgeBase->baseWidge.pencil.DrawFrColor;
	DrawCutRect(hWidgeBase, &drawRect);

	/*恢复绘图区域*/
	DrawResetArea((p_widget_base_t)hWidgeBase);
}

/*事件回调*/
PUBLIC int8_t SLIDE_MARK_HEAD(CallBack)(void *hObject, p_msg_t hMsg)
{
	int8_t ret;
	p_slide_widget_t hBaseWidge = hObject;
	if (!hBaseWidge || !hMsg)
	{
		return -1;
	}
	if (!_GetVisable(hBaseWidge))
	{
		return -1;
	}

	ret = WidgetCallBack(hBaseWidge, hMsg);

	if (ret == RES_OK)
	{
		if (_IsDrawCheckPointR(hMsg->msgVal.rect.x, hMsg->msgVal.rect.y, (p_xrect_t)hBaseWidge))
		{
			switch (hMsg->msgID)
			{
			case MSG_TOUCH_PRESS:

				break;
			case MSG_TOUCH_MOVE:
			{
				int16_t Dx;

				Dx = P_CDE2OPPOSITE_X(hBaseWidge, hMsg->msgVal.rect.x);
				// Dy = P_CDE2OPPOSITE_Y(hBaseWidge, hMsg->msgVal.rect.y);
				hBaseWidge->currentVal = (uint16_t)(((float)Dx / (float)WIDGE_W(hBaseWidge)) *
												  (hBaseWidge->maxVal));
				WindowsInvaildRect(hObject, NULL);
			}
			break;
			case MSG_TOUCH_RELEASE:

				break;
			}
			return RES_OK;
		}
	}

	return ret;
}
