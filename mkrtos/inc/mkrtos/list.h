/*
 * list.h
 *
 *  Created on: 2022Äê7ÔÂ23ÈÕ
 *      Author: Administrator
 */

#ifndef INC_LIST_H_
#define INC_LIST_H_

struct list{
	 struct list *next;
	 struct list *prev;
};

#define list_entry(ptr,type,member)\
	    ((type *)( (char *)(ptr) - (unsigned long)(&((type*)0)->member)))

#define list_iterator_start(ls) for(struct list *__item__ = (ls);__item__!=NULL;__item__=__item__->next){\
	if(!__item__){\
		break;\
	}\
	do{}while(0)

#define list_iterator_entry(type, member) list_entry(__item__,type,member)

#define list_iterator_end() }do{}while(0)

void list_insert_before(struct list* list,struct list* insert_list);
void list_insert_after(struct list* list,struct list* insert_list);
void list_remove(struct list* list);

#endif /* INC_LIST_H_ */
