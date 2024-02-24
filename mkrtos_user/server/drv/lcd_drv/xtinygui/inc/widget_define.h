/**
 * @file widget_define.h
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "msg.h"

enum brush_type
{
	NORMAL_TYPE,
	CIRCLE_TYPE,
	RECT_TYPE,
};
/*画笔的定义*/
typedef struct
{
	/*允许被绘制的区域,全0XFFFF表示无效区域*/
	int16_t x;
	int16_t y;
	uint16_t w;
	uint16_t h;

	uint16_t LineWidth;
	uint8_t lineType;
	enum brush_type brush_type;

	uintColor DrawColor;   /*画笔颜色*/
	uintColor DrawFrColor; /*前景色*/
	uintColor DrawBkColor; /*背景色*/
} * p_pencil_t, pencil_t;

/*矩形*/
typedef struct
{
	int16_t x;
	int16_t y;
	int16_t w;
	int16_t h;
} * p_xrect_t, xrect_t;

/*点*/
typedef struct
{
	int16_t x;
	int16_t y;
} * p_xpoint_t, xpoint_t;

/*横线*/
typedef struct
{
	int16_t x0; //!< x开始位置
	int16_t y;	//!< y位置
	int16_t x1; //!< x结束位置
} * p_xhline_t, xhline_t;

/*变量强转为矩形*/
#define _PToHXRECTType(a) ((p_xrect_t)(a))

/*刷新函数*/
typedef void (*WidgetPaintFunction)(void *hObject);
/*移动函数*/
typedef void (*WidgetMoveToFunction)(void *hObject, int16_t x, int16_t y);
/*事件回调函数*/
typedef int8_t (*WidgetCallBackFunction)(void *hObject, p_msg_t hMsg);
/*关闭函数*/
typedef void (*WidgetCloseFunction)(void *hObject);
/*重设大小函数*/
typedef void (*WidgetResizeFunction)(void *hOjbect, int16_t x, int16_t y, uint16_t w, uint16_t h);

/*窗口事件回调*/
typedef uint8_t (*WinProcessCallBack)(void *Object, void *arg, p_msg_t hMsg);
/*按钮事件回调*/
typedef void (*ViewClickCallBack)(void *Object, void *arg, uint8_t status);
