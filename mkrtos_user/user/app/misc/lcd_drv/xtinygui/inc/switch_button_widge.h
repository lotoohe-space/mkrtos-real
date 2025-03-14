/**
 * @file switch_button_widge.h
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
// #include "x_widget.h"
#include "bitmap.h"
// #include "xwindows.h"
#include "text_widge.h"
#define SWITCH_BUTTON_WIDGE_MARK_HEAD(a) SwitchButtonWidge##a

typedef struct
{
	text_widget_t textWidge;

	const char *offText;
	const char *onText;
	p_xbitmap_t offBitmap; /*关闭的图片*/
	p_xbitmap_t onBitmap;  /*打开的图片*/

	uint8_t flag; /*0，1bit：当前状态 2:bit 文字模式/图片模式 */
} *p_switch_button_widget_t, switch_button_widget_t;

/*控件是否需要进行透明处理*/
#define _SET_SW_TEXT_MODE(a) _CLR_BIT((((p_switch_button_widget_t)(a))->flag), 2)
#define _SET_SW_BMP_MODE(a) _SET_BIT((((p_switch_button_widget_t)(a))->flag), 2)
#define _GET_SW_MODE(a) _GET_BIT((((p_switch_button_widget_t)(a))->flag), 2)

PUBLIC p_switch_button_widget_t SWITCH_BUTTON_WIDGE_MARK_HEAD(Create)(int16_t x, int16_t y, int16_t w, int16_t h, const char *onText, const char *offText);
PUBLIC void SWITCH_BUTTON_WIDGE_MARK_HEAD(MoveTo)(p_switch_button_widget_t hObject, int16_t x, int16_t y);
PUBLIC void SWITCH_BUTTON_WIDGE_MARK_HEAD(SetParentWin)(p_switch_button_widget_t hObject, p_widget_base_t parentWidge);
PUBLIC void SWITCH_BUTTON_WIDGE_MARK_HEAD(SetColor)(p_switch_button_widget_t hObject, uintColor color);
PUBLIC void SWITCH_BUTTON_WIDGE_MARK_HEAD(Paint)(void *hObject);
PUBLIC int8_t SWITCH_BUTTON_WIDGE_MARK_HEAD(CallBack)(void *hObject, p_msg_t hMsg);
PUBLIC void SWITCH_BUTTON_WIDGE_MARK_HEAD(SetClickBack)(p_switch_button_widget_t hObject, void *arg, ViewClickCallBack viewClickCallBack);

PUBLIC void SWITCH_BUTTON_WIDGE_MARK_HEAD(SetOnText)(p_switch_button_widget_t hObject, const char *text);
PUBLIC void SWITCH_BUTTON_WIDGE_MARK_HEAD(SetOffText)(p_switch_button_widget_t hObject, const char *text);
PUBLIC void SWITCH_BUTTON_WIDGE_MARK_HEAD(SetOnBitmap)(p_switch_button_widget_t hObject, p_xbitmap_t hBitmap);
PUBLIC void SWITCH_BUTTON_WIDGE_MARK_HEAD(SetOffBitmap)(p_switch_button_widget_t hObject, p_xbitmap_t hBitmap);
