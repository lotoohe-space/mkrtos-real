/*
 * dev_object.c
 *
 *  Created on: 2022年7月23日
 *      Author: Administrator
 */

#include <mkrtos/object.h>
#include <assert.h>
#include <mkrtos/fs.h>

enum dev_type{
	OBJECT_NONE_TYPE,
	OBJECT_DEV_TYPE,
	OBJECT_MEM_TYPE,
	OBJECT_MISC_TYPE,
};

struct dev_object{
	struct object parent;
	/**
	 * @brief 对象类型
	 */
	enum dev_type type;


	/**
	* @brief 操作函数
	*/
	struct file_operations *f_ops;
};


int dev_object_init(struct dev_object *obj){
	MKRTOS_ASSERT(obj==NULL);
	return object_init((struct object*)obj);
}

int dev_object_open(struct dev_object *obj){
	MKRTOS_ASSERT(obj==NULL);

	if(obj->f_ops && obj->f_ops->open){
		obj->f_ops->open();
	}

}

