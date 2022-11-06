/*
 * list.h
 *
 *  Created on: 2022年7月23日
 *      Author: Administrator
 */

#ifndef INC_LIST_H_
#define INC_LIST_H_

struct list{
	 struct list *next;
	 struct list *prev;
};

/**
 * @brief 获得list的入口位置
 */
#define list_entry(ptr,type,member)\
	    ((type *)( (char *)(ptr) - (unsigned long)(&((type*)0)->member)))

/**
 * 开始迭代
 */
#define list_iterator_start(ls) for(struct list *__item__ = (ls);__item__!=NULL;__item__=__item__->next){\
	if(!__item__){\
		break;\
	}\
	do{}while(0)

/**
 * 获得迭代的入口
 */
#define list_iterator_entry(type, member) list_entry(__item__,type,member)

/**
 * 结束迭代
 */
#define list_iterator_end() }do{}while(0)


void list_init(struct list *list);
/**
 * 插入到前面
 */
void list_insert_before(struct list* list,struct list* insert_list);

/**
 * 插入到后面
 */
void list_insert_after(struct list* list,struct list* insert_list);

/**
 * 从链表中移除
 */
void list_remove(struct list* list);

#endif /* INC_LIST_H_ */
