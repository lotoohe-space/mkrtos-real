/**
 * @file group_widge.h
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include "x_widget.h"
#include "list.h"

#define GROUP_MARK_HEAD(a) GroupWidge##a

/*组控件*/
typedef struct
{
	widget_base_t widgeBase; /*继承Widge*/
	list_head_t widgetList;	 /*控件列表*/
	uint16_t widgeLength;	 /*控件个数*/
} *p_group_widget_t, group_widget_t;

#define _PToHGroupWidgeType(a) ((p_group_widget_t)a)

PUBLIC p_group_widget_t GROUP_MARK_HEAD(Create)(int16_t x, int16_t y, uint16_t w, uint16_t h);
PUBLIC uint8_t GROUP_MARK_HEAD(Init)(p_group_widget_t hBaseWidge, int16_t x, int16_t y, uint16_t w, uint16_t h);
PUBLIC void GROUP_MARK_HEAD(Close)(p_group_widget_t hObject);
PUBLIC p_widget_base_t GROUP_MARK_HEAD(GetWidge)(p_group_widget_t hObject, uint16_t index);
PUBLIC void GROUP_MARK_HEAD(Resize)(p_group_widget_t hObject, int16_t x, int16_t y, uint16_t w, uint16_t h);

PUBLIC void GROUP_MARK_HEAD(GroupProcess)(p_group_widget_t hBaseWidge, p_widget_base_t currentWidge);
/*删除一个控件*/
PUBLIC uint8_t GROUP_MARK_HEAD(Del)(p_group_widget_t hBaseWidge, p_widget_base_t widge);
PUBLIC uint8_t GROUP_MARK_HEAD(Add)(p_group_widget_t hBaseWidge, p_widget_base_t widge);
PUBLIC void GROUP_MARK_HEAD(Paint)(void *hObject);
PUBLIC void GROUP_MARK_HEAD(MoveTo)(p_group_widget_t hObject, int16_t x, int16_t y);
PUBLIC int8_t GROUP_MARK_HEAD(CallBack)(void *hObject, p_msg_t hMsg);
