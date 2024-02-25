/**
 * @file paint.h
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
#include "bitmap.h"
uintColor _GetPixel(int16_t x, int16_t y);
void DrawPixel(uintColor color, int16_t x, int16_t y);
void DrawAPixel(uintColor aColor, int16_t x, int16_t y);
void DrawLine(void *obj, xpoint_t *st_point, xpoint_t *end_point);
void DrawSquare(void *obj, xpoint_t *st_p, int w);
void DrawCircle(void *obj, xpoint_t *center, int radius);
void DrawArc(void *obj, xpoint_t *cen, float alpha1, float alpha2, float R);
void DrawPie(void *obj,
			 xpoint_t *cen,
			 int r, // 扇形半径
			 float start_angle,
			 float end_angle // 扇形弧度
);
void DrawPie2(void *obj, xpoint_t *center, int r, int st_angle, int end_angle);
void DrawFillCircle(void *obj, xpoint_t *cen, int r);
uint8_t DrawRect(p_pencil_t hPencil, p_xrect_t hXRECT);
uint8_t DrawCutRect(void *hObject, p_xrect_t hXRECT);
uint8_t DrawChar(p_pencil_t hPencil, const p_font_t hFont, p_xrect_t border, p_xrect_t bgBorder, char ch);
uint8_t DrawString(p_pencil_t hPencil, const p_font_t hFont, p_xrect_t dHRect, int16_t x, int16_t y, uint8_t *text);

uint8_t DrawCutChar(void *hObject, const p_font_t hFont, p_xrect_t bgRect, p_xpoint_t hStartXPoint, char ch);
uint8_t DrawCutString(void *hObject, const p_font_t hFont, p_xrect_t border, p_xpoint_t hXPoint, const char *text);
uint8_t DrawCutBitmap(void *hObject, p_xrect_t border, p_xbitmap_t hXBitmap);

extern void DMA2D_GUICopy(xrect_t *src_rect, int src_wh, uint8_t *bitmap,
						  xrect_t *dst_rest, int dst_wh, uint8_t *dst_bitmap);
