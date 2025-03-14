/**
 * @file radio_button_widge.h
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

#define RADIO_BUTTON_MARK_HEAD(a) RadioButtonWidge##a

typedef struct
{
	text_widget_t textWidge;
	uint8_t flag;			  /*0，1bit：当前状态*/
	xbitmap_t checkedImg; /*选中后的图片*/

} *p_radio_button_widget_t, radio_button_widget_t;

PUBLIC p_radio_button_widget_t RADIO_BUTTON_MARK_HEAD(Create)(int16_t x, int16_t y, int16_t w, int16_t h, const char *text);
PUBLIC void RADIO_BUTTON_MARK_HEAD(MoveTo)(p_radio_button_widget_t hObject, int16_t x, int16_t y);
PUBLIC void RADIO_BUTTON_MARK_HEAD(SetParentWin)(p_radio_button_widget_t hObject, p_widget_base_t parentWidge);
PUBLIC void RADIO_BUTTON_MARK_HEAD(SetStatus)(p_radio_button_widget_t hObject, uint8_t status);
PUBLIC void RADIO_BUTTON_MARK_HEAD(SetColor)(p_radio_button_widget_t hObject, uintColor color);
PUBLIC void RADIO_BUTTON_MARK_HEAD(Paint)(void *hObject);
PUBLIC int8_t RADIO_BUTTON_MARK_HEAD(CallBack)(void *hObject, p_msg_t hMsg);
PUBLIC void RADIO_BUTTON_MARK_HEAD(SetClickBack)(p_radio_button_widget_t hObject, void *arg, ViewClickCallBack viewClickCallBack);
