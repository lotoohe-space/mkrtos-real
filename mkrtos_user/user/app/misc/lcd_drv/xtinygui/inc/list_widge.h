/**
 * @file list_widge.h
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include "group_widge.h"
#include "GUI_timeout.h"

#define ListWidget(a) ListWidget##a

#define LIST_WIDGE_ITEM_SIZE 20
#define LIST_WIDGE_ITEM_GAP 5

typedef struct
{
	group_widget_t groupWidge; /*继承GROUP_WIDGE*/
	uint16_t itemGap;		   /*元件之间的Gap*/
	union
	{
		uint16_t itemSize; /*间距大小 定高定宽模式有效*/
		uint16_t itemsH;   /*控件高度 高度自适应模式*/
	} itemsSize;
	int16_t lastXY; /*上一次的位置*/

	/*******动画效果用*******/
	uint16_t firstTime; /*第一时间戳*/
	int16_t firstXY;
	int16_t dSP;				 /*速度值*/
	p_guitime_out_t hGUITimeout; /*超时时间*/
	/************************/

	uint8_t flag; /*0:bit 定高定宽模式/高度自适应模式  1:bit list方向 2:bit 开启动画 7:bit 是否正在滑动*/
} *p_xlist_widget_t, xlist_widget_t;

/*list的模式相应位*/
typedef enum
{
	LIST_SHOW_MODE = 0,
	LIST_DIR = 1,
	LIST_OPEN_ANT = 2,
	LIST_SLIDING = 7
} LIST_WIDGE_FLAG;

/*设置List显示模式*/
#define _SetListWidgeMode(a) (_SET_BIT(((p_xlist_widget_t)(a))->flag, LIST_SHOW_MODE))
#define _ClrListWidgeMode(a) (_CLR_BIT(((p_xlist_widget_t)(a))->flag, LIST_SHOW_MODE))
#define _GetListWidgeMode(a) (_GET_BIT(((p_xlist_widget_t)(a))->flag, LIST_SHOW_MODE))

/*list方向*/
#define _SetOTN_H(a) (_SET_BIT(((p_xlist_widget_t)(a))->flag, LIST_DIR))
#define _SetOTN_V(a) (_CLR_BIT(((p_xlist_widget_t)(a))->flag, LIST_DIR))
#define _GetOTN(a) (_GET_BIT(((p_xlist_widget_t)(a))->flag, LIST_DIR))

/*开启动画*/
#define _OpenListAnimation(a) (_SET_BIT(((p_xlist_widget_t)(a))->flag, LIST_OPEN_ANT))
#define _CloseListAnimation(a) (_CLR_BIT(((p_xlist_widget_t)(a))->flag, LIST_OPEN_ANT))
#define _GetListAnimation(a) (_GET_BIT(((p_xlist_widget_t)(a))->flag, LIST_OPEN_ANT))

/*是否正在滑动*/
#define _SetSliding(a) (_SET_BIT(((p_xlist_widget_t)(a))->flag, LIST_SLIDING))
#define _ClrSliding(a) (_CLR_BIT(((p_xlist_widget_t)(a))->flag, LIST_SLIDING))
#define _GetSlideState(a) (_GET_BIT(((p_xlist_widget_t)(a))->flag, LIST_SLIDING))

PUBLIC p_xlist_widget_t ListWidgetCreate(int16_t x, int16_t y, uint16_t w, uint16_t h);
PUBLIC uint8_t ListWidgetAdd(p_xlist_widget_t hBaseWidge, p_widget_base_t widge);
PUBLIC void ListWidgetPaint(void *hObject);
PUBLIC void ListWidgetClose(p_xlist_widget_t hObject);
PUBLIC int8_t ListWidgetCallBack(void *hObject, p_msg_t hMsg);

PUBLIC void ListWidgetSetFlag(p_xlist_widget_t hBaseWidge, LIST_WIDGE_FLAG flagBisPoi, BOOL status);
