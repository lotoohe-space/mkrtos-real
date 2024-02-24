/**
 * @file progress_widge.h
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
#include "bitmap.h"
#include "xwindows.h"
#define PROGRESS_MARK_HEAD(a) ProgressWidge##a

typedef struct
{

	widget_base_t widgeBase;

	uint16_t maxVal;	   /*最大值*/
	uint16_t currentVal; /*当前值*/

} *p_progress_widget_t, progress_widget_t;

PUBLIC p_progress_widget_t PROGRESS_MARK_HEAD(Create)(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t maxVal);
PUBLIC void PROGRESS_MARK_HEAD(SetParentWin)(p_progress_widget_t hObject, p_win_t hWIN);
PUBLIC void PROGRESS_MARK_HEAD(SetColor)(p_progress_widget_t hObject, uintColor color);
PUBLIC void PROGRESS_MARK_HEAD(MoveTo)(p_progress_widget_t hObject, int16_t x, int16_t y);
PUBLIC void PROGRESS_MARK_HEAD(SetProgressVal)(p_progress_widget_t hObject, uint16_t val);

PRIVATE void PROGRESS_MARK_HEAD(Paint)(void *hObject);
PRIVATE int8_t PROGRESS_MARK_HEAD(CallBack)(void *hObject, p_msg_t hMsg);
