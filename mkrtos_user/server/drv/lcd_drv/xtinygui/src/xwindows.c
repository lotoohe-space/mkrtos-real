/**
 * @file xwindows.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "xwindows.h"
#include "x_malloc.h"
#include "msg.h"
#include "paint_cut.h"
#include "paint.h"
#include "gui.h"
#include <assert.h>

static void WindowsHeadBtnCallBack(void *Object, void *arg, uint8_t status);
#define WINDOWS_HEAD_H 30
/**
 * @brief Create a window.
 *
 * @param title The title of window.
 * @param x The x of win position.
 * @param y The y of win position.
 * @param w The w of win.
 * @param h The h of win.
 * @return p_win_t ==NULL is fail, != NULL is success.
 */
p_win_t WindowsCreate(const char *title, int16_t x, int16_t y, int16_t w, int16_t h)
{
	assert(title);
	/*alloc memory for window.*/
	p_win_t hWin = (p_win_t)(XMalloc(sizeof(win_t)));
	if (hWin == NULL)
	{
		return NULL;
	}
	if (GROUP_MARK_HEAD(Init)(_PToHGroupWidgeType(hWin), x, y, w, h) == FALSE)
	{
		return NULL;
	}
	/*create win head widget.*/
	hWin->hWinHead = WindowsHeadWidgeCreate(title, 0, 0, w, WINDOWS_HEAD_H);
	if (hWin->hWinHead == NULL)
	{
		_PToHGroupWidgeType(hWin)->widgeBase.widgeCloseFun(_PToHGroupWidgeType(hWin));
		XFree(hWin);
		return NULL;
	}
	WidgetInit((p_widget_base_t)hWin, x, y, w, h);

	/*set call back functions.*/
	_PToHWidgeBaseType(hWin)->paintFun = (WidgetPaintFunction)WindowsPaint;
	_PToHWidgeBaseType(hWin)->moveToFun = (WidgetMoveToFunction)WindowsMoveTo;
	_PToHWidgeBaseType(hWin)->widgeCallBackFun = (WidgetCallBackFunction)WindowsCallBack;
	_PToHWidgeBaseType(hWin)->widgeResizeFun = (WidgetResizeFunction)WindowsResize;
	_PToHWidgeBaseType(hWin)->widgeCloseFun = (WidgetCloseFunction)WindowsClose;

	_PToHWidgeBaseType(hWin)->pencil.lineType = 0xff;
	_PToHWidgeBaseType(hWin)->pencil.LineWidth = 1;
	_PToHWidgeBaseType(hWin)->pencil.DrawColor = RGB565_BLACK;
	_PToHWidgeBaseType(hWin)->pencil.DrawFrColor = _DefaultFrColor;
	_PToHWidgeBaseType(hWin)->pencil.DrawBkColor = RGB565_GEN(230, 235, 230);

	hWin->lastRect.x = x;
	hWin->lastRect.y = y;
	hWin->lastRect.w = w;
	hWin->lastRect.h = h;
	hWin->t_dx = 0;
	hWin->t_dy = 0;

	hWin->flag = 0;
	/*Windows事件处理回调函数*/
	hWin->winProcessFun = NULL;

	/*设置基础控件中的窗口标识为真*/
	_SET_IS_WIN(hWin);
	/*设置需要全部重绘*/
	_SetDrawAllLag(hWin);
	/*先设置为不显示头部*/
	_ClrDrawWinHead(hWin);

	/*窗口头部控件*/
	WindowsAdd(hWin, hWin->hWinHead);
	/*最小化的回调*/
	WindowsHeadWidgeSetMinimumBtnClickBack(hWin->hWinHead, hWin, WindowsHeadBtnCallBack);
	/*最大化回调接口*/
	WindowsHeadWidgeSetMaximumBtnClickBack(hWin->hWinHead, hWin, WindowsHeadBtnCallBack);
	WindowsHeadWidgeSetCloseBtnClickBack(hWin->hWinHead, hWin, WindowsHeadBtnCallBack);

	/*默认显示头部*/
	_SetDrawWinHead(hWin);
	return hWin;
}

/**
 * @brief 窗口中button点击事件的回调接口函数
 *
 * @param Object
 * @param arg
 * @param status
 */
static void WindowsHeadBtnCallBack(void *Object, void *arg, uint8_t status)
{
	assert(Object);
	p_win_t CentWIN = (p_win_t)arg;

	if (CentWIN == NULL)
	{
		return;
	}
	if (status)
	{
		if (CentWIN->hWinHead->hXButtonMax == Object)
		{
			/*按下最大化按钮*/
			WindowsSetMax(CentWIN);
		}
		else if (CentWIN->hWinHead->hXButtonMin == Object)
		{
			/*按下最小化按钮*/
			WindowsSetMin(CentWIN);
		}
		else if (CentWIN->hWinHead->hXButtonClose == Object)
		{
			GUISendWINDelMsg(WIDGE_PARENT(CentWIN), CentWIN);
		}
	}
}
/**
 * @brief 设置窗口处理回调事件
 *
 * @param hObject
 * @param winProcessFun
 */
void WindowsSetProcessCallBack(p_win_t hObject, WinProcessCallBack winProcessFun)
{
	assert(hObject);
	hObject->winProcessFun = winProcessFun;
}
/**
 * @brief 设置窗口最小化
 *
 * @param hObject
 */
void WindowsSetMin(p_win_t hObject)
{
	assert(hObject);
	/*处于最大化时不能够最小化*/
	if (_IsMaxWIN(hObject))
	{
		return;
	}

	if (_IsMinWIN(hObject))
	{
		_ClrMinWIN(hObject);
		/*设置为上次的大小*/
		WindowsResize(hObject,
					  WIDGE_X(hObject),
					  WIDGE_Y(hObject),
					  hObject->lastRect.w,
					  hObject->lastRect.h);
	}
	else
	{
		_SetMinWIN(hObject);
		/*保存之前的宽高*/
		hObject->lastRect.w = WIDGE_W(hObject);
		hObject->lastRect.h = WIDGE_H(hObject);
		/*设置为标题头的大小*/
		WindowsResize(hObject,
					  WIDGE_X(hObject),
					  WIDGE_Y(hObject),
					  WIDGE_W(hObject->hWinHead),
					  WIDGE_H(hObject->hWinHead));
	}
}
/**
 * @brief 设置窗口最大化
 *
 * @param hObject
 */
void WindowsSetMax(p_win_t hObject)
{
	assert(hObject);

	/*最小化时不能够最大化*/
	if (_IsMinWIN(hObject))
	{
		return;
	}

	if (_IsMaxWIN(hObject))
	{
		_ClrMaxWIN(hObject);
		/*还原位置*/
		// XRECT_COPY((p_xrect_t)hObject,&(hObject->lastRect));
		WindowsResize(hObject,
					  hObject->lastRect.x,
					  hObject->lastRect.y,
					  hObject->lastRect.w,
					  hObject->lastRect.h);
	}
	else
	{
		_SetMaxWIN(hObject);
		XRECT_COPY(&(hObject->lastRect), hObject);
		WindowsResize(hObject,
					  WIDGE_X(WIDGE_PARENT(hObject)),
					  WIDGE_Y(WIDGE_PARENT(hObject)),
					  WIDGE_W(WIDGE_PARENT(hObject)),
					  WIDGE_H(WIDGE_PARENT(hObject)));
	}
}

/**
 * @brief 重新设置窗口的大小,还需要更改内部控件的大小与位置
 *
 * @param hObject
 * @param x
 * @param y
 * @param w
 * @param h
 */
void WindowsResize(p_win_t hObject, int16_t x, int16_t y, uint16_t w, uint16_t h)
{
	assert(hObject);
	p_widget_base_t hWidgeBase;
	p_widget_base_t hWidgeBaseHead;
	int16_t dx, dy;

	/*窗口位置改变的偏移*/
	dx = x - ((p_xrect_t)hObject)->x;
	dy = y - ((p_xrect_t)hObject)->y;

	WidgetResize((p_widget_base_t)hObject, x, y, w, h);

	hWidgeBaseHead = WindowsGetWidge(hObject, 0);
	if (hWidgeBaseHead == NULL)
	{
		return;
	}
	WindowsHeadWidgeResize((p_win_head_t)hWidgeBaseHead, x, y, w, WINDOWS_HEAD_H);

	widget_base_t *pos;
	widget_base_t *head_widge = WindowsGetWidge(hObject, 1);

	if (head_widge)
	{
		list_foreach(pos, &(head_widge->widget_node), widget_node)
		{
			_PToHWidgeBaseType(pos)->moveToFun(_PToHWidgeBaseType(pos),
											   _PToHWidgeBaseType(pos)->rect.x + dx,
											   _PToHWidgeBaseType(pos)->rect.y + dy);
		}
	}

	/*改变窗口大小必须刷新父控件*/
	WindowsInvaildRect(((p_widget_base_t)hObject)->parentHWIN, NULL);
}

/**
 * @brief 获取控件
 *
 * @param hObject
 * @param index
 * @return p_widget_base_t
 */
p_widget_base_t WindowsGetWidge(p_win_t hObject, uint16_t index)
{
	assert(hObject);
	p_list_t hlist = ListGet(&_PToHGroupWidgeType(hObject)->widgetList, index);

	if (hlist != NULL)
	{
		return container_of(hlist, widget_base_t, widget_node);
	}
	else
	{
		return NULL;
	}
}

/**
 * @brief 往窗体中添加一个控件
 *
 * @param hWin
 * @param widge
 * @return int8_t
 */
int8_t WindowsAdd(p_win_t hWin, void *widge)
{
	assert(hWin);
	assert(widge);

	ListInit(&((widget_base_t *)widge)->widget_node);
	ListAddLast(&_PToHGroupWidgeType(hWin)->widgetList, &((widget_base_t *)widge)->widget_node);
	p_widget_base_t hWidge = widge;

	hWidge->moveToFun(
		hWidge,
		hWidge->rect.x + WIDGE_X(hWin),
		hWidge->rect.y + WIDGE_Y(hWin) + (_IsDrawWinHead(hWin) ? _PToHGroupWidgeType(hWin->hWinHead)->widgeBase.rect.h : 0));
	/*设置父窗口*/
	WidgetSetParentWin(hWidge, hWin);

	if (WIDGE_PARENT(hWin))
	{
		/* 设置更新窗口区域 */
		WindowsInvaildRect((p_widget_base_t)(WIDGE_PARENT(hWin)), (p_xrect_t)(hWin));
	}
	return 0;
}
/**
 * @brief 设置窗口的无效区域，调用gui的无效区域设置函数
 *
 * @param hWidgeBase
 * @param hXRect
 */
void WindowsInvaildRect(p_widget_base_t hWidgeBase, p_xrect_t hXRect)
{
	p_widget_base_t srcWidge;

	if (!hWidgeBase)
	{
		return;
	}
	/*如果父窗口是透明窗口则源为透明窗口*/
	srcWidge = hWidgeBase;
	// while (WIDGE_PARENT(srcWidge)!=NULL && _GET_IS_DPY(WIDGE_PARENT(srcWidge))) {
	//	srcWidge = WIDGE_PARENT(srcWidge);
	// }

	if (hXRect == NULL)
	{
		GUISendDrawMsg(srcWidge, MSG_WIN_INVAILD_UPDATE, 0,
					   hWidgeBase->rect.x, hWidgeBase->rect.y, hWidgeBase->rect.w, hWidgeBase->rect.h);
	}
	else
	{
		GUISendDrawMsg(srcWidge, MSG_WIN_INVAILD_UPDATE, 0,
					   hXRect->x, hXRect->y, hXRect->w, hXRect->h);
	}
}
/**
 * @brief 关闭窗口
 *
 * @param hWin
 */
void WindowsClose(p_win_t hWin)
{
	assert(hWin);
	/*在这里释放窗口中的内存*/

	/*处理该类需要释放的内存*/

	/*调用父类的关闭函数*/
	GROUP_MARK_HEAD(Close)
	((p_group_widget_t)hWin);
}
/**
 * @brief 移动窗体
 *
 * @param hWin
 * @param x
 * @param y
 */
void WindowsMoveTo(p_win_t hWin, int16_t x, int16_t y)
{
	assert(hWin);
	int32_t dx;
	int32_t dy;
	xrect_t lastRect;
	xrect_t updateRect;

	/*新位置相对于上一次移动的偏移量*/
	dx = x - WIDGE_X(hWin);
	dy = y - WIDGE_Y(hWin);

	/*位置没有改变*/
	if (x == WIDGE_X(hWin) && y == WIDGE_Y(hWin))
	{
		return;
	}
	XRECT_COPY(&lastRect, (p_xrect_t)hWin);
	/* 更新窗口，更新的位置是上一次的位置*/
	// WindowsInvaildRect((p_widget_base_t)(WIDGE_PARENT(hWin)), (p_xrect_t)(&(lastRect)));
	/*设置新的位置*/
	_PToHXRECTType(hWin)->x = x;
	_PToHXRECTType(hWin)->y = y;
	widget_base_t *pos;

	list_foreach(pos, &_PToHGroupWidgeType(hWin)->widgetList.head, widget_node)
	{
		pos->moveToFun(pos, pos->rect.x + dx, pos->rect.y + dy);
	}

	updateRect.x = MIN(((p_xrect_t)(hWin))->x, lastRect.x);
	updateRect.y = MIN(((p_xrect_t)(hWin))->y, lastRect.y);
	updateRect.w = MAX(((p_xrect_t)(hWin))->w + ((p_xrect_t)(hWin))->x,
					   lastRect.x + lastRect.w) -
				   updateRect.x;
	updateRect.h = MAX(((p_xrect_t)(hWin))->h + ((p_xrect_t)(hWin))->y,
					   lastRect.y + lastRect.h) -
				   updateRect.y;

	WindowsInvaildRect((p_widget_base_t)(WIDGE_PARENT(hWin)), (p_xrect_t)(&updateRect));
	WindowsInvaildRect((p_widget_base_t)(WIDGE_PARENT(hWin)), (p_xrect_t)hWin);
}
/**
 * @brief 设置窗口颜色
 *
 * @param hWin
 * @param color
 */
void WindowsSetColor(p_win_t hWin, uintColor color)
{
	assert(hWin);
	_PToHWidgeBaseType(hWin)->pencil.DrawBkColor = color;

	/* 更新窗口 */
	WindowsInvaildRect((p_widget_base_t)hWin, (p_xrect_t)hWin);
}

/**
 * @brief 设置是否有效
 *
 * @param hObject
 * @param isVisable
 */
void WindowsSetVisable(void *hObject, int8_t isVisable)
{
	assert(hObject);
	p_win_t hWin;

	hWin = hObject;
	WidgetSetVisable((p_widget_base_t)hWin, isVisable);
}
/**
 * @brief 重新绘制窗口
 *
 * @param hObject
 */
void WindowsPaint(void *hObject)
{
	assert(hObject);
	p_list_t cutPostionList;
	p_win_t hWin;

	hWin = hObject;
	if (_IsDrawAllLag(hWin))
	{
		/*首次的创建事件*/
		if (hWin->winProcessFun != NULL)
		{
			msg_t msg;
			msg.msgSrc = hWin;
			msg.msgType = MSG_WIN;
			msg.msgID = MSG_WIN_CREATE;
			hWin->winProcessFun(hWin, _PToHWidgeBaseType(hWin)->arg, &msg);
		}
	}
	/*是否需要显示*/
	if (!_GetVisable(hWin))
	{
		return;
	}

	/*是否需要剪裁*/
	if (!IsGUINeedCut((p_widget_base_t)hWin) && !_IsDrawAllLag(hWin))
	{
		return;
	}

	/*初次需要全部刷新*/
	_ClrDrawAllLag(hWin);

	/*设置当前绘图区域*/
	if (!DrawSetArea((p_widget_base_t)hWin))
	{
		return;
	}

	int draw_rect_sub = 0;
	/*根据是否添加了窗体边框来设置剪裁域*/
	if (_IsDrawWinHead(hWin))
	{
		/*切换到当前剪裁域*/
		cutPostionList = RectCutAddRectList(_PToHGroupWidgeType(hWin)->widgetList.head.next);
		draw_rect_sub = WINDOWS_HEAD_H;
	}
	else
	{
		widget_base_t *widget = WindowsGetWidge(hWin, 1);

		if (widget)
		{
			cutPostionList = RectCutAddRectList(&(WindowsGetWidge(hWin, 1)->widget_node));
		}
		draw_rect_sub = 0;
	}
	if (!_IsMinWIN(hWin))
	{
		_PToHWidgeBaseType(hWin)->pencil.DrawColor = _PToHWidgeBaseType(hWin)->pencil.DrawBkColor;
		/*画矩形*/
		DrawCutRect(hWin, &(xrect_t){
							  .x = _PToHXRECTType(hWin)->x,
							  .y = _PToHXRECTType(hWin)->y + draw_rect_sub,
							  .w = _PToHXRECTType(hWin)->w,
							  .h = _PToHXRECTType(hWin)->h - draw_rect_sub,
						  });

		/*重绘事件*/
		if (hWin->winProcessFun != NULL)
		{
			msg_t msg;
			msg.msgSrc = hWin;
			msg.msgType = MSG_WIN;
			msg.msgID = MSG_WIN_PAINT;
			hWin->winProcessFun(hWin, _PToHWidgeBaseType(hWin)->arg, &msg);
		}
	}
	if (cutPostionList)
	{
		RectCutSplitRectList(cutPostionList);
		widget_base_t *pos;

		list_foreach(pos, &_PToHGroupWidgeType(hWin)->widgetList.head, widget_node)
		{
			/*当前列表控件加入剪裁域*/
			cutPostionList = RectCutAddRectList(pos->widget_node.next);
			/*绘制控件*/
			_PToHWidgeBaseType(pos)->paintFun(_PToHWidgeBaseType(pos));
			/*退回剪裁域*/
			RectCutSplitRectList(cutPostionList);
			if (_IsMinWIN(hWin))
			{
				/*只画头部*/
				break;
			}
		}
	}

	/*恢复绘图区域*/
	DrawResetArea((p_widget_base_t)hWin);
}
/**
 * @brief 是否显示头部
 *
 * @param hWin
 * @param isEnable
 */
void WindowsSetDrawHead(p_win_t hWin, int8_t isEnable)
{
	assert(hWin);

	uint8_t i = 0;

	if (isEnable)
	{
		_SetDrawWinHead(hWin);
	}
	else
	{
		_ClrDrawWinHead(hWin);
	}

	/*前四个控件为边界和头部*/
	widget_base_t *widge = WindowsGetWidge(hWin, 0);

	if (widge != NULL)
	{
		if (isEnable)
		{
			_SetVisable(widge);
		}
		else
		{
			_ClrVisable(widge);
		}
	}
	/* 更新窗口 */
	WindowsInvaildRect((p_widget_base_t)hWin, (p_xrect_t)hWin);
}

/**
 * @brief 窗口事件处理
 *
 * @param hObject
 * @param hMsg
 * @return int8_t
 */
int8_t WindowsCallBack(void *hObject, p_msg_t hMsg)
{
	assert(hObject);
	assert(hMsg);

	p_win_t hWin = hObject;

	if (!_GetVisable(hWin))
	{
		return RES_ASSERT_ERR;
	}

	if (hMsg->msgType == MSG_TOUCH)
	{
		if (_IsDrawWinHead(hWin))
		{
			switch (hMsg->msgID)
			{
			case MSG_TOUCH_PRESS:
				// 有头才能抓取移动
				/*被抓的是不是头*/
				if (WindowsHeadWidgeCallBack(hWin->hWinHead, hMsg) == RES_OK)
				{
					hWin->t_dx = hMsg->msgVal.rect.x - WIDGE_X(hWin);
					hWin->t_dy = hMsg->msgVal.rect.y - WIDGE_Y(hWin);
					_SetWinMoveing(hWin);
					WindowsInvaildRect(WIDGE_PARENT(hWin), _PToHXRECTType(hWin));
					return 0;
				}
				break;
			case MSG_TOUCH_MOVE:
				// 有头才能抓取移动
				if (_IsWinMoving(hWin))
				{
					WindowsMoveTo(hWin,
								  hMsg->msgVal.rect.x - hWin->t_dx, hMsg->msgVal.rect.y - hWin->t_dy);
					// GUISendMoveMsg(hWin, MSG_WIN, MSG_WIN_MOVE, hMsg->msgVal.rect.x - hWin->t_dx, hMsg->msgVal.rect.y - hWin->t_dy);
					return 0;
				}
				break;
			case MSG_TOUCH_RELEASE:
				if (_IsWinMoving(hWin))
				{
					// 有头才能抓取移动
					_ClrWinMoveing(hWin);
					// return 0;
				}
				break;
			}
		}

		switch (hMsg->msgID)
		{
		case MSG_TOUCH_PRESS:
			if (_IsDrawCheckPointR(hMsg->msgVal.rect.x, hMsg->msgVal.rect.y, (xrect_t *)hWin))
			{
				if (
					hMsg->msgVal.rect.x >= WIDGE_X(hWin) + WIDGE_W(hWin) - 2 && hMsg->msgVal.rect.x <= WIDGE_X(hWin) + WIDGE_W(hWin))
				{
					hWin->t_xy = hMsg->msgVal.rect.x;
					_SetChangeWinRHSize(hWin);
					return 0;
				}
				else if (
					hMsg->msgVal.rect.y >= WIDGE_Y(hWin) + WIDGE_H(hWin) - 2 && hMsg->msgVal.rect.y <= WIDGE_Y(hWin) + WIDGE_H(hWin))
				{
					hWin->t_xy = hMsg->msgVal.rect.y;
					_SetChangeWinVSize(hWin);
					return 0;
				}
				else if (
					hMsg->msgVal.rect.x >= WIDGE_X(hWin) && hMsg->msgVal.rect.x <= WIDGE_X(hWin) + 2)
				{
					hWin->t_xy = hMsg->msgVal.rect.x;
					_SetChangeWinLHSize(hWin);
					return 0;
				}
			}
			break;
		case MSG_TOUCH_MOVE:
			if (_IsChangeWinRHSize(hWin))
			{
				_PToHWidgeBaseType(hWin)->widgeResizeFun(hWin, ((p_xrect_t)hWin)->x, ((p_xrect_t)hWin)->y,
														 ((p_xrect_t)hWin)->w + (hMsg->msgVal.rect.x - hWin->t_xy), ((p_xrect_t)hWin)->h);
				hWin->t_xy = hMsg->msgVal.rect.x;
				return 0;
			}
			else if (_IsChangeWinVSize(hWin))
			{
				_PToHWidgeBaseType(hWin)->widgeResizeFun(hWin, ((p_xrect_t)hWin)->x, ((p_xrect_t)hWin)->y,
														 ((p_xrect_t)hWin)->w, (((p_xrect_t)hWin)->h + (hMsg->msgVal.rect.y - hWin->t_xy)));
				hWin->t_xy = hMsg->msgVal.rect.y;
				return 0;
			}
			else if (_IsChangeWinLHSize(hWin))
			{
				_PToHWidgeBaseType(hWin)->widgeResizeFun(hWin,
														 ((p_xrect_t)hWin)->x + (hMsg->msgVal.rect.x - hWin->t_xy),
														 ((p_xrect_t)hWin)->y,
														 ((p_xrect_t)hWin)->w - (hMsg->msgVal.rect.x - hWin->t_xy),
														 ((p_xrect_t)hWin)->h);
				hWin->t_xy = hMsg->msgVal.rect.x;
				return 0;
			}
			break;
		case MSG_TOUCH_RELEASE:
			if (_IsChangeWinRHSize(hWin))
			{
				_ClrChangeWinRHSize(hWin);
				return 0;
			}
			if (_IsChangeWinVSize(hWin))
			{
				_ClrChangeWinVSize(hWin);
				return 0;
			}
			if (_IsChangeWinLHSize(hWin))
			{
				_ClrChangeWinLHSize(hWin);
				return 0;
			}
			//
			break;
		}
		if (!_IsDrawCheckPoint(hMsg->msgVal.rect.x, hMsg->msgVal.rect.y,
							   WIDGE_X(hWin), WIDGE_Y(hWin),
							   WIDGE_W(hWin), WIDGE_H(hWin)))
		{
			return 2;
		}
		widget_base_t *pos;

		list_foreach_l(pos, _PToHGroupWidgeType(hWin)->widgetList.tail, widget_node)
		{
			int8_t ret;
			if ((ret = pos->widgeCallBackFun(pos, hMsg)) == RES_OK)
			{
				return RES_OK;
			}
		}

		return RES_FAIL;
	}
	else if (hMsg->msgType == MSG_KEY)
	{
		if (hWin->winProcessFun != NULL)
		{
			hWin->winProcessFun(hWin, _PToHWidgeBaseType(hWin)->arg, hMsg);
		}
		return RES_FAIL;
	}

	return RES_FAIL;
}
