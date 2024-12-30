/**
 * @file xwindows.h
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
#include "paint.h"
#include "x_tool.h"
#include "color.h"
#include "list.h"
#include "x_widget.h"
#include "widget_define.h"
#include "xwindows_head.h"

#define WINDOWS_MARK_HEAD(a) Windows##a

/**
 * @brief 窗口结构体
 *
 */
typedef struct
{
	group_widget_t groupWidge; /*继承GROUP_WIDGE*/
	p_win_head_t hWinHead;	   /*标题头控件*/

	int16_t t_dx;
	int16_t t_dy;
	/*临时偏移量，窗口在移动的时候，
	 *需要得到一个偏移量，用这个偏移量来偏移内部的每一个控件
	 */

	xrect_t lastRect;				  /*之前窗口的大小*/
	int16_t t_xy;					  /*临时存储*/
	WinProcessCallBack winProcessFun; /*windows处理回调函数*/
	uint8_t flag;					  /*
									   * 0bit:是否绘制标题栏 (1表示绘制)
									   * 1bit:窗口移动标志位，标志窗口是否正在移动
									   * 2bit:是否需要重新剪裁与绘制
									   * 3bit:是否处于最小化
									   * 4bit:是否处于最大化
									   * 5bit:是否正在拖动窗口横向大小
									   * 6bit:是否正在拖动窗口纵向大小
									   * 7bit:是否正在拖动窗口左横向大小
									   */
} *p_win_t, win_t;

/*获取是否绘制win头部标志位*/
#define _IsDrawWinHead(a) (_GET_BIT(((p_win_t)(a))->flag, 0))
/*设置显示窗口头*/
#define _SetDrawWinHead(a) (_SET_BIT(((p_win_t)(a))->flag, 0))
/*不显示头*/
#define _ClrDrawWinHead(a) (_CLR_BIT(((p_win_t)(a))->flag, 0))

/*是否需要重新剪裁与绘制*/
#define _SetDrawAllLag(a) (_SET_BIT(((p_win_t)(a))->flag, 2))
#define _ClrDrawAllLag(a) (_CLR_BIT(((p_win_t)(a))->flag, 2))
#define _IsDrawAllLag(a) (_GET_BIT(((p_win_t)(a))->flag, 2))

/*设置是否处于最小化*/
#define _IsMinWIN(a) (_GET_BIT(((p_win_t)(a))->flag, 3))
#define _SetMinWIN(a) (_SET_BIT(((p_win_t)(a))->flag, 3))
#define _ClrMinWIN(a) (_CLR_BIT(((p_win_t)(a))->flag, 3))

/*设置是否处于最大化*/
#define _IsMaxWIN(a) (_GET_BIT(((p_win_t)(a))->flag, 4))
#define _SetMaxWIN(a) (_SET_BIT(((p_win_t)(a))->flag, 4))
#define _ClrMaxWIN(a) (_CLR_BIT(((p_win_t)(a))->flag, 4))

/*是否正在拖动窗口横向大小*/
#define _IsChangeWinRHSize(a) (_GET_BIT(((p_win_t)(a))->flag, 5))
#define _SetChangeWinRHSize(a) (_SET_BIT(((p_win_t)(a))->flag, 5))
#define _ClrChangeWinRHSize(a) (_CLR_BIT(((p_win_t)(a))->flag, 5))

#define _IsChangeWinVSize(a) (_GET_BIT(((p_win_t)(a))->flag, 6))
#define _SetChangeWinVSize(a) (_SET_BIT(((p_win_t)(a))->flag, 6))
#define _ClrChangeWinVSize(a) (_CLR_BIT(((p_win_t)(a))->flag, 6))

#define _IsChangeWinLHSize(a) (_GET_BIT(((p_win_t)(a))->flag, 7))
#define _SetChangeWinLHSize(a) (_SET_BIT(((p_win_t)(a))->flag, 7))
#define _ClrChangeWinLHSize(a) (_CLR_BIT(((p_win_t)(a))->flag, 7))

/*设置窗口正在移动*/
#define _SetWinMoveing(a) (_SET_BIT((a)->flag, 1))
/*清除窗口正在移动*/
#define _ClrWinMoveing(a) (_CLR_BIT((a)->flag, 1))
/*获取是否在移动*/
#define _IsWinMoving(a) (_GET_BIT((a)->flag, 1))

p_win_t WindowsCreate(const char *title, int16_t x, int16_t y, int16_t w, int16_t h);
void WindowsClose(p_win_t hWin);
void WindowsSetProcessCallBack(p_win_t hObject, WinProcessCallBack winProcessFun);
void WindowsSetMax(p_win_t hObject);
void WindowsSetMin(p_win_t hObject);
p_widget_base_t WindowsGetWidge(p_win_t hObject, uint16_t index);
void WindowsResize(p_win_t hObject, int16_t x, int16_t y, uint16_t w, uint16_t h);
void WindowsSetVisable(void *hObject, int8_t isVisable);
void WindowsPaint(void *hObject);
void WindowsMoveTo(p_win_t hWin, int16_t x, int16_t y);
void WindowsSetColor(p_win_t hWin, uintColor color);
int8_t WindowsAdd(p_win_t hWin, void *widge);
int8_t WindowsCallBack(void *hObject, p_msg_t hMsg);
void WindowsSetDrawHead(p_win_t hWin, int8_t isEnable);
void WindowsInvaildRect(p_widget_base_t hWidgeBase, p_xrect_t hXRect);
