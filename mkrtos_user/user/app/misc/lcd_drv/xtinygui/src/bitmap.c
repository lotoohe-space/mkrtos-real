/**
 * @file bitmap.c
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "bitmap.h"

uint8_t InitBitmap(p_xbitmap_t hXBitmap, const uint8_t *pixels,
				   uint16_t w, uint16_t h, uint8_t colorDepth)
{
	if (hXBitmap == NULL)
	{
		return FALSE;
	}

	hXBitmap->w = w;
	hXBitmap->h = h;
	hXBitmap->pixels = pixels;
	hXBitmap->bitsPerPixel = colorDepth;
	hXBitmap->flag = 0;
	hXBitmap->alpha = 255;
	return TRUE;
}
void BitmapSetFlag(p_xbitmap_t hXBitmap, uint8_t flag)
{
	if (hXBitmap == NULL)
	{
		return;
	}
	hXBitmap->flag = flag;
}
uint8_t InitBitmapFromFile(const char *file)
{

	return TRUE;
}
