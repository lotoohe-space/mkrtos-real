/*
 * list.c
 *
 *  Created on: 2022��7��23��
 *      Author: Administrator
 */

#include <mkrtos/list.h>

/**
 * @brief ��listǰ�����һ��
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
 * @brief ��list�������һ��
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
 * @brief �Ƴ�һ��
 */
void list_remove(struct list* list){
	if(list->next){
		list->next->prev=list->prev;
	}
	list->prev->next=list->next;
}

