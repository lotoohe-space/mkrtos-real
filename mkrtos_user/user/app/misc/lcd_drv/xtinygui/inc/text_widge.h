/**
 * @file text_widge.h
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
#include "x_widget.h"

#define TEXT_MARK_HEAD(a) TextWidget##a

/*文字的显示模式*/
typedef enum
{
	TEXT_TOP_LEFT_ALIGN = 0,  /*顶部左对齐*/
	TEXT_TOP_CENTER_ALIGN,	  /*顶部居中对齐*/
	TEXT_TOP_RIGHT_ALIGN,	  /*顶部右对齐*/
	TEXT_BOTTOM_LEFT_ALIGN,	  /*底部左边对齐*/
	TEXT_BOTTOM_CENTER_ALIGN, /*底部居中对齐*/
	TEXT_BOTTOM_RIGHT_ALIGN,  /*底部右对齐*/
	TEXT_LEFT_CENTER_ALIGN,	  /*居中左对齐*/
	TEXT_RIGHT_CENTER_ALIGN,  /*居中右对齐*/
	TEXT_CENTER_ALIGN		  /*居中对齐*/
} TextMode;

typedef struct
{
	widget_base_t baseWidge; /*继承widget*/
	const font_t *hFont;	 /*字体*/
	const char *text;		 /*显示的文字内容*/
	TextMode textMode;		 /*文字显示模式 0:左对齐 1:居中对齐 2:右对齐*/
} *p_text_widget_t, text_widget_t;

#define _PToHTextWidgeType(a) ((p_text_widget_t)a)

PUBLIC p_text_widget_t TextWidgetCreate(const char *text, int16_t x, int16_t y, uint16_t w, uint16_t h);
PUBLIC void TextWidgetInit(p_text_widget_t hObject, const char *text, int16_t x, int16_t y, uint16_t w, uint16_t h);
/*根据模式得到文字绘制的起始位置*/
PUBLIC p_xpoint_t TextWidgetGetPOIByTextMode(p_text_widget_t hTextWidge, p_xpoint_t startPoint);
PUBLIC void TextWidgetSetTextMode(p_text_widget_t hOjbect, TextMode textMode);
PUBLIC void TextWidgetSetFont(p_text_widget_t hObject, p_font_t hFont);
PUBLIC void TextWidgetSetText(p_text_widget_t hObject, const char *text);
PUBLIC void TextWidgetMoveTo(p_text_widget_t hObject, int16_t x, int16_t y);
PUBLIC void TextWidgetPaint(void *hObject);
PUBLIC int8_t TextWidgetCallBack(void *hObject, p_msg_t hMsg);
