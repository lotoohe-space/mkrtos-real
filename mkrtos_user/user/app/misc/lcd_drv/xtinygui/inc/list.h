/**
 * @file list.h
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include "util.h"
typedef struct list
{
	struct list *next; // 左节点
	struct list *prev; // 右节点
} *p_list_t, list_t;

typedef struct list_head
{
	list_t head;
	list_t *tail;
} *p_list_head_t, list_head_t;

static inline void ListInit(list_t *list)
{
	list->next = NULL;
	list->prev = NULL;
}
static inline void ListHeadInit(list_head_t *head)
{
	ListInit(&head->head);
	head->tail = NULL;
}

int ListAddFirst(list_head_t *list_m, p_list_t add_item);
void ListAddLast(list_head_t *list_m, p_list_t add_item);
p_list_t ListGet(list_head_t *list_m, int index);
void ListDelByVal(list_head_t *list_m, p_list_t val);
void ListDelInx(list_head_t *list_m, int index);
void ListClear(list_head_t *list_m);
int ListLen(list_head_t *list_m);
p_list_t ListGetLast(list_head_t *list_m);

static inline BOOL list_is_empty(list_head_t *head)
{
	return !!(head->tail);
}
/**
 * @brief 获得结构体的入口
 *
 */
#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define list_foreach(pos, head, member)                        \
	for (pos = list_entry((head)->next, typeof(*pos), member); \
		 (pos) != list_entry(NULL, typeof(*pos), member);      \
		 pos = list_entry((pos)->member.next, typeof(*pos), member))

#define list_foreach_without_next(pos, head, member)           \
	for (pos = list_entry((head)->next, typeof(*pos), member); \
		 (pos) != list_entry(NULL, typeof(*pos), member);)

#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*pos), member)

#define list_foreach_l(pos, head, member)                 \
	for (pos = list_entry((head), typeof(*pos), member);  \
		 (pos) != list_entry(NULL, typeof(*pos), member); \
		 pos = list_entry((pos)->member.prev, typeof(*pos), member))
