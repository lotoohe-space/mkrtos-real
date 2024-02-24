/**
 * @file list_widge.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "list_widge.h"
#include "paint_cut.h"
#include "x_malloc.h"

static void GUITimeoutCb(void *arg);

PUBLIC p_xlist_widget_t ListWidgetCreate(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
	p_xlist_widget_t hObject;

	hObject = (p_xlist_widget_t)(XMalloc(sizeof(xlist_widget_t)));
	if (hObject == NULL)
	{
		return NULL;
	}
	GROUP_MARK_HEAD(Init)
	((p_group_widget_t)hObject, x, y, w, h);

	_PToHGroupWidgeType(hObject)->widgeBase.paintFun = (void *)ListWidgetPaint;
	//_PToHGroupWidgeType(hObject)->widgeBase.moveToFun = GROUP_MARK_HEAD(MoveTo);
	_PToHGroupWidgeType(hObject)->widgeBase.widgeCallBackFun = (void *)ListWidgetCallBack;
	_PToHGroupWidgeType(hObject)->widgeBase.widgeCloseFun = (void *)ListWidgetClose;

	_PToHWidgeBaseType(hObject)->pencil.DrawColor = _PToHWidgeBaseType(hObject)->pencil.DrawBkColor;

	hObject->itemsSize.itemSize = LIST_WIDGE_ITEM_SIZE;
	hObject->lastXY = 0;
	hObject->flag = 0;
	hObject->itemGap = 5;
	hObject->dSP = 0;
	hObject->hGUITimeout = GUITimeoutCreate(1000 / 60, hObject, GUITimeoutCb);
	GUITimeoutClose(hObject->hGUITimeout);

	_SetListWidgeMode(hObject);
	_SetOTN_V(hObject);
	_OpenListAnimation(hObject);
	hObject->itemsSize.itemsH = 0;

	return hObject;
}
PUBLIC void ListWidgetSetFlag(p_xlist_widget_t hBaseWidge, LIST_WIDGE_FLAG flagBisPoi, BOOL status)
{
	if (hBaseWidge == NULL)
	{
		return;
	}

	hBaseWidge->flag = ((hBaseWidge->flag) & (~(1 << flagBisPoi))) | ((status & 0x01) << flagBisPoi);
}
PUBLIC void ListWidgetClose(p_xlist_widget_t hObject)
{
	if (!hObject)
	{
		return;
	}
	GUITimeoutFree(hObject->hGUITimeout);
	GROUP_MARK_HEAD(Close)
	((p_group_widget_t)hObject);
}
PUBLIC uint8_t ListWidgetAdd(p_xlist_widget_t hBaseWidge, p_widget_base_t widge)
{
	uint16_t widgeLength;
	if (hBaseWidge == NULL || widge == NULL)
	{
		return FALSE;
	}

	if (GROUP_MARK_HEAD(Add)((p_group_widget_t)hBaseWidge, widge) == FALSE)
	{
		return FALSE;
	}
	widgeLength = ((p_group_widget_t)hBaseWidge)->widgeLength - 1;
	if (!_GetListWidgeMode(hBaseWidge))
	{
		if (_GetOTN(hBaseWidge))
		{
			((p_xrect_t)widge)->x = hBaseWidge->groupWidge.widgeBase.rect.x +
									hBaseWidge->itemsSize.itemSize * widgeLength + widgeLength;
			((p_xrect_t)widge)->y = hBaseWidge->groupWidge.widgeBase.rect.y;
		}
		else
		{
			((p_xrect_t)widge)->x = hBaseWidge->groupWidge.widgeBase.rect.x;
			((p_xrect_t)widge)->y = hBaseWidge->groupWidge.widgeBase.rect.y +
									hBaseWidge->itemsSize.itemSize * widgeLength + widgeLength;
		}
		((p_xrect_t)widge)->h = hBaseWidge->itemsSize.itemSize;
		((p_xrect_t)widge)->w = hBaseWidge->groupWidge.widgeBase.rect.w;
	}
	else
	{
		if (_GetOTN(hBaseWidge))
		{
			((p_xrect_t)widge)->x = hBaseWidge->groupWidge.widgeBase.rect.x +
									hBaseWidge->itemsSize.itemsH + hBaseWidge->itemGap * widgeLength;
			((p_xrect_t)widge)->y = hBaseWidge->groupWidge.widgeBase.rect.y;
			hBaseWidge->itemsSize.itemsH += ((p_xrect_t)widge)->w;
			//((p_xrect_t)widge)->h = hBaseWidge->groupWidge.widgeBase.rect.h;
		}
		else
		{
			((p_xrect_t)widge)->x = hBaseWidge->groupWidge.widgeBase.rect.x;
			((p_xrect_t)widge)->y = hBaseWidge->groupWidge.widgeBase.rect.y +
									hBaseWidge->itemsSize.itemsH + hBaseWidge->itemGap * widgeLength;
			hBaseWidge->itemsSize.itemsH += ((p_xrect_t)widge)->h;
			//((p_xrect_t)widge)->w = hBaseWidge->groupWidge.widgeBase.rect.w;
		}
	}

	WindowsInvaildRect((p_widget_base_t)hBaseWidge, NULL);
	return TRUE;
}
PRIVATE uint8_t ListWidgetListSlide(p_xlist_widget_t hBaseWidge, int16_t dXY)
{
	p_xrect_t widgeRectLast;
	p_xrect_t widgeRectFirst;

	assert(hBaseWidge);
	widgeRectFirst = (p_xrect_t)GROUP_MARK_HEAD(GetWidge)((p_group_widget_t)hBaseWidge, 0);
	if (widgeRectFirst == NULL)
	{
		return FALSE;
	}
	widgeRectLast = (p_xrect_t)GROUP_MARK_HEAD(GetWidge)((p_group_widget_t)hBaseWidge, hBaseWidge->groupWidge.widgeLength - 1);
	if (widgeRectLast == NULL)
	{
		return FALSE;
	}
	//!< 重新计算偏移的位置
	if (_GetOTN(hBaseWidge))
	{
		if (widgeRectLast->x + dXY + widgeRectLast->w <
			((p_xrect_t)(hBaseWidge))->x + ((p_xrect_t)(hBaseWidge))->w)
		{
			dXY = ((p_xrect_t)(hBaseWidge))->x +
				  ((p_xrect_t)(hBaseWidge))->w - (widgeRectLast->x + widgeRectLast->w);
		}
		if (widgeRectFirst->x + dXY > ((p_xrect_t)(hBaseWidge))->x)
		{
			dXY = ((p_xrect_t)(hBaseWidge))->x - widgeRectFirst->x;
		}
	}
	else
	{
		if (widgeRectLast->y + dXY + widgeRectLast->h <
			((p_xrect_t)(hBaseWidge))->y + ((p_xrect_t)(hBaseWidge))->h)
		{
			dXY = (((p_xrect_t)(hBaseWidge))->y +
				   ((p_xrect_t)(hBaseWidge))->h) -
				  (widgeRectLast->y + widgeRectLast->h);
		}
		if (widgeRectFirst->y + dXY > ((p_xrect_t)(hBaseWidge))->y)
		{
			dXY = ((p_xrect_t)(hBaseWidge))->y - widgeRectFirst->y;
		}
	}
	widget_base_t *pos;

	//!< 改变内部每个组件的位置
	list_foreach(pos, &_PToHGroupWidgeType(hBaseWidge)->widgetList.head, widget_node)
	{
		p_xrect_t hWidge = (p_xrect_t)_PToHWidgeBaseType(pos);

		if (_GetOTN(hBaseWidge))
		{
			hWidge->x += dXY;
		}
		else
		{
			hWidge->y += dXY;
		}
	}

	WindowsInvaildRect(_PToHWidgeBaseType(hBaseWidge), (p_xrect_t)hBaseWidge);
	return TRUE;
}
static void GUITimeoutCb(void *arg)
{
	p_xlist_widget_t hBaseWidge = arg;
	if (arg == NULL)
	{
		return;
	}
	if (_GetListAnimation(hBaseWidge))
	{
		if (!(hBaseWidge->dSP >= -1 && hBaseWidge->dSP <= 1))
		{
			//!< 滑动listview
			if (!ListWidgetListSlide(hBaseWidge, hBaseWidge->dSP))
			{
				hBaseWidge->dSP = 0;
			}
			else
			{
				if (hBaseWidge->dSP < 0)
				{
					hBaseWidge->dSP++;
					if (hBaseWidge->dSP > 0)
					{
						hBaseWidge->dSP = 0;
					}
				}
				else if (hBaseWidge->dSP > 0)
				{
					hBaseWidge->dSP--;
					if (hBaseWidge->dSP < 0)
					{
						hBaseWidge->dSP = 0;
					}
				}
				GUITimeoutOpen(hBaseWidge->hGUITimeout);
			}
		}
	}
}
PUBLIC void ListWidgetPaint(void *hObject)
{
	p_xlist_widget_t hBaseWidge;
	p_list_t cutPostionList;
	hBaseWidge = hObject;
	if (!hBaseWidge)
	{
		return;
	}
	if (!_GetVisable(hBaseWidge))
	{
		return;
	}
	if (!IsGUINeedCut((p_widget_base_t)hBaseWidge))
	{
		return;
	}
	if (!DrawSetArea((p_widget_base_t)hBaseWidge))
	{
		return;
	}

	cutPostionList = RectCutAddRectList(_PToHGroupWidgeType(hBaseWidge)->widgetList.head.next);
	DrawCutRect(hBaseWidge,
				&(_PToHGroupWidgeType(hBaseWidge)->widgeBase.rect));
	RectCutSplitRectList(cutPostionList);
	widget_base_t *pos;

	list_foreach(pos, &_PToHGroupWidgeType(hBaseWidge)->widgetList.head, widget_node)
	{
		cutPostionList = RectCutAddRectList(pos->widget_node.next);
		_PToHWidgeBaseType(pos)->paintFun(_PToHWidgeBaseType(pos));
		RectCutSplitRectList(cutPostionList);
	}

	DrawResetArea((p_widget_base_t)hBaseWidge);
}
PUBLIC int8_t ListWidgetCallBack(void *hObject, p_msg_t hMsg)
{
	p_xlist_widget_t hBaseWidge = hObject;
	if (!hBaseWidge || !hMsg)
	{
		return -1;
	}
	if (!_GetVisable(hBaseWidge))
	{
		return -1;
	}
	if (hMsg->msgType == MSG_TOUCH)
	{
		if (_IsDrawCheckPointR(hMsg->msgVal.rect.x, hMsg->msgVal.rect.y,
							   &(_PToHGroupWidgeType(hBaseWidge)->widgeBase.rect)))
		{
			int16_t dXY = 0;

			if (hMsg->msgType == MSG_TOUCH)
			{
				switch (hMsg->msgID)
				{
				case MSG_TOUCH_PRESS:
					if (_GetOTN(hBaseWidge))
					{
						hBaseWidge->lastXY = hMsg->msgVal.rect.x;

						hBaseWidge->firstXY = hMsg->msgVal.rect.x;
					}
					else
					{
						hBaseWidge->lastXY = hMsg->msgVal.rect.y;

						hBaseWidge->firstXY = hMsg->msgVal.rect.y;
					}
					if (_GetListAnimation(hBaseWidge))
					{
						hBaseWidge->firstTime = (uint16_t)GUIGetTick();
					}
					break;
				case MSG_TOUCH_MOVE:
					if (_GetOTN(hBaseWidge))
					{
						dXY = hMsg->msgVal.rect.x - hBaseWidge->lastXY;
					}
					else
					{
						dXY = hMsg->msgVal.rect.y - hBaseWidge->lastXY;
					}
					if (dXY != 0)
					{
						_SetSliding(hBaseWidge);
					}
					if (_GetSlideState(hBaseWidge))
					{
						ListWidgetListSlide(hBaseWidge, dXY);
						if (_GetOTN(hBaseWidge))
						{
							hBaseWidge->lastXY = hMsg->msgVal.rect.x;
						}
						else
						{
							hBaseWidge->lastXY = hMsg->msgVal.rect.y;
						}
						return RES_OK;
					}
					break;
				case MSG_TOUCH_RELEASE:
					if (_GetSlideState(hBaseWidge))
					{
						if (_GetListAnimation(hBaseWidge))
						{
							int16_t dis = 0;
							if (_GetOTN(hBaseWidge))
							{
								dis = hMsg->msgVal.rect.x - hBaseWidge->firstXY;
							}
							else
							{
								dis = hMsg->msgVal.rect.y - hBaseWidge->firstXY;
							}

							hBaseWidge->dSP = 25 * (dis) / ((uint16_t)((uint16_t)GUIGetTick() - hBaseWidge->firstTime));
							if (hBaseWidge->dSP != 0)
							{
								GUITimeoutOpen(hBaseWidge->hGUITimeout);
							}
						}
						_ClrSliding(hBaseWidge);
						return RES_OK;
					}
					break;
				}
			}
			widget_base_t *pos;

			list_foreach_l(pos, _PToHGroupWidgeType(hBaseWidge)->widgetList.tail, widget_node)
			{
				int8_t ret;

				if ((ret = pos->widgeCallBackFun(pos, hMsg)) == 0)
				{
					return 0;
				}
			}

			return 0;
		}
		else
		{
			if (_GetSlideState(hBaseWidge))
			{
				if (_GetListAnimation(hBaseWidge))
				{
					int16_t dis = 0;

					if (_GetOTN(hBaseWidge))
					{
						dis = hMsg->msgVal.rect.x - hBaseWidge->firstXY;
					}
					else
					{
						dis = hMsg->msgVal.rect.y - hBaseWidge->firstXY;
					}

					hBaseWidge->dSP = (dis) / ((uint16_t)((uint16_t)GUIGetTick() - hBaseWidge->firstTime));
					if (hBaseWidge->dSP != 0)
					{
						GUITimeoutOpen(hBaseWidge->hGUITimeout);
					}
				}
				_ClrSliding(hBaseWidge);
				return RES_OK;
			}
		}
	}
	return -1;
}
