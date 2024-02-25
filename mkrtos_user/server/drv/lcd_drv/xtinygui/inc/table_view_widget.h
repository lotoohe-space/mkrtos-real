/**
 * @file table_view_widget.h
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
#include "group_widge.h"

#define TABLE_WIDGE_MARK_HEAD(a) TableWidge##a

#define TABLE_WIDGE_H_NUM 5
#define TABLE_WIDGE_V_NUM 5

#define TABLE_WIDGE_HEIGHT 30
#define TABLE_WIDGE_WIDTH 30

#define TABLE_WIDGE_ITEM_GAP 5

typedef struct
{
	group_widget_t groupWidge; /*继承GROUP_WIDGE*/

	uint8_t HItemsNum;   /*横向多少控件*/
	uint8_t VItemsNum;   /*竖向多少控件*/
	uint16_t itemHeight; /*控件高度*/
	uint16_t itemWidth;  /*控件宽度*/

	uint8_t flag;
} *p_table_widget_t, table_widget_t;
