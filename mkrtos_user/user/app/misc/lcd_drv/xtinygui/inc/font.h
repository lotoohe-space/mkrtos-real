/**
 * @file font.h
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

typedef struct
{
	uint16_t w;			// 字符的宽
	uint16_t h;			// 字符的高
	uint16_t startInx;	// 开始位置
	uint16_t endInx;		// 结束位置
	uint16_t perRowBytes; // 一个字符的一行占多少字节
	union
	{
		const uint8_t *addrFont; // 字体的地址
		uint32_t fd;			   // 文件句柄
	} fontVal;
} *p_font_info_t, font_info_t;

#define ASCII_TYPE 0
#define GB2312_TYPE 1
#define GBK_TYPE 2
#define UNICODE_TYPE 3

typedef struct
{
	font_info_t fontInfo;
	p_font_info_t additionFont;
	uint8_t typePos;	// 0:字体在内存中 1:字体在外部存储设备
	uint8_t fontType; // 字库类型 0:ascii 1:gb2312 2:gbk(未实现) 3:unicode编码
} *p_font_t, font_t;
////计算一行占多少个字节
// #define FONT_CHAR_PER_BTYES(a) ((uint16_t)(a->fontInfo.w/8.0f+1))

extern const font_t fontASCII8_12;
extern const font_t fontASCII12_16;
extern const font_t fontASCII16_16;
extern const font_t fontASCII100_100;
extern const font_t fontGB231216_16;
extern const font_t fontUNICODE16_16;

uint8_t *FontReadChar(p_font_t hFont, uint16_t ch);
