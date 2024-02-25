/**
 * @file group_widge.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "group_widge.h"
#include "paint_cut.h"
#include "x_malloc.h"
#include "radio_button_widge.h"

PUBLIC p_group_widget_t GROUP_MARK_HEAD(Create)(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
	p_group_widget_t hObject;

	hObject = (p_group_widget_t)(XMalloc(sizeof(group_widget_t)));
	if (hObject == NULL)
	{
		return NULL;
	}
	GROUP_MARK_HEAD(Init)
	(hObject, x, y, w, h);
	return hObject;
}
PUBLIC uint8_t GROUP_MARK_HEAD(Init)(p_group_widget_t hBaseWidge, int16_t x, int16_t y, uint16_t w, uint16_t h)
{
	WidgetInit((p_widget_base_t)hBaseWidge, x, y, w, h);
	ListHeadInit(&hBaseWidge->widgetList);
	hBaseWidge->widgeBase.paintFun = (void *)GROUP_MARK_HEAD(Paint);
	hBaseWidge->widgeBase.moveToFun = (void *)GROUP_MARK_HEAD(MoveTo);
	hBaseWidge->widgeBase.widgeCallBackFun = (void *)GROUP_MARK_HEAD(CallBack);
	hBaseWidge->widgeBase.widgeResizeFun = (void *)GROUP_MARK_HEAD(Resize);
	hBaseWidge->widgeBase.widgeCloseFun = (void *)GROUP_MARK_HEAD(Close);

	hBaseWidge->widgeLength = 0;

	return TRUE;
}
PUBLIC void GROUP_MARK_HEAD(Close)(p_group_widget_t hObject)
{
	if (!hObject)
	{
		return;
	}
	widget_base_t *pos;

	list_foreach_without_next(pos, &_PToHGroupWidgeType(hObject)->widgetList.head, widget_node)
	{
		widget_base_t *next = list_next_entry(pos, widget_node);
		pos->widgeCloseFun(pos);

		pos = next;
	}

	ListClear(&_PToHGroupWidgeType(hObject)->widgetList);
	_PToHGroupWidgeType(hObject)->widgeLength = 0;

	WidgetClose((p_widget_base_t)hObject);
}
PUBLIC p_widget_base_t GROUP_MARK_HEAD(GetWidge)(p_group_widget_t hObject, uint16_t index)
{
	if (!hObject)
	{
		return NULL;
	}
	p_list_t hlist = ListGet(&hObject->widgetList, index);

	if (hlist != NULL)
	{
		return container_of(hlist, widget_base_t, widget_node);
	}
	else
	{
		return NULL;
	}
}
PUBLIC void GROUP_MARK_HEAD(Resize)(p_group_widget_t hObject, int16_t x, int16_t y, uint16_t w, uint16_t h)
{
	if (!hObject)
	{
		return;
	}

	int16_t dx, dy;

	dx = x - ((p_xrect_t)hObject)->x;
	dy = y - ((p_xrect_t)hObject)->y;

	WidgetResize((p_widget_base_t)hObject, x, y, w, h);
	widget_base_t *pos;

	list_foreach(pos, &hObject->widgetList.head, widget_node)
	{
		_PToHWidgeBaseType(pos)->widgeResizeFun(
			_PToHWidgeBaseType(pos),
			_PToHWidgeBaseType(pos)->rect.x + dx,
			_PToHWidgeBaseType(pos)->rect.y + dy,
			_PToHWidgeBaseType(pos)->rect.w,
			_PToHWidgeBaseType(pos)->rect.h);
	}
	// WindowsInvaildRect(((p_widget_base_t)hObject)->parentHWIN, NULL);
}
PUBLIC uint8_t GROUP_MARK_HEAD(Del)(p_group_widget_t hBaseWidge, p_widget_base_t widge)
{
	if (hBaseWidge == NULL || widge == NULL)
	{
		return FALSE;
	}
	ListDelByVal(&hBaseWidge->widgetList, &widge->widget_node);
	hBaseWidge->widgeLength--;
	widge->widgeCloseFun(widge);
	WindowsInvaildRect(((p_widget_base_t)hBaseWidge), NULL);
	return TRUE;
}
PUBLIC uint8_t GROUP_MARK_HEAD(Add)(p_group_widget_t hBaseWidge, p_widget_base_t widge)
{
	if (!hBaseWidge || !widge)
	{
		return FALSE;
	}
	ListInit(&widge->widget_node);
	ListAddLast(&hBaseWidge->widgetList, &widge->widget_node);
	widge->parentHWIN = hBaseWidge;

	widge->moveToFun(widge,
					 widge->rect.x + hBaseWidge->widgeBase.rect.x,
					 widge->rect.y + hBaseWidge->widgeBase.rect.y);
	hBaseWidge->widgeLength++;
	return TRUE;
}
PUBLIC void GROUP_MARK_HEAD(GroupProcess)(p_group_widget_t hBaseWidge, p_widget_base_t currentWidge)
{
	if (hBaseWidge == NULL)
	{
		return;
	}
	widget_base_t *pos;

	list_foreach(pos, &hBaseWidge->widgetList.head, widget_node)
	{
		if (_GET_GROUP_CTRL(pos))
		{
			if (currentWidge != pos)
			{
				RADIO_BUTTON_MARK_HEAD(SetStatus)
				((p_radio_button_widget_t)pos, FALSE);
			}
		}
	}
}
PUBLIC void GROUP_MARK_HEAD(Paint)(void *hObject)
{
	p_group_widget_t hBaseWidge;
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
	// if (!IsGUINeedCut(hWinHead)) { return; }
	if (!DrawSetArea((p_widget_base_t)hBaseWidge))
	{
		return;
	}
	cutPostionList = RectCutAddRectList(hBaseWidge->widgetList.head.next);
	DrawCutRect(hBaseWidge,
				&(hBaseWidge->widgeBase.rect));
	RectCutSplitRectList(cutPostionList);
	widget_base_t *pos;

	list_foreach(pos, &hBaseWidge->widgetList.head, widget_node)
	{
		cutPostionList = RectCutAddRectList(pos->widget_node.next);
		_PToHWidgeBaseType(pos)->paintFun(_PToHWidgeBaseType(pos));
		RectCutSplitRectList(cutPostionList);
	}
	DrawResetArea((p_widget_base_t)hBaseWidge);
}
PUBLIC void GROUP_MARK_HEAD(MoveTo)(p_group_widget_t hObject, int16_t x, int16_t y)
{
	int32_t dx = 0;
	int32_t dy = 0;

	if (!hObject)
	{
		return;
	}
	dx = x - (hObject->widgeBase.rect.x);
	dy = y - (hObject->widgeBase.rect.y);
	hObject->widgeBase.rect.x = x;
	hObject->widgeBase.rect.y = y;
	widget_base_t *pos;

	list_foreach(pos, &hObject->widgetList.head, widget_node)
	{
		pos->moveToFun(pos, pos->rect.x + dx, pos->rect.y + dy);
	}
}
PUBLIC int8_t GROUP_MARK_HEAD(CallBack)(void *hObject, p_msg_t hMsg)
{
	p_group_widget_t hBaseWidge = hObject;
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
							   &(hBaseWidge->widgeBase.rect)))
		{
			widget_base_t *pos;

			list_foreach_l(pos, hBaseWidge->widgetList.tail, widget_node)
			{
				int8_t ret;

				if ((ret = pos->widgeCallBackFun(pos, hMsg)) == 0)
				{
					return 0;
				}
			}
			return 0;
		}
	}
	return -1;
}