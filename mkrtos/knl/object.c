/*
 * object.c
 *
 *  Created on: 2022��7��22��
 *      Author: Administrator
 */

#include <type.h>
#include <assert.h>
#include <string.h>
#include <mkrtos/object.h>
#include <mkrtos/list.h>


/**
 * @brief ϵͳ�����list
 */
static struct object object_list;


/**
 * @brief ��ʼ��һ������
 */
void object_init(struct object *obj){
	MKRTOS_ASSERT(obj == NULL);
	obj->list.next=NULL;
	obj->list.prev=NULL;
	obj->name[0]=0;
}
void object_reg(struct object *obj){
	MKRTOS_ASSERT(obj == NULL);
	list_insert_after(&object_list.list, &obj->list);
}
void object_unreg(struct object *obj){
	MKRTOS_ASSERT(obj == NULL);
	list_remove(&obj->list);
}
struct object* object_find(const char* name){
	struct object *tmp_obj;
	MKRTOS_ASSERT(name == NULL);
	list_iterator_start(&(object_list.list));

	tmp_obj=list_iterator_entry(struct object,list);

	if(strcmp(tmp_obj->name,name)==0){
		return tmp_obj;
	}

	list_iterator_end();
	return NULL;
}


