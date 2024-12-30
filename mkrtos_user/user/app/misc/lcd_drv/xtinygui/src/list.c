/**
 * @file list.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "x_malloc.h"
#include "list.h"
#include <stdio.h>
#include <assert.h>

// 在list头部添加元素
int ListAddFirst(list_head_t *list_m, p_list_t add_item)
{
	assert(list_m);
	if (list_m->head.next == NULL)
	{
		assert(list_m->tail);
		list_m->tail = add_item;
	}
	add_item->next = list_m->head.next;
	if (list_m->head.next != NULL)
	{
		list_m->head.next->prev = add_item;
	}
	add_item->prev = NULL;
	list_m->head.next = add_item;
	return 0;
}
// 在list尾部中添加元素
void ListAddLast(list_head_t *list_m, p_list_t add_item)
{
	assert(list_m);
	p_list_t temp_lm;

	temp_lm = list_m->tail;
	if (temp_lm == NULL)
	{
		assert(list_m->head.next == NULL);
		add_item->prev = NULL;
		list_m->head.next = add_item;
		list_m->head.prev = NULL;
		list_m->tail = add_item;
	}
	else
	{
		temp_lm->next = add_item;
		add_item->prev = temp_lm;
		add_item->next = NULL;
		list_m->tail = add_item;
	}
}
/*获取最后一个元素*/
p_list_t ListGetLast(list_head_t *list_m)
{
	assert(list_m);
	return list_m->tail;
}
/*获取第index个元素*/
p_list_t ListGet(list_head_t *list_m, int index)
{
	assert(list_m);
	p_list_t temp_lm;
	int len = 0;

	temp_lm = list_m->head.next;
	for (; temp_lm;)
	{
		if (len == index)
		{
			return temp_lm;
		}
		len++;
		temp_lm = temp_lm->next;
	}
	return 0x00;
}

/**
 * @brief 通过Val值删除一个元素
 *
 * @param hList
 * @param val
 */
void ListDelByVal(list_head_t *list_m, p_list_t val)
{
	assert(list_m);
	assert(val);

	if (val == list_m->tail)
	{
		//!< 删除的是最后一个元素
		list_m->tail = list_m->tail->prev;
		val->prev->next = NULL;
		return;
	}
	if (val->prev)
	{
		val->prev->next = val->next;
	}
	if (val->next)
	{
		val->next->prev = val->prev;
	}
}
/*清空list*/
void ListClear(list_head_t *list_m)
{
	list_m->head.next = NULL;
	list_m->head.prev = NULL;
}
/*List的长度*/
int ListLen(list_head_t *list_m)
{
	assert(list_m);
	p_list_t temp_lm;
	int len = 0;

	temp_lm = list_m->head.next;
	for (; temp_lm;)
	{
		len++;
		temp_lm = temp_lm->next;
	}
	return len;
}
