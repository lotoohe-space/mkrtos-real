/**
 * @file button_widge.h
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
#include "font.h"
#include "text_widge.h"

#define BUTTON_MARK_HEAD(a) ButtonWidget##a

/*按钮的管理结构体*/
typedef struct
{
	text_widget_t textWidge; /*继承文本控件*/

	uint8_t flag; /*0:bit 文字模式/图片模式 */

	p_xbitmap_t downBitmap; /*按下图片*/
	p_xbitmap_t upBitmap;	/*松开图片*/

	uintColor downColor; /*按下颜色*/
	uintColor upColor;	 /*松开颜色*/

} *p_xbutton_t, xbutton_t;

/*button模式*/
#define _SetTextMode(a) (_CLR_BIT(((p_xbutton_t)(a))->flag, 0))
#define _SetBMPMode(a) (_SET_BIT(((p_xbutton_t)(a))->flag, 0))
#define _GetBtnMode(a) (_GET_BIT(((p_xbutton_t)(a))->flag, 0))

/*按下与松开颜色*/
#define _DefaultButtonDownColor RGB565_GEN(0xff, 0xff, 0xff)
#define _DefaultButtonUpColor RGB565_GEN(200, 200, 200)
/*字体颜色*/
#define _DefaultButtonFontColor 0x0000

PUBLIC p_xbutton_t ButtonWidgetCreate(const char *text, int16_t x, int16_t y, int16_t w, int16_t h);
PUBLIC void ButtonWidgetMoveTo(p_xbutton_t hObject, int16_t x, int16_t y);
PUBLIC void ButtonWidgetSetClickBack(p_xbutton_t hObject, void *arg, ViewClickCallBack viewClickCallBack);

PUBLIC void ButtonWidgetSetDownBitmap(p_xbutton_t hObject, p_xbitmap_t hBitmap);
PUBLIC void ButtonWidgetSetUpBitmap(p_xbutton_t hObject, p_xbitmap_t hBitmap);

PUBLIC void ButtonWidgetSetText(p_xbutton_t hObject, const char *text);

PRIVATE void ButtonWidgetPaint(void *hObject);
PRIVATE int8_t ButtonWidgetCallBack(void *hObject, p_msg_t hMsg);
