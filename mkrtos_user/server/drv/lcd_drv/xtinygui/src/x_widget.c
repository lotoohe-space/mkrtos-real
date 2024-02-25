/**
 * @file x_widget.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "paint.h"
#include "x_widget.h"
#include "x_malloc.h"
#include "gui.h"
#include "paint_cut.h"
#include <assert.h>
/**
 * @brief ����һ�����
 *
 * @param x
 * @param y
 * @param w
 * @param h
 * @return PUBLIC
 */
PUBLIC p_widget_base_t WidgetCreate(int16_t x, int16_t y, int16_t w, int16_t h)
{
	p_widget_base_t hWidgeBase = (p_widget_base_t)(XMalloc(sizeof(widget_base_t)));

	if (hWidgeBase == NULL)
	{
		return NULL;
	}
	WidgetInit(hWidgeBase, x, y, w, h);
	return hWidgeBase;
}
/**
 * @brief ����һ�����
 *
 * @param hXRect
 * @return PUBLIC
 */
PUBLIC p_widget_base_t WidgetCreateEx(p_xrect_t hXRect)
{
	assert(hXRect);
	return WidgetCreate(hXRect->x, hXRect->y, hXRect->w, hXRect->h);
}
/**
 * @brief ��ʼ��һ��С���
 *
 * @param hWidgeBase
 * @param x
 * @param y
 * @param w
 * @param h
 * @return PUBLIC
 */
PUBLIC void WidgetInit(p_widget_base_t hWidgeBase, int16_t x, int16_t y, int16_t w, int16_t h)
{
	assert(hWidgeBase);

	ListInit(&hWidgeBase->widget_node);

	hWidgeBase->rect.x = x;
	hWidgeBase->rect.y = y;
	hWidgeBase->rect.w = w;
	hWidgeBase->rect.h = h;

	hWidgeBase->paintFun = WidgetPaint;
	hWidgeBase->moveToFun = WidgetMoveTo;
	hWidgeBase->widgeCallBackFun = WidgetCallBack;
	hWidgeBase->widgeCloseFun = WidgetClose;
	hWidgeBase->widgeResizeFun = WidgetResize;

	hWidgeBase->pencil.DrawColor = RGB565_BLACK;
	hWidgeBase->pencil.DrawFrColor = _DefaultFrColor;
	hWidgeBase->pencil.DrawBkColor = _DefaultBKColor;

	hWidgeBase->pencil.x = x;
	hWidgeBase->pencil.y = y;
	hWidgeBase->pencil.w = w;
	hWidgeBase->pencil.h = h;

	hWidgeBase->parentHWIN = NULL;

	hWidgeBase->viewClickCallBack = NULL;
	hWidgeBase->arg = NULL;

	hWidgeBase->flag = 0;

	_SET_IS_WIN(hWidgeBase);
	_SetVisable(hWidgeBase);
	_CLR_IS_DPY(hWidgeBase);
}
/**
 * @brief �ر�һ��С���
 *
 * @param hObject
 * @return PUBLIC
 */
PUBLIC void WidgetClose(p_widget_base_t hObject)
{
	assert(hObject);
	XFree(hObject);
}
/**
 * @brief ���ûص�����
 *
 * @param hObject
 * @param arg
 * @param viewClickCallBack
 * @return PUBLIC
 */
PUBLIC void WidgetSetClickBack(p_widget_base_t hObject, void *arg, ViewClickCallBack viewClickCallBack)
{
	assert(hObject);
	hObject->arg = arg;
	hObject->viewClickCallBack = viewClickCallBack;
}
/**
 * @brief �����Ƿ�ɼ�
 *
 * @param hObject
 * @param isVisable
 * @return PUBLIC
 */
PUBLIC void WidgetSetVisable(p_widget_base_t hObject, uint8_t isVisable)
{
	assert(hObject);
	if (isVisable)
	{
		_SetVisable(hObject);
	}
	else
	{
		_ClrVisable(hObject);
	}
	WindowsInvaildRect(hObject->parentHWIN, (p_xrect_t)hObject);
}
/**
 * @brief �������ô�С
 *
 * @param hObject
 * @param x
 * @param y
 * @param w
 * @param h
 * @return PUBLIC
 */
PUBLIC void WidgetResize(p_widget_base_t hObject, int16_t x, int16_t y, uint16_t w, uint16_t h)
{
	assert(hObject);
	hObject->rect.x = x;
	hObject->rect.y = y;
	hObject->rect.w = w;
	hObject->rect.h = h;
	// WindowsInvaildRect(((p_widget_base_t)hObject)->parentHWIN, NULL);
}
/**
 * @brief ���ø��ؼ�
 *
 * @param hObject
 * @param hWIN
 * @return PUBLIC
 */
PUBLIC void WidgetSetParentWin(p_widget_base_t hObject, void *hWIN)
{
	assert(hObject);
	hObject->parentHWIN = hWIN;
}
/**
 * @brief ����ǰ��ɫ
 *
 * @param hObject
 * @param color
 * @return PUBLIC
 */
PUBLIC void WidgetSetColor(p_widget_base_t hObject, uintColor color)
{
	assert(hObject);
	hObject->pencil.DrawColor = color;
	WindowsInvaildRect(hObject->parentHWIN, (p_xrect_t)hObject);
}
/**
 * @brief �ƶ��ؼ�
 *
 * @param hObject
 * @param x
 * @param y
 * @return PUBLIC
 */
PUBLIC void WidgetMoveTo(p_widget_base_t hObject, int16_t x, int16_t y)
{
	assert(hObject);
	hObject->rect.x = x;
	hObject->rect.y = y;
	// WindowsInvaildRect(hObject->parentHWIN, (p_xrect_t)hObject);
}
/**
 * @brief �ػ溯��
 *
 * @param hObject
 * @return PRIVATE
 */
PRIVATE void WidgetPaint(void *hObject)
{
	assert(hObject);

	p_xrect_t nextCutRect = NULL;
	p_widget_base_t hWidgeBase;

	hWidgeBase = hObject;

	if (!_GetVisable(hWidgeBase))
	{
		return;
	}
	// if (!IsGUINeedCutEx(hWidgeBase)) { return; }

	if (!DrawSetArea(hWidgeBase))
	{
		return;
	}
	hWidgeBase->pencil.DrawColor = hWidgeBase->pencil.DrawBkColor;
	// ���Ƶ�ɫ
	RECT_CUT_FOREACH(nextCutRect, &(hWidgeBase->rect))
	{
		DrawRect(&(hWidgeBase->pencil), nextCutRect);
	}

	/*�ָ���ͼ����*/
	DrawResetArea(hWidgeBase);
}
/**
 * @brief �¼��ص�����
 *
 * @param hObject
 * @param hMsg
 * @return PUBLIC
 */
PUBLIC int8_t WidgetCallBack(void *hObject, p_msg_t hMsg)
{
	assert(hObject);
	assert(hMsg);
	p_widget_base_t hWidgeBase = hObject;

	if (!_GetVisable(hWidgeBase))
	{
		return RES_ASSERT_ERR;
	}
	if (hMsg->msgType == MSG_TOUCH)
	{

		if (_IsDrawCheckPointR(hMsg->msgVal.rect.x, hMsg->msgVal.rect.y, (&(hWidgeBase->rect))))
		{
			switch (hMsg->msgID)
			{
			case MSG_TOUCH_MOVE:
				break;
			case MSG_TOUCH_PRESS:
				/*����*/
				if (hWidgeBase->viewClickCallBack != NULL)
				{
					hWidgeBase->viewClickCallBack(hWidgeBase, hWidgeBase->arg, _GetBtnStatus(hWidgeBase));
				}
				_SetBtnPress(hWidgeBase);
				WindowsInvaildRect(WIDGE_PARENT(hWidgeBase), (p_xrect_t)hWidgeBase);
				break;
			case MSG_TOUCH_RELEASE:
				/*�ɿ�*/
				if (_GetBtnStatus(hWidgeBase))
				{
					if (hWidgeBase->viewClickCallBack != NULL)
					{
						hWidgeBase->viewClickCallBack(hWidgeBase, hWidgeBase->arg, _GetBtnStatus(hWidgeBase));
					}
					_SetBtnRelease(hWidgeBase);
					WindowsInvaildRect(WIDGE_PARENT(hWidgeBase), (p_xrect_t)hWidgeBase);
				}
				break;
			}
			return RES_OK;
		}
		else
		{
			/*�ɿ�*/
			if (_GetBtnStatus(hWidgeBase))
			{
				if (hWidgeBase->viewClickCallBack != NULL)
				{
					hWidgeBase->viewClickCallBack(hWidgeBase, hWidgeBase->arg, _GetBtnStatus(hWidgeBase));
				}
				_SetBtnRelease(hWidgeBase);
				WindowsInvaildRect(WIDGE_PARENT(hWidgeBase), (p_xrect_t)hWidgeBase);
			}
		}
	}
	return RES_FAIL;
}
