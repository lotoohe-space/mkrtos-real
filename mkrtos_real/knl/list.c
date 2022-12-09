/*
 * list.c
 *
 *  Created on: 2022年7月23日
 *      Author: Administrator
 */

#include <mkrtos/list.h>
#include <mkrtos.h>
void list_init(struct list *list) {
	MKRTOS_ASSERT(list != 0);
	list->next = 0;
	list->prev = 0;
}
/**
 * @brief 在list前面插入一个
 */
void list_insert_before(struct list* list,struct list* insert_list){
	insert_list->prev=list->prev;
	insert_list->next=list;

	if(list->prev){
		list->prev->next=insert_list;
	}
	list->prev=insert_list;
}
/**
 * @brief 在list后面插入一个
 */
void list_insert_after(struct list* list,struct list* insert_list){
	insert_list->prev=list;
	insert_list->next=list->next;

	if(list->next){
		list->next->prev=insert_list;
	}
	list->next=insert_list;
}
/**
 * @brief 移除一个
 */
void list_remove(struct list* list){
	if(list->next){
		list->next->prev=list->prev;
	}
	list->prev->next=list->next;
}

