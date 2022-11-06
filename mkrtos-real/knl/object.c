/*
 * object.c
 *
 *  Created on: 2022年7月22日
 *      Author: Administrator
 */

#include <type.h>
#include <assert.h>
#include <string.h>
#include <mkrtos/object.h>
#include <mkrtos/list.h>
#include <mkrtos/sched.h>
#include <arch/arch.h>
#include <knl_service.h>


//enum obj_type{
//	ObjMemType,
//	ObjFipcType
//};
//
//struct knl_obj_set{
//	struct list obj_list;
//	int obj_size;
//	enum obj_type type;
//};
//
//static struct knl_obj_set obj_set_list[]={
//		{0,0,ObjMemType},
//};

#define SYS_OBJECT_NUM 3

/**
 * @brief 系统对象的list
 */
static struct object object_list;

struct object * object_create(void){
	struct object* obj;
	obj=malloc(sizeof(struct object));
	if(!obj){
		return NULL;
	}
	return obj;
}

void object_free(struct object *obj){
	free(obj);
}
/**
 * @brief 初始化一个对象
 */
void object_init(struct object *obj){
	MKRTOS_ASSERT(obj == NULL);
	obj->list.next=NULL;
	obj->list.prev=NULL;
	obj->name[0]=0;
}
void object_reg(struct object *obj,const char* name){
	MKRTOS_ASSERT(obj == NULL);
	sche_lock();
	if(object_find(name)){
		sche_unlock();
		return ;
	}
	mkrtos_strncpy(obj->name,name,OBJECT_NAME_LEN);
	list_insert_after(&object_list.list, &obj->list);
	sche_unlock();
}
void object_unreg(struct object *obj){
	MKRTOS_ASSERT(obj == NULL);
	sche_lock();
	list_remove(&obj->list);
	sche_unlock();
}
struct object* object_find(const char* name){
	struct object *tmp_obj;
	MKRTOS_ASSERT(name == NULL);
	sche_lock();
	list_iterator_start(&(object_list.list));

	tmp_obj=list_iterator_entry(struct object,list);

	if(mkrtos_strcmp(tmp_obj->name,name)==0){
		sche_unlock();
		return tmp_obj;
	}

	list_iterator_end();
	sche_unlock();
	return NULL;
}
