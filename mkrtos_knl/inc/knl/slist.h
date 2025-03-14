#pragma once
#include "types.h"
/**
 * @brief 循环双向链表
 */
typedef struct slist_head
{
	struct slist_head *prev; //!< 前一个
	struct slist_head *next; //!< 后一个
} slist_head_t;
#define SLIST_HEAD_INIT(name) { &(name), &(name) }
#define WRITE_ONCE(var, val) (*((volatile typeof(val) *)(&(var))) = (val))
#define READ_ONCE(var) (*((volatile typeof(var) *)(&(var))))
static inline void INIT_SLIST_HEAD(slist_head_t *list)
{
    WRITE_ONCE(list->next, list);
    list->prev = list;
}
static inline void slist_init(slist_head_t *list)
{
	list->prev = list;
	list->next = list;
}
/**
 * @brief 链表是否为空
 *
 * @param list
 * @return mword_t
 */
static inline mword_t slist_is_empty(slist_head_t *list)
{
	return (list->prev == list && list->next == list) ||
		   (list->prev == 0 && list->next == 0);
}
/**
 * @brief 节点是否再链表中
 *
 * @param item
 * @return bool_t
 */
static inline bool_t slist_in_list(slist_head_t *item)
{
	if (slist_is_empty(item))
	{
		return FALSE;
	}
	if (item->prev->next != item)
	{
		return FALSE;
	}
	if (item->next->prev != item)
	{
		return FALSE;
	}
	return TRUE;
}
static inline bool_t slist_in_list_check(slist_head_t *head, slist_head_t *item)
{
	slist_head_t *_head = head;

	head = head->next;

	while (head != _head) {
		if (item == head) {
			return TRUE;
		}
		head = head->next;
	}
}
/**
 * @brief 在头部添加一个节点
 *
 * @param head
 * @param item
 */
static inline void slist_add(slist_head_t *head, slist_head_t *item)
{
	item->next = head->next;
	item->prev = head;
	head->next->prev = item;
	head->next = item;
}
static inline void slist_add_append(slist_head_t *head, slist_head_t *item)
{
	slist_head_t *tail = head->prev;

	return slist_add(tail, item);
}
/**
 * @brief 获取第一个节点
 *
 * @param head
 * @return slist_head_t*
 */
static inline slist_head_t *slist_first(slist_head_t *head)
{
	return head->next;
}
/**
 * @brief 删除一个节点
 *
 * @param item
 */
static inline void slist_del(slist_head_t *item)
{
	item->prev->next = item->next;
	item->next->prev = item->prev;

	// slist_init(item);
}

/**
 * @brief 获得结构体的入口
 *
 */
#define slist_entry(ptr, type, member) container_of(ptr, type, member)

/**
 * @brief 遍历每一个节点获得struct的入口
 */
#define slist_foreach(pos, head, member)                        \
	for (pos = slist_entry((head)->next, typeof(*pos), member); \
		 &(pos)->member != (head) && !slist_is_empty(head);     \
		 pos = slist_entry((pos)->member.next, typeof(*pos), member))

#define slist_foreach_not_next(pos, head, member)               \
	for (pos = slist_entry((head)->next, typeof(*pos), member); \
		 &(pos)->member != (head) && !slist_is_empty(head);)

#define slist_next_entry(pos, head, member) \
	slist_entry((pos)->member.next, typeof(*pos), member)
