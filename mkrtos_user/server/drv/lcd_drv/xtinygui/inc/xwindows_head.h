/**
 * @file xwindows_head.h
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
#include "list.h"
#include "font.h"
#include "x_widget.h"
#include "button_widge.h"
#include "group_widge.h"

typedef struct
{
	group_widget_t groupWidge; /*继承GROUP_WIDGE控件*/

	p_xbutton_t hXButtonMin;	/*最小化按钮，该按钮被添加到widgetList中*/
	p_xbutton_t hXButtonMax;	/*最大化按钮，该按钮被添加到widgetList中*/
	p_xbutton_t hXButtonClose; /*最大化按钮，该按钮被添加到widgetList中*/

} *p_win_head_t, win_head_t;

#define _DefaultHeadColor 0xc100
#define _DefaultFontColor 0xffff

// 默认的窗口名
#define _DefaultWinHeadName "window"

PUBLIC p_win_head_t WindowsHeadWidgeCreate(const char *title, int16_t x, int16_t y, int16_t w, int16_t h);
PUBLIC void WindowsHeadWidgeClose(p_win_head_t hWinHead);
PUBLIC void WindowsHeadWidgeSetVisable(void *hObject, int8_t isVisable);
PUBLIC void WindowsHeadWidgeSetMaximumBtnClickBack(p_win_head_t hObject, void *arg, ViewClickCallBack viewClickCallBack);
PUBLIC void WindowsHeadWidgeSetMinimumBtnClickBack(p_win_head_t hObject, void *arg, ViewClickCallBack viewClickCallBack);
PUBLIC void WindowsHeadWidgeSetCloseBtnClickBack(p_win_head_t hObject, void *arg, ViewClickCallBack viewClickCallBack);
PUBLIC void WindowsHeadWidgeMoveTo(p_win_head_t hObject, int16_t x, int16_t y);
PRIVATE void WindowsHeadWidgePaint(void *hObject);
PUBLIC int8_t WindowsHeadWidgeCallBack(void *hObject, p_msg_t hMsg);

/*需要抽象出来*/
PUBLIC p_widget_base_t WindowsHeadWidgeGetWidge(p_win_head_t hObject, uint16_t index);
PUBLIC void WindowsHeadWidgeResize(p_win_head_t hObject, int16_t x, int16_t y, uint16_t w, uint16_t h);

PUBLIC void WindowsHeadWidgeSetArea(p_win_head_t hObject, int16_t x, int16_t y, int16_t w, int16_t h);
PUBLIC int8_t WindowsHeadWidgeAdd(p_win_head_t hWinHead, void *widge);
