/**
 * @file x_widget.h
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include "x_types.h"
#include "widget_define.h"
#include "x_tool.h"
#include "list.h"
typedef struct widget_base
{
	xrect_t rect;	 /*控件大小*/
	pencil_t pencil; /*画笔，设置有效绘图大小，以及颜色*/

	/*内部调用使用*/
	WidgetPaintFunction paintFun;			 /*控件刷新函数*/
	WidgetMoveToFunction moveToFun;			 /*控件移动函数*/
	WidgetCallBackFunction widgeCallBackFun; /*控件事件回调函数*/
	WidgetCloseFunction widgeCloseFun;		 /*控件关闭函数*/
	WidgetResizeFunction widgeResizeFun;	 /*控件重设大小函数*/

	/*外部调用，点击事件回调函数，所有控件共有的特性*/
	ViewClickCallBack viewClickCallBack;
	/*可以传递一个参数*/
	void *arg;

	list_t widget_node;

	void *parentHWIN; /*父控件*/
	uint8_t flag;	  /*
				  0:bit 是否为窗口
				  1:bit 是否被点击
				  2:bit 是否显示
				  3:bit 是否有焦点(还未实现)
				  4:bit 是否需要进行透明处理
				  5:bit 是否(在group_widge中需要特殊处理)
				  */

} *p_widget_base_t, widget_base_t;

/*控件是否需要开关联动*/
#define _SET_GROUP_CTRL(a) _SET_BIT((((p_widget_base_t)(a))->flag), 5)
#define _CLR_GROUP_CTRL(a) _CLR_BIT((((p_widget_base_t)(a))->flag), 5)
#define _GET_GROUP_CTRL(a) _GET_BIT((((p_widget_base_t)(a))->flag), 5)

/*控件是否需要进行透明处理*/
#define _SET_IS_DPY(a) _SET_BIT((((p_widget_base_t)(a))->flag), 4)
#define _CLR_IS_DPY(a) _CLR_BIT((((p_widget_base_t)(a))->flag), 4)
#define _GET_IS_DPY(a) _GET_BIT((((p_widget_base_t)(a))->flag), 4)

/*该widge是不是窗口*/
#define _SET_IS_WIN(a) _SET_BIT((((p_widget_base_t)(a))->flag), 0)
#define _CLR_IS_WIN(a) _CLR_BIT((((p_widget_base_t)(a))->flag), 0)
#define _GET_IS_WIN(a) _GET_BIT((((p_widget_base_t)(a))->flag), 0)

/*是否被点击*/
#define _SetBtnPress(a) _SET_BIT(((p_widget_base_t)(a))->flag, 1)
#define _SetBtnRelease(a) _CLR_BIT(((p_widget_base_t)(a))->flag, 1)
#define _GetBtnStatus(a) _GET_BIT(((p_widget_base_t)(a))->flag, 1)

/*是否显示*/
#define _SetVisable(a) _SET_BIT(((p_widget_base_t)(a))->flag, 2)
#define _ClrVisable(a) _CLR_BIT(((p_widget_base_t)(a))->flag, 2)
#define _GetVisable(a) _GET_BIT(((p_widget_base_t)(a))->flag, 2)

#define _PToHWidgeBaseType(a) ((p_widget_base_t)a) /*转换为HWIDGE_BASE对象*/

PUBLIC p_widget_base_t WidgetCreate(int16_t x, int16_t y, int16_t w, int16_t h);
PUBLIC p_widget_base_t WidgetCreateEx(p_xrect_t hXRect);
PUBLIC void WidgetClose(p_widget_base_t hObject);
PUBLIC void WidgetInit(p_widget_base_t hWidgeBase, int16_t x, int16_t y, int16_t w, int16_t h);
PUBLIC void WidgetSetClickBack(p_widget_base_t hObject, void *arg, ViewClickCallBack viewClickCallBack);
PUBLIC void WidgetSetVisable(p_widget_base_t hObject, uint8_t isVisable);
PUBLIC void WidgetResize(p_widget_base_t hObject, int16_t x, int16_t y, uint16_t w, uint16_t h);
PUBLIC void WidgetSetParentWin(p_widget_base_t hObject, void *hWIN);
PUBLIC void WidgetSetColor(p_widget_base_t hObject, uintColor color);
PUBLIC void WidgetMoveTo(p_widget_base_t hObject, int16_t x, int16_t y);
PRIVATE void WidgetPaint(void *hObject);
PUBLIC int8_t WidgetCallBack(void *hObject, p_msg_t hMsg);
