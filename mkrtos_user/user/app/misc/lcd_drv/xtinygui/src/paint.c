/**
 * @file paint.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "paint.h"
#include "x_tool.h"
#include "gui.h"
#include "paint_cut.h"
#include "x_widget.h"
#include "msg.h"
#include "bitmap.h"
#include "text_widge.h"
#include "mem_dev.h"
#include "GUI_cursor.h"
#include "GUI_interface_extern.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <x_math.h>

/*读点*/
inline uintColor _GetPixel(int16_t x, int16_t y)
{
#if USE_MEM_DEV
	int16_t rX = x - hXDesktop->hMemDev->rect.x;
	int16_t rY = y - hXDesktop->hMemDev->rect.y;
	return MemDevReadPT(hXDesktop->hMemDev, rX, rY);
#else
	return GUIGetPixel(x, y);
#endif
}
/*写点*/
inline void DrawPixel(uintColor color, int16_t x, int16_t y)
{
#if USE_MEM_DEV
#if USE_FRAMEBUFFER_MODE
	int16_t rX = x;
	int16_t rY = y;

	if (rX < 0 || rY < 0)
	{
		return;
	}

	if (rX >= hXDesktop->hMemDev->rect.x + hXDesktop->hMemDev->rect.w ||
		rY >= hXDesktop->hMemDev->rect.y + hXDesktop->hMemDev->rect.h)
	{
		return;
	}
#else
	int16_t rX = x - hXDesktop->hMemDev->rect.x;
	int16_t rY = y - hXDesktop->hMemDev->rect.y;

	if (rX < 0 || rY < 0)
	{
		return;
	}

	if (rX >= hXDesktop->hMemDev->rect.w ||
		rY >= hXDesktop->hMemDev->rect.h)
	{
		return;
	}
#endif
	MemDevDrawPT(hXDesktop->hMemDev, rX, rY, color);
#else /*USE_MEM_DEV*/
#if USE_CURSOR
	if (!GUICursorDrawMem(x, y, color))
	{ /*写入失败则写入到屏幕，否则写入到缓存中*/
		GUIDrawPixel(x, y, color);
	}
#else  /*USE_CURSOR*/
	GUIDrawPixel(x, y, color);
#endif /*USE_CURSOR*/
#endif /*USE_MEM_DEV*/
}
void DrawPixelX(uintColor color, int16_t x, int16_t y, int16_t h_lim)
{
	if (y < h_lim)
	{
		DrawPixel(color, x, y);
	}
}

/*画一个透明的点*/
inline void DrawAPixel(uintColor aColor, int16_t x, int16_t y)
{

	uint16_t lcColor = _GetPixel(x, y);
	uint8_t A = 0xFF - C565A(aColor);

	if (A == 0x00)
	{
		return;
	}
	else if (A == 0xff)
	{
		DrawPixel(aColor, x, y);
		return;
	}
	else
	{
		uint8_t R = ((C565R(aColor) * A + C565R(lcColor) * (0xff - A)) >> 8) & 0xff;
		uint8_t G = ((C565G(aColor) * A + C565G(lcColor) * (0xff - A)) >> 8) & 0xff;
		uint8_t B = ((C565B(aColor) * A + C565B(lcColor) * (0xff - A)) >> 8) & 0xff;

		DrawPixel(RGB565_GEN(R, G, B), x, y);
	}
}
// extern void GUIDrawRect(int16_t x, int16_t y, uint16_t w, uint16_t h, uintColor color);
extern void DMA2D_GUIDrawRect2Mem(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t wh,
								  uintColor color, uint16_t *data);

/*画矩形*/
void RawDrawRect(p_xrect_t hRect, uintColor color)
{
	int16_t i, j;

#if USE_GPU
	uintColor *data = MemDevGetPT(hXDesktop->hMemDev);
#if USE_FRAMEBUFFER_MODE
	int16_t rX = hRect->x;
	int16_t rY = hRect->y;
#else
	int16_t rX = hRect->x - hXDesktop->hMemDev->rect.x;
	int16_t rY = hRect->y - hXDesktop->hMemDev->rect.y;
#endif
	if (hRect->h != 0 && hRect->w != 0)
	{
#if USE_FRAMEBUFFER_MODE
		DMA2D_GUIDrawRect2Mem(rX, rY, hRect->w, hRect->h, hXDesktop->hMemDev->r_rect.w, color, data);
#else
		DMA2D_GUIDrawRect2Mem(rX, rY, hRect->w, hRect->h, hXDesktop->hMemDev->rect.w, color, data);
#endif
	}
#else
	for (i = hRect->y; i < hRect->y + hRect->h; i++)
	{
		for (j = hRect->x; j < hRect->x + hRect->w; j++)
		{
			DrawPixel(color, j, i);
		}
	}
#endif
}
/*画透明矩形*/
void RawDrawARect(p_xrect_t hRect, uintColor color)
{
	int16_t i, j;

	for (i = hRect->y; i < hRect->y + hRect->h; i++)
	{
		for (j = hRect->x; j < hRect->x + hRect->w; j++)
		{
			DrawAPixel(color, j, i);
		}
	}
}

/*绘制原始图片*/
void RawDrawBitmap(p_pencil_t hPencil, p_xrect_t drawBorder, p_xpoint_t startDrawBMP, p_xbitmap_t hXBitmap)
{
#if USE_GPU
	uintColor *data = MemDevGetPT(hXDesktop->hMemDev);

	if (drawBorder->h != 0 && drawBorder->w != 0)
	{
		DMA2D_GUICopy(
			&(xrect_t){
				.x = startDrawBMP->x,
				.y = startDrawBMP->y,
				.w = drawBorder->w,
				.h = drawBorder->h},
			hXBitmap->w,
			hXBitmap->pixels,
			&(xrect_t) {
#if USE_FRAMEBUFFER_MODE
				.x = drawBorder->x,
				.y = drawBorder->y,
#else
				.x = drawBorder->x - hXDesktop->hMemDev->rect.x,
				.y = drawBorder->y - hXDesktop->hMemDev->rect.y,
#endif
				.w = drawBorder->w,
				.h = drawBorder->h },
#if USE_FRAMEBUFFER_MODE
			hXDesktop->hMemDev->r_rect.w,
#else
			hXDesktop->hMemDev->rect.w,
#endif
			data);
	}
#else
	int16_t i, j;
	int16_t endy;
	int16_t endx;
	uint16_t *pixels;

	pixels = (uint16_t *)(hXBitmap->pixels);
	endy = drawBorder->h + drawBorder->y;
	endx = drawBorder->w + drawBorder->x;
	for (i = drawBorder->y; i < endy; i++)
	{
		for (j = drawBorder->x; j < endx; j++)
		{
			if ((i - drawBorder->y + startDrawBMP->y) < hXBitmap->h &&
				(j - drawBorder->x + startDrawBMP->x) < hXBitmap->w)
			{
				uintColor color = pixels[(i - drawBorder->y + startDrawBMP->y) * hXBitmap->w +
										 (j - drawBorder->x) + startDrawBMP->x];
				DrawPixel(color, j, i);
			}
			else
			{
				DrawPixel(hPencil->DrawBkColor, j, i);
			}
		}
	}
#endif
}
/*画透明图片*/
void RawDrawABitmap(
	p_pencil_t hPencil, p_xrect_t drawBorder, p_xpoint_t startDrawBMP, p_xbitmap_t hXBitmap)
{
	int16_t i, j;
	int16_t endy;
	int16_t endx;
	uint16_t *pixels;
	pixels = (uint16_t *)(hXBitmap->pixels);
	endy = drawBorder->h + drawBorder->y;
	endx = drawBorder->w + drawBorder->x;
	for (i = drawBorder->y; i < endy; i++)
	{
		for (j = drawBorder->x; j < endx; j++)
		{
			if ((i - drawBorder->y + startDrawBMP->y) < hXBitmap->h && (j - drawBorder->x + startDrawBMP->x) < hXBitmap->w)
			{
				uintColor color = pixels[(i - drawBorder->y + startDrawBMP->y) * hXBitmap->w + (j - drawBorder->x) + startDrawBMP->x];
				DrawAPixel((hXBitmap->alpha << 16) | color, j, i);
			}
			else
			{
				DrawPixel(hPencil->DrawBkColor, j, i);
			}
		}
	}
}
/*画单独通道的透明图片ARGB8888*/
void RawDrawARGBBitmap(
	p_pencil_t hPencil, p_xrect_t drawBorder, p_xpoint_t startDrawBMP, p_xbitmap_t hXBitmap)
{
	int16_t i, j;
	int16_t endy;
	int16_t endx;
	uint32_t *pixels;

	pixels = (uint32_t *)(hXBitmap->pixels);
	endy = drawBorder->h + drawBorder->y;
	endx = drawBorder->w + drawBorder->x;
	for (i = drawBorder->y; i < endy; i++)
	{
		for (j = drawBorder->x; j < endx; j++)
		{
			if ((i - drawBorder->y + startDrawBMP->y) < hXBitmap->h &&
				(j - drawBorder->x + startDrawBMP->x) < hXBitmap->w)
			{
				uint32_t color = pixels[(i - drawBorder->y + startDrawBMP->y) * hXBitmap->w +
										(j - drawBorder->x) + startDrawBMP->x];
				DrawAPixel(
					((color & 0xff000000) >> 8) |
						BGR888T0RGB565(color),
					j, i);
			}
			else
			{
				DrawPixel(hPencil->DrawBkColor, j, i);
			}
		}
	}
}

/*绘制二进制图片,宽和高只能是8的倍数*/
void DrawBitmapBinary(p_pencil_t hPencil, p_xrect_t drawBorder, p_xpoint_t startDrawPT, p_xbitmap_t hXBitmap)
{
	uint16_t i, j;
	uint16_t pixel;
	uint16_t draw_w;
	uint16_t draw_h;
	if (drawBorder == NULL || startDrawPT == NULL || hXBitmap == NULL)
	{
		return;
	}
	draw_w = MIN(drawBorder->w, hXBitmap->w);
	draw_h = MIN(drawBorder->h, hXBitmap->h);

	if (hXBitmap->bitsPerPixel == 1)
	{
		for (i = 0; i < draw_h; i++)
		{
			for (j = 0; j < draw_w; j++)
			{
				uint16_t temp = (i + startDrawPT->y) * hXBitmap->w +
								(j + startDrawPT->x); // 当前像素的位置
				pixel = hXBitmap->pixels[temp / 8] & ((1 << (7 - (temp % 8))));
				if (pixel)
				{
					DrawPixel(hPencil->DrawFrColor, j + drawBorder->x, i + drawBorder->y);
				}
				else
				{
					DrawPixel(hPencil->DrawBkColor, j + drawBorder->x, i + drawBorder->y);
				}
			}
		}
	}
}

/*在规定区域内绘制透明矩形*/
uint8_t DrawARect(p_pencil_t hPencil, p_xrect_t hXRECT)
{
	xrect_t rRect;
	if (!hPencil)
	{
		return (uint8_t)0;
	}
	GetOverLapRect(hXRECT, (p_xrect_t)hPencil, &rRect);

	RawDrawARect(&rRect, hPencil->DrawColor);
	return (uint8_t)1;
}

/*在规定区域内绘制矩形*/
uint8_t DrawRect(p_pencil_t hPencil, p_xrect_t hXRECT)
{
	xrect_t rRect;
	if (!hPencil)
	{
		return FALSE;
	}
	if (GetOverLapRect(hXRECT, (p_xrect_t)hPencil, &rRect) == FALSE)
	{
		return FALSE;
	}

#if USE_MEM_DEV
	RawDrawRect(&rRect, hPencil->DrawColor);
#else
	GUIDrawRect(rRect.x, rRect.y, rRect.w, rRect.h, hPencil->DrawColor);
#endif
	return TRUE;
}

/*
 * 在规定区域内绘制图片
 * hPencil 画布
 * border 绘图的边界
 * bgBorder 控件的边界
 * hXBitmap 绘制的图片
 */
uint8_t DrawBitmap(p_pencil_t hPencil, p_xrect_t border, p_xrect_t bgBorder, p_xbitmap_t hXBitmap)
{
	xrect_t rRect;	/*可以绘制的区域*/
	xrect_t rRect1; /*可以绘制的区域*/
	xrect_t temp;
	if (!hPencil)
	{
		return (uint8_t)0;
	}

	GetOverLapRect(border, (p_xrect_t)hPencil, &rRect);
	temp.x = bgBorder->x;
	temp.y = bgBorder->y;
	temp.w = hXBitmap->w;
	temp.h = hXBitmap->h;

	GetOverLapRect(&rRect, (p_xrect_t)&temp, &rRect1);

	xpoint_t xPoint;
	xPoint.x = rRect.x - bgBorder->x;
	xPoint.y = rRect.y - bgBorder->y;
	switch (hXBitmap->bitsPerPixel)
	{
	case 1: /*2位色*/
		DrawBitmapBinary(hPencil, &rRect1, &xPoint, hXBitmap);
		break;
	case 16: /*16位色*/
		if (hXBitmap->flag & BITMAP_DRAW_ARGB)
		{
			/*统一控制透明度*/
			RawDrawABitmap(
				hPencil, &rRect, &xPoint, hXBitmap);
		}
		else
		{
			RawDrawBitmap(
				hPencil, &rRect, &xPoint, hXBitmap);
		}
		break;
	case 32: /*32位色*/
		if (hXBitmap->flag & BITMAP_DRAW_ARGB8888)
		{
			/*单独通道控制透明度*/
			RawDrawARGBBitmap(hPencil, &rRect, &xPoint, hXBitmap);
		}
		break;
	}
	return TRUE;
}

/*
 * 在规定区域内绘制字符串
 */
uint8_t DrawString(p_pencil_t hPencil, const p_font_t hFont, p_xrect_t border, int16_t x, int16_t y, uint8_t *text)
{
	xrect_t rRect;
	uint16_t i;
	int16_t stx;
	int16_t sty;
	int16_t endy;
	int16_t endx;
	if (!hPencil)
	{
		return (uint8_t)0;
	}

	GetOverLapRect(border, (p_xrect_t)hPencil, &rRect);

	stx = MAX(rRect.x, x);
	sty = MAX(rRect.y, y);
	endy = MIN((hFont->fontInfo.h) + y, rRect.y + rRect.h);
	endx = MIN((int16_t)(GBK_Strlen(text) * (hFont->fontInfo.w)) + x, rRect.x + rRect.w);

	for (i = 0; text[i]; i++)
	{
		uint8_t *chData;
		chData = FontReadChar(hFont, text[i]);
		if (chData != NULL)
		{
			int16_t i, j;
			for (i = y; i < MIN(hFont->fontInfo.h + y, endy); i++)
			{
				if (i >= sty)
				{
					for (j = x; j < MIN(hFont->fontInfo.w + x, endx); j++)
					{
						if (j >= stx)
						{
							uint16_t tCHData = hFont->fontInfo.perRowBytes == 2 ? ((uint16_t *)chData)[i - y] : chData[i - y];
							if (_GET_BIT(tCHData, j - x))
							{
								DrawPixel(hPencil->DrawFrColor, j, i);
							}
							else
							{
								DrawPixel(hPencil->DrawBkColor, j, i);
							}
						}
					}
				}
			}
		}
		x += hFont->fontInfo.w;
	}
	return (uint8_t)1;
}

static uint8_t _DrawChar(p_pencil_t hPencil, p_xrect_t hXRectArea, p_font_t hFont, p_xpoint_t startPoint, uint8_t ch)
{
	uint8_t *chData;
	if (!hXRectArea)
	{
		return (uint8_t)0;
	}
	chData = FontReadChar(hFont, ch);
	if (chData != NULL)
	{
		uint16_t i, j;
		uint16_t pixel;
		uint16_t draw_w;
		uint16_t draw_h;
		if (hXRectArea == NULL || startPoint == NULL)
		{
			return 0;
		}
		draw_w = MIN(hXRectArea->w, hFont->fontInfo.w);
		draw_h = MIN(hXRectArea->h, hFont->fontInfo.h);

		for (i = 0; i < draw_h; i++)
		{
			for (j = 0; j < draw_w; j++)
			{
				uint16_t temp = (i + startPoint->y) * hFont->fontInfo.w +
								(j + startPoint->x); // 当前像素的位置
				pixel = chData[temp / 8] & ((1 << ((temp % 8))));
				if (pixel)
				{
					DrawPixel(hPencil->DrawFrColor, j + hXRectArea->x, i + hXRectArea->y);
				}
				else
				{
					DrawPixel(hPencil->DrawBkColor, j + hXRectArea->x, i + hXRectArea->y);
				}
			}
		}
	}
	return (uint8_t)1;
}

uint8_t _DrawCharEx(p_xrect_t bgRect, p_xrect_t drawRect, p_xpoint_t hStartXPoint, p_font_t hFont, char ch, p_pencil_t hPencil)
{
	uint16_t i, j;
	uint16_t pixel;
	uint8_t *chData;
	uint16_t draw_w;
	uint16_t draw_h;
	uint16_t sDrawX;
	uint16_t sDrawY;

	chData = FontReadChar(hFont, ch);
	if (chData != NULL)
	{

		sDrawX = bgRect->x + hStartXPoint->x;
		sDrawY = bgRect->y + hStartXPoint->y;

		draw_w = hFont->fontInfo.w; // MIN(drawRect->w, hFont->fontInfo.w);
		draw_h = hFont->fontInfo.h; // MIN(drawRect->h, hFont->fontInfo.h);

		for (i = drawRect->y; i < drawRect->y + drawRect->h; i++)
		{
			for (j = drawRect->x; j < drawRect->x + drawRect->w; j++)
			{
				if (j >= sDrawX && i >= sDrawY && j <= sDrawX + draw_w && i <= sDrawY + draw_h)
				{

					uint16_t temp = (i - sDrawY) * hFont->fontInfo.w +
									(j - sDrawX); // 当前像素的位置
					pixel = chData[temp / 8] & ((1 << ((temp % 8))));
					if (pixel)
					{
						DrawPixel(hPencil->DrawFrColor,
								  j, i);
					}
					else
					{
						DrawPixel(hPencil->DrawBkColor,
								  j, i);
					}
				}
				else
				{
					DrawPixel(hPencil->DrawBkColor,
							  j, i);
				}
			}
		}
	}

	return 1;
}
uint8_t _DrawStringEx(p_xrect_t bgRect, p_xrect_t drawRect, p_xpoint_t hStartXPoint, p_font_t hFont, const char *text, p_pencil_t hPencil)
{
	// int6 drawX=0;
	int16_t i, j;
	uint16_t pixel;
	uint8_t *chData = NULL;
	uint16_t draw_w;
	uint16_t draw_h;
	int16_t sDrawX;
	int16_t sDrawY;
	// uint16_t m;
	int16_t lastIndex = 0;
	int16_t lastIndex1 = 0;
	uint16_t ct = -1;
	sDrawX = bgRect->x + hStartXPoint->x;
	sDrawY = bgRect->y + hStartXPoint->y;
	if (hFont->fontType == 3)
	{ /*UNICODE*/
		draw_w = hFont->fontInfo.w * UNI_Strlen((uint16_t *)text);
	}
	else
	{ /*ASCII GB2312*/
		draw_w = hFont->fontInfo.w * GBK_Strlen(text);
	}
	draw_h = hFont->fontInfo.h;
	for (i = drawRect->y; i < drawRect->y + drawRect->h; i++)
	{
		lastIndex = 0;
		lastIndex1 = 0;
		for (j = drawRect->x; j < drawRect->x + drawRect->w; j++)
		{
			if (j >= sDrawX && i >= sDrawY && j < sDrawX + draw_w && i < sDrawY + draw_h)
			{
				uint16_t index = (j - sDrawX) / hFont->fontInfo.w; /*当前需要获取的字符索引ASCII字符*/
				uint16_t temp = (j - sDrawX) % hFont->fontInfo.w;
				uint16_t temp1 = ((i - sDrawY) % hFont->fontInfo.h) * hFont->fontInfo.perRowBytes;
				if (hFont->fontType == 3)
				{ // UNICODE
					uint16_t Wchar = (uint16_t)((((uint16_t)(text[index * hFont->fontInfo.perRowBytes + 1]) << 8) & 0xff00) | (text[index * hFont->fontInfo.perRowBytes] & 0xff));
					chData = FontReadChar(hFont, Wchar);
				}
				else
				{ /*ascii gb22312*/
					const char *wChar = GBK_CharGet(text, index);
					if (wChar == NULL)
					{
						continue;
					}
					if (wChar[0] & 0x80)
					{
						uint16_t Wchar = (uint16_t)(((uint16_t)(wChar[0]) << 8) | (wChar[1] & 0xff));
						chData = FontReadChar(hFont, Wchar);
					}
					else
					{
						chData = FontReadChar(hFont, wChar[0]);
					}
				}
				if (chData == NULL)
				{
					continue;
				}
				pixel = chData[temp1 + temp / 8] & ((1 << (temp % 8)));
				if (pixel)
				{
					DrawPixel(hPencil->DrawFrColor, j, i);
				}
				else
				{
					DrawPixel(hPencil->DrawBkColor, j, i);
				}
			}
			else
			{
				DrawPixel(hPencil->DrawBkColor,
						  j, i);
			}
		}
	}

	// Vsrefresh();
	return 1;
}
uint8_t DrawStringEx(p_xrect_t bgBorder, p_xrect_t drawBorder, p_xpoint_t hStartXPoint, p_font_t hFont, const char *text, p_pencil_t hPencil)
{
	xrect_t rRect;	/*可以绘制的区域*/
	xrect_t rRect1; /*可以绘制的区域*/
	xrect_t temp;
	if (!hPencil)
	{
		return (uint8_t)0;
	}
	GetOverLapRect(drawBorder, (p_xrect_t)hPencil, &rRect);

	_DrawStringEx(bgBorder, &rRect, hStartXPoint, hFont, text, hPencil);
	return TRUE;
}

uint8_t DrawCharEx(p_xrect_t bgBorder, p_xrect_t drawBorder, p_xpoint_t hStartXPoint, p_font_t hFont, char ch, p_pencil_t hPencil)
{
	xrect_t rRect;	/*可以绘制的区域*/
	xrect_t rRect1; /*可以绘制的区域*/
	xrect_t temp;
	if (!hPencil)
	{
		return (uint8_t)0;
	}
	GetOverLapRect(drawBorder, (p_xrect_t)hPencil, &rRect);

	_DrawCharEx(bgBorder, &rRect, hStartXPoint, hFont, ch, hPencil);
	return TRUE;
}

uint8_t DrawChar(p_pencil_t hPencil, const p_font_t hFont, p_xrect_t border, p_xrect_t bgBorder, char ch)
{
	xrect_t rRect;	/*可以绘制的区域*/
	xrect_t rRect1; /*可以绘制的区域*/
	xrect_t temp;
	if (!hPencil)
	{
		return (uint8_t)0;
	}
	GetOverLapRect(border, (p_xrect_t)hPencil, &rRect);
	temp.x = bgBorder->x;
	temp.y = bgBorder->y;
	temp.w = hFont->fontInfo.w;
	temp.h = hFont->fontInfo.h;

	GetOverLapRect(&rRect, (p_xrect_t)&temp, &rRect1);

	xpoint_t xPoint;
	xPoint.x = rRect.x - bgBorder->x;
	xPoint.y = rRect.y - bgBorder->y;

	_DrawChar(hPencil, &rRect1, hFont, &xPoint, ch);
	return TRUE;
}

/*字绘制*/
uint8_t DrawCutChar(void *hObject, const p_font_t hFont, p_xrect_t bgRect, p_xpoint_t hStartXPoint, char ch)
{
	p_widget_base_t hWidgeBase = hObject;
	if (!hWidgeBase)
	{
		return (uint8_t)FALSE;
	}
	p_xrect_t nextCutRect = NULL;
	RECT_CUT_FOREACH(nextCutRect, bgRect)
	{
		DrawCharEx(bgRect, nextCutRect, hStartXPoint, hFont, ch, &(hWidgeBase->pencil));
	}
	return (int8_t)TRUE;
}

/*字符串剪裁绘制*/
uint8_t DrawCutString(void *hObject, const p_font_t hFont, p_xrect_t border, p_xpoint_t hXPoint, const char *text)
{
	xpoint_t startPoint;
	p_widget_base_t hWidgeBase = hObject;
	if (!hWidgeBase)
	{
		return (uint8_t)FALSE;
	}

	startPoint.x = 0;
	startPoint.y = 0;
	p_xrect_t nextCutRect = NULL;

	RECT_CUT_FOREACH(nextCutRect, border)
	{
		DrawStringEx(border, nextCutRect, hXPoint == 0 ? &startPoint : hXPoint, hFont, text, &(hWidgeBase->pencil));
	}
	return (int8_t)TRUE;
}

/*图片剪裁绘制*/
uint8_t DrawCutBitmap(void *hObject, p_xrect_t border, p_xbitmap_t hXBitmap)
{
	p_xrect_t nextCutRect = NULL;
	p_widget_base_t hWidgeBase = hObject;
	if (!hWidgeBase || !border || !hXBitmap)
	{
		return (uint8_t)0;
	}
	/*循环绘制剪裁后的矩形*/
	RECT_CUT_FOREACH(nextCutRect, border)
	{
		DrawBitmap(&(hWidgeBase->pencil), nextCutRect, border, hXBitmap);
	}

	return (uint8_t)1;
}
/*矩形剪裁绘制*/
uint8_t DrawCutRect(void *hObject, p_xrect_t hXRECT)
{
	p_xrect_t nextCutRect = NULL;
	p_widget_base_t hWidgeBase = hObject;
	if (!hWidgeBase)
	{
		return (uint8_t)0;
	}
	/*循环绘制剪裁后的矩形*/
	RECT_CUT_FOREACH(nextCutRect, hXRECT)
	{
		if (_GET_IS_DPY(hWidgeBase))
		{
			/*透明矩形*/
			DrawARect(&(hWidgeBase->pencil), nextCutRect);
		}
		else
		{
			DrawRect(&(hWidgeBase->pencil), nextCutRect);
		}
	}

	return (uint8_t)1;
}
#if 0
static void DrawCicPaint(void *obj, xpoint_t *cen_p, xpoint_t *d_p, int line_width, uintColor color)
{
	widget_base_t *hWidgeBase = obj;

	line_width = 10;
	// 利用圆的八分对称性画点
#define ARC_X d_p->x
#define ARC_Y d_p->y
	{
		int cen_x = cen_p->x;
		int cen_y = cen_p->y;

		float k = (float)(ARC_Y - cen_y) / (float)(ARC_X - cen_x);
		float b = ARC_Y - ARC_X * k;
		float A = 1 + k * k;
		float B = 2 * k * (b - ARC_Y) - 2 * ARC_X;
		float C = (b - ARC_Y) * (b - ARC_Y) - line_width * line_width + ARC_X * ARC_X;

		float x_z = (-B - sqrt(B * B - 4 * A * C)) / (2 * A);
		float y_z = k * x_z + b;
		float x_f = (-B + sqrt(B * B - 4 * A * C)) / (2 * A);
		float y_f = k * x_f + b;

		DrawLine(obj, &(xpoint_t){.x = x_z, .y = y_z}, &(xpoint_t){.x = x_f, .y = y_f});
	}
#undef ARC_X
#undef ARC_Y
}
#endif
void DrawCircle(void *obj, xpoint_t *center, int radius)
{
	widget_base_t *hWidgeBase = obj;

	int x = hWidgeBase->rect.x + center->x;
	int y = hWidgeBase->rect.y + center->y;
	int r = radius;
	int tx = 0, ty = r, d = 1.25 - r;
	uintColor color = hWidgeBase->pencil.DrawColor;

	while (tx <= ty)
	{
		DrawPixelX(color, x + tx, y + ty, hWidgeBase->rect.y + hWidgeBase->rect.h);
		DrawPixelX(color, x + tx, y - ty, hWidgeBase->rect.y + hWidgeBase->rect.h);
		DrawPixelX(color, x - tx, y + ty, hWidgeBase->rect.y + hWidgeBase->rect.h);
		DrawPixelX(color, x - tx, y - ty, hWidgeBase->rect.y + hWidgeBase->rect.h);
		DrawPixelX(color, x + ty, y + tx, hWidgeBase->rect.y + hWidgeBase->rect.h);
		DrawPixelX(color, x + ty, y - tx, hWidgeBase->rect.y + hWidgeBase->rect.h);
		DrawPixelX(color, x - ty, y + tx, hWidgeBase->rect.y + hWidgeBase->rect.h);
		DrawPixelX(color, x - ty, y - tx, hWidgeBase->rect.y + hWidgeBase->rect.h);
		if (d < 0)
			d += 2 * tx + 3;
		else
			d += 2 * (tx - ty) + 5, ty--;

		tx++;
	}
}
#define cacl_d_pos(angle) ((angle) / 90)
#define cacl_xl(x0, y0, x1, y1) (((float)(y1) - (float)(y0)) / ((float)(x1) - (float)(x0)))
static inline calc_d_pos_point(int x, int y, xpoint_t *cp)
{
	if (x > cp->x && y > cp->y)
	{
		return 0;
	}
	if (x > cp->x && y < cp->y)
	{
		return 1;
	}
	if (x < cp->x && y < cp->y)
	{
		return 2;
	}
	if (x < cp->x && y > cp->y)
	{
		return 3;
	}
}
void DrawPie2(void *obj, xpoint_t *center, int r, int st_angle, int end_angle)
{
	widget_base_t *hWidgeBase = obj;

	int x = hWidgeBase->rect.x + center->x;
	int y = hWidgeBase->rect.y + center->y;
	float stk = tan(st_angle);
	int st_d = cacl_d_pos(st_angle - 1);

	float endk = tan(end_angle);
	int end_d = cacl_d_pos(end_angle - 1);

	for (int py = y - r; py <= y + r; ++py)
	{
		for (int px = x - r; px <= x + r; ++px)
		{
			float k = cacl_xl(center->x, center->y, px, py); // 当前点的斜率
			int p_d = calc_d_pos_point(px, py, center);		 // 当前点的象限

			if (!(p_d >= st_d && p_d <= end_d))
			{
				continue;
			}
			if (p_d == st_d)
			{
				switch (st_d)
				{
				case 0:
					if (k >= stk && k <= endk)
					{
						DrawPixelX(hWidgeBase->pencil.DrawColor, px, py, hWidgeBase->rect.y + hWidgeBase->rect.h);
					}
					break;
				case 1:
					if (k <= stk)
					{
						switch (end_d)
						{
						case 0:
							if (k >= endk)
							{
								DrawPixelX(hWidgeBase->pencil.DrawColor, px, py, hWidgeBase->rect.y + hWidgeBase->rect.h);
							}
							break;
						case 2:
						case 3:
							if (k >= endk)
							{
								DrawPixelX(hWidgeBase->pencil.DrawColor, px, py, hWidgeBase->rect.y + hWidgeBase->rect.h);
							}
							break;
						}
					}
					break;
				case 2:
					if (k <= stk)
					{
						switch (end_d)
						{
						case 0:
							if (k >= endk)
							{
								DrawPixelX(hWidgeBase->pencil.DrawColor, px, py, hWidgeBase->rect.y + hWidgeBase->rect.h);
							}
							break;
						case 3:
							if (k >= endk)
							{
								DrawPixelX(hWidgeBase->pencil.DrawColor, px, py, hWidgeBase->rect.y + hWidgeBase->rect.h);
							}
							break;
						}
					}
					break;
				case 3:
					if (k <= stk)
					{
						switch (end_d)
						{
						case 0:
							if (k >= endk)
							{
								DrawPixelX(hWidgeBase->pencil.DrawColor, px, py, hWidgeBase->rect.y + hWidgeBase->rect.h);
							}
							break;
						}
					}
					break;
				}
				continue;
			}
			if (p_d == end_d)
			{
				continue;
			}
			DrawPixelX(hWidgeBase->pencil.DrawColor, px, py, hWidgeBase->rect.y + hWidgeBase->rect.h);
		}
	}
}
void DrawLine(void *obj, xpoint_t *st_point, xpoint_t *end_point)
{
	int x0;
	int y0;
	int x1;
	int y1;

	widget_base_t *hWidgeBase = obj;

	assert(hWidgeBase);
	x0 = st_point->x + (hWidgeBase->rect.x);
	y0 = st_point->y + (hWidgeBase->rect.y);
	x1 = end_point->x + (hWidgeBase->rect.x);
	y1 = end_point->y + (hWidgeBase->rect.y);

	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2; /* error value e_xy */
	int j = 0;

	for (;;)
	{ /* loop */
		if (hWidgeBase->pencil.lineType & (1 << (j % 8)))
		{
			if (j == 7) // 线型循环条件
				j = 0;
			if (dy <= dx) // 斜率<1,刷子与X轴垂直,X+-(i/2)
			{
				for (int i = 0; i < hWidgeBase->pencil.LineWidth; i++)
				{
					DrawPixelX(hWidgeBase->pencil.DrawColor, (int)(x0 + i / 2), (int)(y0),
							   hWidgeBase->rect.y + hWidgeBase->rect.h);
					DrawPixelX(hWidgeBase->pencil.DrawColor, (int)(x0 - i / 2), (int)(y0),
							   hWidgeBase->rect.y + hWidgeBase->rect.h);
				}
			}
			else // 斜率>1,刷子与X轴平行,Y+-(i/2)
			{
				for (int i = 0; i < hWidgeBase->pencil.LineWidth; i++)
				{
					DrawPixelX(hWidgeBase->pencil.DrawColor, (int)(x0), (int)(y0 + i / 2),
							   hWidgeBase->rect.y + hWidgeBase->rect.h);
					DrawPixelX(hWidgeBase->pencil.DrawColor, (int)(x0), (int)(y0 - i / 2),
							   hWidgeBase->rect.y + hWidgeBase->rect.h);
				}
			}
		}
		j++;
		if (x0 == x1 && y0 == y1)
			break;
		e2 = 2 * err;
		if (e2 >= dy)
		{
			err += dy;
			x0 += sx;
		} /* e_xy+e_x > 0 */
		if (e2 <= dx)
		{
			err += dx;
			y0 += sy;
		} /* e_xy+e_y < 0 */
	}
}

// 说明：d代表移动到第几象限，r是半径，a1a2分别是起点和终点且0<=a1<a2<=90
static void _DrawArc(widget_base_t *hWidgeBase, xpoint_t *cen, int d, int r, int a1, int a2, uintColor color)
{
	// 计算起始点坐标
	int y = r * x_sin((double)a1 * M_PI / 180.0), x = r * x_cos((double)a1 * M_PI / 180.0); //
	int limit_x = r * x_cos((double)a2 * M_PI / 180.0);										// 终点的x坐标
	int delta_i = x * x + y * y - r * r;
	int flag; // 0代表左点，1代表左上点，2代表上点
	while (x >= limit_x)
	{
		flag = 1;
		switch (hWidgeBase->pencil.brush_type)
		{
		case NORMAL_TYPE:
			/* code */
			switch (d)
			{
			case 1:
#define ARC_X (x + cen->x)
#define ARC_Y (cen->y - y)
#define XL_VAL ((float)(cen->x - ARC_X) / ((float)(ARC_Y - cen->y)))
				if (XL_VAL >= -1 && XL_VAL <= 1) // 斜率<1,刷子与X轴垂直,X+-(i/2)
				{
					for (int i = 0; i < hWidgeBase->pencil.LineWidth; i++)
					{
						DrawPixelX(color, (int)(ARC_X + 0.5), (int)(ARC_Y + 0.5 + i / 2),
								   hWidgeBase->rect.y + hWidgeBase->rect.h);
						DrawPixelX(color, (int)(ARC_X + 0.5), (int)(ARC_Y + 0.5 - i / 2),
								   hWidgeBase->rect.y + hWidgeBase->rect.h);
					}
				}
				else // 斜率>1,刷子与X轴平行,Y+-(i/2)
				{

					for (int i = 0; i < hWidgeBase->pencil.LineWidth; i++)
					{
						DrawPixelX(color, (int)(ARC_X + 0.5 + i / 2), (int)(ARC_Y + 0.5),
								   hWidgeBase->rect.y + hWidgeBase->rect.h);
						DrawPixelX(color, (int)(ARC_X + 0.5 - i / 2), (int)(ARC_Y + 0.5),
								   hWidgeBase->rect.y + hWidgeBase->rect.h);
					}
				}
#undef ARC_X
#undef ARC_Y
				// DrawPixelX(color, x + cen->x, cen->y - y,
				// 		   hWidgeBase->rect.y + hWidgeBase->rect.h);
				break;
			case 2:
#define ARC_X (-y + cen->x)
#define ARC_Y (cen->y - x)
				if (XL_VAL >= -1 && XL_VAL <= 1) // 斜率<1,刷子与X轴垂直,X+-(i/2)
				{

					for (int i = 0; i < hWidgeBase->pencil.LineWidth; i++)
					{
						DrawPixelX(color, (int)(ARC_X + 0.5), (int)(ARC_Y + 0.5 + i / 2),
								   hWidgeBase->rect.y + hWidgeBase->rect.h);
						DrawPixelX(color, (int)(ARC_X + 0.5), (int)(ARC_Y + 0.5 - i / 2),
								   hWidgeBase->rect.y + hWidgeBase->rect.h);
					}
				}
				else // 斜率>1,刷子与X轴平行,Y+-(i/2)
				{
					for (int i = 0; i < hWidgeBase->pencil.LineWidth; i++)
					{
						DrawPixelX(color, (int)(ARC_X + 0.5 + i / 2), (int)(ARC_Y + 0.5),
								   hWidgeBase->rect.y + hWidgeBase->rect.h);
						DrawPixelX(color, (int)(ARC_X + 0.5 - i / 2), (int)(ARC_Y + 0.5),
								   hWidgeBase->rect.y + hWidgeBase->rect.h);
					}
				}
#undef ARC_X
#undef ARC_Y
				break;
			case 3:
#define ARC_X (-x + cen->x)
#define ARC_Y (cen->y + y)
				if (XL_VAL >= -1 && XL_VAL <= 1) // 斜率<1,刷子与X轴垂直,X+-(i/2)
				{
					for (int i = 0; i < hWidgeBase->pencil.LineWidth; i++)
					{
						DrawPixelX(color, (int)(ARC_X + 0.5), (int)(ARC_Y + 0.5 + i / 2),
								   hWidgeBase->rect.y + hWidgeBase->rect.h);
						DrawPixelX(color, (int)(ARC_X + 0.5), (int)(ARC_Y + 0.5 - i / 2),
								   hWidgeBase->rect.y + hWidgeBase->rect.h);
					}
				}
				else // 斜率>1,刷子与X轴平行,Y+-(i/2)
				{

					for (int i = 0; i < hWidgeBase->pencil.LineWidth; i++)
					{
						DrawPixelX(color, (int)(ARC_X + 0.5 + i / 2), (int)(ARC_Y + 0.5),
								   hWidgeBase->rect.y + hWidgeBase->rect.h);
						DrawPixelX(color, (int)(ARC_X + 0.5 - i / 2), (int)(ARC_Y + 0.5),
								   hWidgeBase->rect.y + hWidgeBase->rect.h);
					}
				}
#undef ARC_X
#undef ARC_Y

				break;
			case 4:
#define ARC_X (y + cen->x)
#define ARC_Y (cen->y + x)
				if (XL_VAL >= -1 && XL_VAL <= 1) // 斜率<1,刷子与X轴垂直,X+-(i/2)
				{

					for (int i = 0; i < hWidgeBase->pencil.LineWidth; i++)
					{
						DrawPixelX(color, (int)(ARC_X + 0.5), (int)(ARC_Y + 0.5 + i / 2),
								   hWidgeBase->rect.y + hWidgeBase->rect.h);
						DrawPixelX(color, (int)(ARC_X + 0.5), (int)(ARC_Y + 0.5 - i / 2),
								   hWidgeBase->rect.y + hWidgeBase->rect.h);
					}
				}
				else // 斜率>1,刷子与X轴平行,Y+-(i/2)
				{
					for (int i = 0; i < hWidgeBase->pencil.LineWidth; i++)
					{
						DrawPixelX(color, (int)(ARC_X + 0.5 + i / 2), (int)(ARC_Y + 0.5),
								   hWidgeBase->rect.y + hWidgeBase->rect.h);
						DrawPixelX(color, (int)(ARC_X + 0.5 - i / 2), (int)(ARC_Y + 0.5),
								   hWidgeBase->rect.y + hWidgeBase->rect.h);
					}
				}
#undef ARC_X
#undef ARC_Y
				break;
			default:
				break;
			}
			break;
		case CIRCLE_TYPE:
			switch (d)
			{
			case 1:
				DrawFillCircle(hWidgeBase, &(xpoint_t){x + cen->x - hWidgeBase->rect.x, cen->y - y - hWidgeBase->rect.y},
							   hWidgeBase->pencil.LineWidth);
				break;
			case 2:
				DrawFillCircle(hWidgeBase, &(xpoint_t){
											   -y + cen->x - hWidgeBase->rect.x,
											   cen->y - x - hWidgeBase->rect.y,
										   },
							   hWidgeBase->pencil.LineWidth);
				// DrawPixelX(color, -y + cen->x, cen->y - x,
				// 		   hWidgeBase->rect.y + hWidgeBase->rect.h);
				break;
			case 3:
				DrawFillCircle(hWidgeBase, &(xpoint_t){
											   -x + cen->x - hWidgeBase->rect.x,
											   cen->y + y - hWidgeBase->rect.y,
										   },
							   hWidgeBase->pencil.LineWidth);
				// DrawPixelX(color, -x + cen->x, cen->y + y,
				// 		   hWidgeBase->rect.y + hWidgeBase->rect.h);
				break;
			case 4:
				DrawFillCircle(hWidgeBase, &(xpoint_t){
											   y + cen->x - hWidgeBase->rect.x,
											   cen->y + x - hWidgeBase->rect.y,
										   },
							   hWidgeBase->pencil.LineWidth);
				// DrawPixelX(color, y + cen->x, cen->y + x,
				// 		   hWidgeBase->rect.y + hWidgeBase->rect.h);
				break;
			}
			break;
		case RECT_TYPE:
			switch (d)
			{
			case 1:
#define ARC_X (x + cen->x)
#define ARC_Y (cen->y - y)

				DrawSquare(hWidgeBase, &(xpoint_t){x + cen->x - hWidgeBase->rect.x, cen->y - y - hWidgeBase->rect.y},
						   hWidgeBase->pencil.LineWidth);
#undef ARC_X
#undef ARC_Y
				break;
			case 2:
				DrawSquare(hWidgeBase, &(xpoint_t){
										   -y + cen->x - hWidgeBase->rect.x,
										   cen->y - x - hWidgeBase->rect.y,
									   },
						   hWidgeBase->pencil.LineWidth);
				// DrawPixelX(color, -y + cen->x, cen->y - x,
				// 		   hWidgeBase->rect.y + hWidgeBase->rect.h);
				break;
			case 3:
				DrawSquare(hWidgeBase, &(xpoint_t){
										   -x + cen->x - hWidgeBase->rect.x,
										   cen->y + y - hWidgeBase->rect.y,
									   },
						   hWidgeBase->pencil.LineWidth);
				// DrawPixelX(color, -x + cen->x, cen->y + y,
				// 		   hWidgeBase->rect.y + hWidgeBase->rect.h);
				break;
			case 4:
				DrawSquare(hWidgeBase, &(xpoint_t){
										   y + cen->x - hWidgeBase->rect.x,
										   cen->y + x - hWidgeBase->rect.y,
									   },
						   hWidgeBase->pencil.LineWidth);
				// DrawPixelX(color, y + cen->x, cen->y + x,
				// 		   hWidgeBase->rect.y + hWidgeBase->rect.h);
				break;
			}
			break;
		default:
			break;
		}

		if (delta_i > 0)
		{
			if (2 * delta_i - 2 * y - 1 > 0)
			{
				flag = 0;
			}
		}
		else
		{
			if (2 * delta_i + 2 * x - 1 < 0)
			{
				flag = 2;
			}
		}
		switch (flag)
		{
		case (0):
			delta_i += 3 - 2 * x;
			x--;
			break;
		case (1):
			delta_i += 2 * y - 2 * x + 6;
			x--;
			y++;
			break;
		case (2):
			delta_i += 2 * y + 3;
			y++;
			break;
		}
	}
}
void DrawSquare(void *obj, xpoint_t *st_p, int w)
{
	widget_base_t *hWidgeBase = obj;
	assert(hWidgeBase);
	st_p->x += ((xrect_t *)hWidgeBase)->x;
	st_p->y += ((xrect_t *)hWidgeBase)->y;

	for (int i = st_p->y; i < st_p->y + w; i++)
	{
		for (int j = st_p->x; j < st_p->x + w; j++)
		{
			DrawPixelX(hWidgeBase->pencil.DrawColor, j, i,
					   hWidgeBase->rect.y + hWidgeBase->rect.h);
		}
	}
}
void DrawArc(void *obj, xpoint_t *cen, float alpha1, float alpha2, float R)
{
	uintColor color;
	widget_base_t *hWidgeBase = obj;
	assert(hWidgeBase);

	color = hWidgeBase->pencil.DrawColor;
	cen->x += ((xrect_t *)hWidgeBase)->x;
	cen->y += ((xrect_t *)hWidgeBase)->y;
	// 将输入值整数化，方便计算
	int r = (int)(R + 0.5);
	int A1 = (int)(alpha1 + 0.5);
	int A2 = (int)(alpha2 + 0.5);
	if (A1 == A2)
	{
		return;
	}
	// a1<a2恒成立
	int a1 = A1 < A2 ? A1 : A2;
	int a2 = A1 > A2 ? A1 : A2;

	int d1 = a1 / 90 + 1;							 // 判断a1的象限，包括坐标轴
	int d2 = (a2 % 90) == 0 ? a2 / 90 : a2 / 90 + 1; // 判断a2的象限，包括坐标轴

	int delta = d2 - d1;
	switch (delta)
	{
	case 0: // 起点终点在相同象限，一步画
		_DrawArc(hWidgeBase, cen, d1, r, a1 - ((d1 - 1) * 90), a2 - ((d2 - 1) * 90), color);
		break;

	case 1: // 起点终点在相邻象限，两步画
		_DrawArc(hWidgeBase, cen, d1, r, (a1 - ((d1 - 1) * 90)), (d1 * 90), color);
		_DrawArc(hWidgeBase, cen, d2, r, 0, a2 - ((d2 - 1) * 90), color);
		break;
	default: // 起点终点在相隔不小于一个象限的象限，多步画
		_DrawArc(hWidgeBase, cen, d1, r, (a1 - ((d1 - 1) * 90)), (d1 * 90), color);
		for (int i = d1 + 1; i < d2; i++)
		{
			_DrawArc(hWidgeBase, cen, i, r, 0, 90, color);
		}
		_DrawArc(hWidgeBase, cen, d2, r, 0, a2 - ((d2 - 1) * 90), color);
		break;
	}
}
#define Check_Circle(px, py, x, y, r) ((((px) - (x)) * ((px) - (x)) + ((py) - (y)) * ((py) - (y))) <= (r) * (r) ? 1 : 0) // 判断该点是否在该圆中
void DrawPie(void *obj,
			 xpoint_t *cen,
			 int r, // 扇形半径
			 float start_angle,
			 float end_angle // 扇形弧度
)
{
	int x;
	int y; // 扇形中心坐标
	int px, py;
	float istangle;
	widget_base_t *hWidgeBase = obj;
	assert(hWidgeBase);
	BOOL X = 1;		   // 扇形是否从x轴开始
	BOOL positive = 1; // 扇形是否从正半轴开始
	BOOL R = 1;		   // 扇形是否从顺时针开始
	uintColor color = hWidgeBase->pencil.DrawColor;

	x = cen->x + hWidgeBase->rect.x;
	y = cen->y + hWidgeBase->rect.y;
	// 输入纠正
	if (end_angle < 0)
	{
		end_angle = -end_angle;
		R = !R;
	}
	if (end_angle > 6.28)
		end_angle = 6.28;
	// int st_x = x + r * cos(M_PI / 180.0 * 45.0);
	// int st_y = y + r * sin(M_PI / 180.0 * 45.0);
	// 开始输出
	for (py = y - r; py <= y + r; ++py)
	{
		for (px = x - r; px <= x + r; ++px)
		{
			if (Check_Circle(px, py, x, y, r) && !Check_Circle(px, py, x, y, r - 10))
			{				 // 如果该点落在圆中
				if (py == y) // 如果点在x轴上
					istangle = px >= x ? 0 : 3.14;
				else if (px == x) // 如果点在y轴上
					istangle = py > y ? 1.57 : 3.14 + 1.57;
				else
				{
					istangle = x_atan(((double)py - y), ((double)px - x));
					if (px < x)
						istangle += 3.14;
					else if (py < y)
						istangle = 6.28 + istangle;
				}
				if (istangle >= start_angle && istangle < end_angle)
				{
					if (X)
						if (positive)
							if (R)
								DrawPixelX(color, px, py, hWidgeBase->rect.y + hWidgeBase->rect.h);
							else
								DrawPixelX(color, px, (y - py + y), hWidgeBase->rect.y + hWidgeBase->rect.h);
						else if (R)
							DrawPixelX(color, (x - px + x), (y - py + y), hWidgeBase->rect.y + hWidgeBase->rect.h);
						else
							DrawPixelX(color, (x - px + x), py, hWidgeBase->rect.y + hWidgeBase->rect.h);
					else
					{
						if (positive)
							if (R)
								DrawPixelX(color, py - y + x, y - px + x, hWidgeBase->rect.y + hWidgeBase->rect.h);
							else
								DrawPixelX(color, x - py + y, y - px + x, hWidgeBase->rect.y + hWidgeBase->rect.h);
						else if (R)
							DrawPixelX(color, x - py + y, px - x + y, hWidgeBase->rect.y + hWidgeBase->rect.h);
						else
							DrawPixelX(color, x + py - y, y + px - x, hWidgeBase->rect.y + hWidgeBase->rect.h);
					}
				}
			}
		}
	}
}

void DrawFillCircle(void *obj, xpoint_t *cen, int r)
{
	uintColor color;
	int a, b;
	int di;
	int x0, y0;
	widget_base_t *hWidgeBase = obj;
	assert(hWidgeBase);

	color = hWidgeBase->pencil.DrawColor;
	x0 = cen->x + hWidgeBase->rect.x;
	y0 = cen->y + hWidgeBase->rect.y;

	a = 0;
	b = r;
	di = 3 - (r << 1); // 判断下个点位置的标志
	while (a <= b)
	{
		int i = a, p = b;
		while (i > 0)
		{
			DrawPixelX(color, x0 + b, y0 - i, hWidgeBase->rect.y + hWidgeBase->rect.h);
			DrawPixelX(color, x0 - i, y0 + b, hWidgeBase->rect.y + hWidgeBase->rect.h);
			i--;
		}
		while (p > 0)
		{
			DrawPixelX(color, x0 - a, y0 - p, hWidgeBase->rect.y + hWidgeBase->rect.h);
			DrawPixelX(color, x0 - p, y0 - a, hWidgeBase->rect.y + hWidgeBase->rect.h);
			DrawPixelX(color, x0 + a, y0 - p, hWidgeBase->rect.y + hWidgeBase->rect.h);
			DrawPixelX(color, x0 - p, y0 + a, hWidgeBase->rect.y + hWidgeBase->rect.h);
			DrawPixelX(color, x0 + a, y0 + p, hWidgeBase->rect.y + hWidgeBase->rect.h);
			DrawPixelX(color, x0 + p, y0 + a, hWidgeBase->rect.y + hWidgeBase->rect.h);
			p--;
		}
		a++;
		// Bresenham算法画圆
		if (di < 0)
			di += 4 * a + 6;
		else
		{
			di += 10 + 4 * (a - b);
			b--;
		}
	}
	DrawPixelX(color, x0, y0, hWidgeBase->rect.y + hWidgeBase->rect.h); // 圆心坐标
}
