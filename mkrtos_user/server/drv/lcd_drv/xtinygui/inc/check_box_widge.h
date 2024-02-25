/**
 * @file check_box_widge.h
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "bitmap.h"
#include "text_widge.h"
#define CHECKBOX_MARK_HEAD(a) CheckBoxWidget##a

typedef struct
{
	text_widget_t textWidge;
	xbitmap_t checkedImg; /*选中后的图片*/
	uint8_t flag;		  /*0，1bit：当前状态*/
} *p_checkbox_widget_t, checkbox_widget_t;

PUBLIC p_checkbox_widget_t CheckBoxWidgetCreate(int16_t x, int16_t y, int16_t w, int16_t h, const char *text);
PUBLIC void CheckBoxWidgetMoveTo(p_checkbox_widget_t hObject, int16_t x, int16_t y);
PUBLIC void CheckBoxWidgetSetParentWin(p_checkbox_widget_t hObject, p_widget_base_t parentWidge);
PUBLIC void CheckBoxWidgetSetColor(p_checkbox_widget_t hObject, uintColor color);
PUBLIC void CheckBoxWidgetPaint(void *hObject);
PUBLIC int8_t CheckBoxWidgetCallBack(void *hObject, p_msg_t hMsg);
PUBLIC void CheckBoxWidgetSetClickBack(p_checkbox_widget_t hObject, void *arg, ViewClickCallBack viewClickCallBack);
