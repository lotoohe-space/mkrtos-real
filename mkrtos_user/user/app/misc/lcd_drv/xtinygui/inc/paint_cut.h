/**
 * @file paint_cut.h
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
#include "gui.h"

p_list_t RectCutAddRectList(p_list_t addRectList);
void RectCutSplitRectList(p_list_t splitRectList);

int8_t RectCutInit(void);
void *RectCutStart(p_xrect_t hXRECT);
p_xrect_t RectCutFind(void);
void RectCutEnd(void);
BOOL RectCutIsEnd(void);

#define RECT_CUT_FOREACH(pos, a) \
	for (RectCutStart(a); !RectCutIsEnd() && ((pos = RectCutFind()) != NULL);)
