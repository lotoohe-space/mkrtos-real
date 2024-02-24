/**
 * @file bitmap.h
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

/*Bitmap*/
typedef struct
{
	uint16_t w;
	uint16_t h;
	const uint8_t *pixels;	  /*像素值*/
	uint8_t alpha;		  /*当flag:0使能时，透明通道值有效*/
	uint8_t bitsPerPixel; /*一个像素多少位 目前支持RGBR565 BIN */
	uint8_t flag;		  /*0:bit 是否应用全局透明通道 1:bit 是否应用图片透明通道*/
} *p_xbitmap_t, xbitmap_t;

#define BITMAP_DRAW_ARGB 0x1	 /*使用单个alpha控制整个图片的透明度*/
#define BITMAP_DRAW_ARGB8888 0x2 /*使用单独的alpha通道，通道在pixels中*/

uint8_t InitBitmap(p_xbitmap_t hXBitmap, const uint8_t *pixels, uint16_t w, uint16_t h, uint8_t colorDepth);
void BitmapSetFlag(p_xbitmap_t hXBitmap, uint8_t flag);

#define ________ 0X00
#define _______X 0x01
#define ______X_ 0x02
#define _____X__ 0x04
#define ____X___ 0x08
#define ___X____ 0x10
#define __X_____ 0x20
#define _X______ 0x40
#define X_______ 0x80

#define ______XX _______X | ______X_
#define _____XX_ _____X__ | ______X_
#define ____XX__ _____X__ | ____X___
#define ___XX___ ___X____ | ____X___
#define __XX____ __X_____ | ___X____
#define _XX_____ _X______ | __X_____
#define XX______ X_______ | _X______

#define _____XXX ______XX | _____X__
#define ____XXXX _____XXX | ____X___
#define ___XXXXX ____XXXX | ___X____
#define __XXXXXX ___XXXXX | __X_____
#define _XXXXXXX __XXXXXX | _X______
#define XXXXXXXX _XXXXXXX | X_______

#define XXX_____ XX______ | __X_____
#define XXXX____ XXX_____ | ___X____
#define XXXXX___ XXXX____ | ____X___
#define XXXXXX__ XXXXX___ | _____X__
#define XXXXXXX_ XXXXXX__ | ______X_
// #define XXXXXXXX XXXXXXX_|_______X

#define ________________ 0X0000
#define _______________X 0x0001
#define ______________X_ 0x0002
#define _____________X__ 0x0004
#define ____________X___ 0x0008
#define ___________X____ 0x0010
#define __________X_____ 0x0020
#define _________X______ 0x0040
#define ________X_______ 0x0080
#define _______X________ 0x0100
#define ______X_________ 0x0200
#define _____X__________ 0x0400
#define ____X___________ 0x0800
#define ___X____________ 0x1000
#define __X_____________ 0x2000
#define _X______________ 0x4000
#define X_______________ 0x8000

#define __X_______X_____ (___________X____ | __X_____________)
#define ___X_____X______ (__________X_____ | ___X____________)
#define ____X___X_______ (_________X______ | ____X___________)
