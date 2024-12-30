/**
 * @file gui.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "gui.h"
#include "list.h"
#include "paint_cut.h"
#include "x_malloc.h"
#include "xwindows.h"
#include "GUI_timeout.h"
#include "GUI_cursor.h"
#include <string.h>
#include <stdio.h>
/*桌面也应该是一个窗体*/
p_xdesktop_t hXDesktop = NULL;

/*添加一个控件到桌面*/
int8_t WinListAdd(p_widget_base_t hWidgeBase)
{

	WindowsAdd(hXDesktop->desktopWin, hWidgeBase);

	return 0;
}
/*获取顶层的控件*/
p_widget_base_t WinGetTop(void)
{
	p_list_t tList = &(_PToHGroupWidgeType(hXDesktop->desktopWin)->widgetList).head;
	tList = tList->next;
	while (tList)
	{
		if (tList->next == NULL)
		{
			return container_of(tList, widget_base_t, widget_node);
		}
		tList = tList->next;
	}
	return NULL;
}
/*将一个控件移动到最顶上*/
void WinMoveTop(widget_base_t *hObject)
{
	ListDelByVal(&_PToHGroupWidgeType(hXDesktop->desktopWin)->widgetList, &hObject->widget_node);
	ListAddLast(&_PToHGroupWidgeType(hXDesktop->desktopWin)->widgetList, &hObject->widget_node);
	hXDesktop->topWin = hObject;

	WindowsInvaildRect(WIDGE_PARENT(hXDesktop->topWin), _PToHXRECTType(hXDesktop->topWin));
	/*GUISendDrawMsg(hXDesktop->topWin, MSG_WIN_INVAILD_UPDATE, 0,
		WIDGE_X(hXDesktop->topWin), WIDGE_Y(hXDesktop->topWin),
		WIDGE_W(hXDesktop->topWin), WIDGE_H(hXDesktop->topWin)
	);*/
}
void GUIInvailRect(xrect_t *rect)
{
	if (hXDesktop && hXDesktop->topWin)
	{
		WindowsInvaildRect(WIDGE_PARENT(hXDesktop->topWin), rect);
	}
}
#if USE_MUTL_LAYER
volatile int gui_busy = 0;
void GUISwLayer(void)
{
	if (gui_busy == 1)
	{
		return;
	}
	MemdevSw(hXDesktop->hMemDev);
}
#endif
/*GUI事件处理*/
void GUIEvent(void)
{
	p_msg_t hTempMsg;
	uint8_t flag = FALSE;
	/*是否第一次刷新*/
	if (_GET_FIRST_RUN(hXDesktop))
	{
		GUIEventValid();
		/*刷新桌面一次*/
		WindowsInvaildRect((p_widget_base_t)(&(hXDesktop->desktopWin->groupWidge)), NULL);
		_CLR_FIRST_RUN(hXDesktop);
	}

	/*GUI超时事件*/
	GUITimeoutProcess();

	/*触摸事件，按键事件*/
	while ((hTempMsg = GUIGetMsg()) != NULL)
	{
		if (hTempMsg->msgType == MSG_TOUCH)
		{
			/*触摸事件*/
			p_win_t tmpTopHWin = NULL;
			int8_t ret;
			widget_base_t *hWidgeBase;

			list_foreach_l(hWidgeBase, _PToHGroupWidgeType(hXDesktop->desktopWin)->widgetList.tail, widget_node)
			{
				if (_GET_IS_WIN(hWidgeBase))
				{
					/*是否为窗口,是窗口则如果被点击则，窗口置顶*/
					if ((ret = hWidgeBase->widgeCallBackFun(hWidgeBase, hTempMsg)) == 0 || ret == 1)
					{
						// 是被点击了
						tmpTopHWin = (p_win_t)hWidgeBase;
						break;
					}
				}
				else
				{
					/*不是窗口则事件直接传递*/
					hWidgeBase->widgeCallBackFun(hWidgeBase, hTempMsg);
				}
			}
			if (tmpTopHWin != NULL && tmpTopHWin != hXDesktop->topWin)
			{
				/*如果已经是顶层窗口则不需要再设置为顶层窗口*/
				if (_GET_IS_WIN(tmpTopHWin))
				{
					WinMoveTop(tmpTopHWin);
				}
			}
		}
		else if (hTempMsg->msgType == MSG_KEY)
		{
			if (hXDesktop->topWin != NULL)
			{
				_PToHWidgeBaseType(hXDesktop->topWin)->widgeCallBackFun(hXDesktop->topWin, hTempMsg);
			}
		}
		GUIDelMsg(hTempMsg);
	}
#if USE_CURSOR
	while ((hTempMsg = GUIGetCursorMsg()) != NULL)
	{
		if (hTempMsg->msgType == MSG_CURSOR)
		{
			if (hTempMsg->msgID == MSG_CURSOR_MOVE)
			{
				/*设置位置*/
				GUICursorSetPOI(hTempMsg->msgVal.rect.x, hTempMsg->msgVal.rect.y);
			}
		}
		GUIDelCursorMsg(hTempMsg);
	}
#endif
	/*窗口删除*/
	while ((hTempMsg = GUIGetWINDelMsg()) != NULL)
	{
		if (hTempMsg->msgType == MSG_WIN)
		{
			if (hTempMsg->msgID == MSG_WIN_DEL_WIDGE)
			{ /*删除窗口*/
				GROUP_MARK_HEAD(Del)
				(hTempMsg->msgSrc, hTempMsg->msgVal.v);
			}
		}
		GUIDelWinDelMsg(hTempMsg);
	}
	int to_update = 0;

	// printf("gui data %x.\n", GUIGetBuffer());
	/*刷新事件*/
	while ((hTempMsg = GUIGetDrawMsg()) != NULL)
	{
		if (hTempMsg->msgType == MSG_WIN_INVAILD_UPDATE)
		{
#if USE_MUTL_LAYER
			gui_busy = 1;
#endif
			GUIUpdate(hTempMsg->msgSrc, (p_xrect_t)(&(hTempMsg->msgVal.rect)));
			to_update = 1;
		}
		GUIDelDrawMsg(hTempMsg);
	}
	if (to_update)
	{
#if USE_MUTL_LAYER
		if (hXDesktop->hMemDev->mem == hXDesktop->hMemDev->mem1)
		{
			memcpy(hXDesktop->hMemDev->mem2, hXDesktop->hMemDev->mem, hXDesktop->hMemDev->memSize);
		}
		else
		{
			memcpy(hXDesktop->hMemDev->mem1, hXDesktop->hMemDev->mem, hXDesktop->hMemDev->memSize);
		}
		gui_busy = 0;
#endif
	}
}
void GUIUpdate(p_widget_base_t hWidgeBase, p_xrect_t hXRect)
{
#if USE_MEM_DEV
	int16_t i, j;
	xrect_t rRect;
	xrect_t oldDrawRect;
#endif
	SetUpdateArea(hWidgeBase, hXRect);
#if USE_MEM_DEV
	if (MemDevReset(hXDesktop->hMemDev, DESKTOP_DRAW_AREA(hXDesktop)->x, DESKTOP_DRAW_AREA(hXDesktop)->y,
					DESKTOP_DRAW_AREA(hXDesktop)->w, DESKTOP_DRAW_AREA(hXDesktop)->h) == FALSE)
	{
		return;
	}
#if USE_FRAMEBUFFER_MODE == 0
	/*保存之前的*/
	XRECT_COPY(&oldDrawRect, DESKTOP_DRAW_AREA(hXDesktop));
	for (i = DESKTOP_DRAW_AREA(hXDesktop)->y;
		 i < DESKTOP_DRAW_AREA(hXDesktop)->y + DESKTOP_DRAW_AREA(hXDesktop)->h;
		 i += hXDesktop->hMemDev->rect.h)
	{
		/*与存储设备区域计算，得到相交区域*/
		GetOverLapRect(DESKTOP_DRAW_AREA(hXDesktop), &(hXDesktop->hMemDev->rect), &rRect);
		if (rRect.w == 0 || rRect.h == 0)
		{
			hXDesktop->hMemDev->rect.y += hXDesktop->hMemDev->rect.h;
			XRECT_COPY(DESKTOP_DRAW_AREA(hXDesktop), &oldDrawRect);
			continue;
		}
		XRECT_COPY(DESKTOP_DRAW_AREA(hXDesktop), &rRect);
		MemDevReset(hXDesktop->hMemDev, rRect.x, rRect.y,
					rRect.w, rRect.h);
#endif
#endif
		GUIExec();
#if USE_MEM_DEV
		GUICurosrUpdate();
#if USE_FRAMEBUFFER_MODE == 0
		MemDevCopyToLCD(hXDesktop->hMemDev);
		hXDesktop->hMemDev->rect.y += hXDesktop->hMemDev->rect.h;
		XRECT_COPY(DESKTOP_DRAW_AREA(hXDesktop), &oldDrawRect);
	}
	XRECT_COPY(DESKTOP_DRAW_AREA(hXDesktop), &oldDrawRect);
#endif
#endif
}
/*设置更新区域*/
void SetUpdateArea(p_widget_base_t hWidgeBase, p_xrect_t hXRect)
{
	xrect_t rRect;

	hXDesktop->updateParentWidge = hWidgeBase;
	/*绘图区域与桌面取相交，然后赋值给桌面Pencil*/
	GetOverLapRect((p_xrect_t)(&(hXDesktop->desktopWin->groupWidge)), hXRect, &rRect);
	/*重新设置控件的有效绘图区域*/
	XRECT_COPY(&(hXDesktop->desktopWin->groupWidge.widgeBase.pencil), &rRect);
	/*放置更新区域*/
	XRECT_COPY(&(hXDesktop->updateArea), hXRect);
}
#if USE_FRAMEBUFFER_MODE
static int frame_cnt;
void *GUIGetBuffer(void)
{
#if USE_MUTL_LAYER

	if (frame_cnt++ < 120)
	{
		return hXDesktop->hMemDev->mem2;
	}
	else
	{
		if (hXDesktop->hMemDev->mem == hXDesktop->hMemDev->mem2)
		{
			return hXDesktop->hMemDev->mem1;
		}
		else
		{
			return hXDesktop->hMemDev->mem2;
		}
	}
#else
	return hXDesktop->hMemDev->mem;
#endif
}
#endif
/*GUI初始化*/
p_xdesktop_t GUIInit(void)
{
	GUIMsgEventInit(); /*事件初始化*/

	/*创建一个桌面*/
	hXDesktop = (p_xdesktop_t)XMalloc(sizeof(xdesktop_t));
	if (hXDesktop == NULL)
	{
		return NULL;
	}
	hXDesktop->desktopWin = WindowsCreate("", 0, 0, LCD_SCREEN_W, LCD_SCREEN_H);
	if (hXDesktop->desktopWin == NULL)
	{
		XFree(hXDesktop);
		return NULL;
	}
	_SET_FIRST_RUN(hXDesktop);
	hXDesktop->topWin = NULL;
#if USE_MEM_DEV
#if USE_FRAMEBUFFER_MODE
	hXDesktop->hMemDev = MemDevCreate(&(xrect_t){0, 0, LCD_SCREEN_W, LCD_SCREEN_H}, GUI_COLOR_DEEP); /*创建一个存储设备*/
#else
	hXDesktop->hMemDev = MemDevCreate(&(xrect_t){0, 0, LCD_SCREEN_W, LCD_CACHE_LINE}, GUI_COLOR_DEEP); /*创建一个存储设备*/
#endif
#endif /*USE_MEM_DEV*/

#if USE_CURSOR
	GUICursorInit();
#endif /*USE_CURSOR*/

	/*设置桌面默认颜色*/
	WindowsSetColor(hXDesktop->desktopWin, RGB565_GEN(0, 128, 230));
	/*设置桌面不显示头部以及边框*/
	WindowsSetDrawHead(hXDesktop->desktopWin, FALSE);

	/*剪裁初始化*/
	if (!RectCutInit())
	{
		WindowsClose(hXDesktop->desktopWin);
		XFree(hXDesktop);
		return NULL;
	}
	return hXDesktop;
}

/*当前的窗口是否需要剪裁
	TRUE:需要剪裁
*/
BOOL IsGUINeedCut(p_widget_base_t hWidgeBase)
{
	p_xrect_t hXRECT;
	uint8_t flag = 0;
	if (hWidgeBase == NULL)
	{
		return TRUE;
	}
	hXRECT = (p_xrect_t)(hWidgeBase);
#if USE_ALPHA
	/*如果透明则需要剪裁，也就是需要绘制*/
	if (_GET_IS_DPY(hXRECT))
	{
		return TRUE;
	}
	///*如果当前窗口时移动窗口则刷新*/
	// if (memcmp(hXRECT ,&(hXDesktop->movingRect),sizeof(xrect_t)) == 0) {
	//	return TRUE;
	// }
	/*如果当前窗口和移动窗口产生了碰撞则刷新*/
	if (_IsDrawCheckArea(hXDesktop->updateArea.x, hXDesktop->updateArea.y, hXDesktop->updateArea.w, hXDesktop->updateArea.h, hXRECT->x, hXRECT->y, hXRECT->w, hXRECT->h))
	{
		return TRUE;
	}
	if (WIDGE_PARENT(hWidgeBase) != NULL)
	{
		/*有父窗口才能操作*/
		/*如果当前窗口上面有透明窗口则刷新*/
		widget_base_t *pos;

		list_foreach(pos, &((_PToHGroupWidgeType((WIDGE_PARENT(hWidgeBase))))->widgetList.head), widget_node)
		{
			p_xrect_t hItemRect = (p_xrect_t)(pos);
			if (flag == 1)
			{
				if (_GET_IS_DPY(hItemRect))
				{
					if (_IsDrawCheckArea(
							hItemRect->x, hItemRect->y, hItemRect->w, hItemRect->h, hXRECT->x, hXRECT->y, hXRECT->w, hXRECT->h))
					{
						return TRUE;
					}
				}
			}
			if (pos == hWidgeBase)
			{
				flag = 1;
			}
		}
	}

	return FALSE;
#else
	/*如果透明则需要剪裁，也就是需要绘制*/
	if (_GET_IS_DPY(hXRECT))
	{
		return TRUE;
	}

	// if (memcmp(hXRECT, &(hXDesktop->movingRect), sizeof(xrect_t)) == 0) {
	//	return TRUE;
	// }
	return _IsDrawCheckArea(
		hXDesktop->updateArea.x, hXDesktop->updateArea.y, hXDesktop->updateArea.w, hXDesktop->updateArea.h, hXRECT->x, hXRECT->y, hXRECT->w, hXRECT->h);

#endif
}
/*设置允许绘图的窗口，是当前窗口与桌面的相交区域*/
uint8_t DrawSetArea(p_widget_base_t hWidgeBase)
{

	xrect_t rRect;
	if (!hWidgeBase)
	{
		return FALSE;
	}
	if (!WIDGE_PARENT(hWidgeBase))
	{ /*如果没有父控件，则当前控件的绘图区域等于控件大小*/
		/*		XRECT_COPY(&(hWidgeBase->pencil), hWidgeBase)*/;
		return TRUE;
	}

	/*控件矩形与父控件的可绘制区域相交矩形即为绘图区域*/
	GetOverLapRect((p_xrect_t)(&(((p_widget_base_t)(hWidgeBase->parentHWIN))->pencil)), &(hWidgeBase->rect), &rRect);
	if (rRect.w == 0 || rRect.h == 0)
	{
		return FALSE;
	}
	/*重新设置控件的有效绘图区域*/
	XRECT_COPY(&(hWidgeBase->pencil), &rRect);

	return TRUE;
}
/*复位绘图区域*/
void DrawResetArea(p_widget_base_t hWidgeBase)
{
	if (!hWidgeBase)
	{
		return;
	}
	// XRECT_COPY(_PToHXRECTType(&(hWidgeBase->pencil)) ,hWidgeBase);
}
// GUI执行函数
void GUIExec(void)
{
#if !USE_ALPHA
	if (!_GET_IS_DPY(hXDesktop->updateParentWidge) && hXDesktop->updateParentWidge == (p_widget_base_t)(hXDesktop->topWin))
	{
		/*如果是更新的窗口不是透明窗口，且是顶层窗口,则直接刷新顶层窗口*/
		hXDesktop->updateParentWidge->paintFun(hXDesktop->updateParentWidge);
	}
	else
#endif
	{
		_PToHWidgeBaseType(hXDesktop->desktopWin)->paintFun(hXDesktop->desktopWin); /*桌面重绘*/
	}
}
