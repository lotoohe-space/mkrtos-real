/*
 * object.h
 *
 *  Created on: 2022��7��23��
 *      Author: Administrator
 */

#ifndef INC_OBJECT_H_
#define INC_OBJECT_H_

#include <mkrtos/list.h>

#define OBJECT_NAME_LEN 16

enum dev_type{
	OBJECT_NONE_TYPE,
	OBJECT_DEV_TYPE,
	OBJECT_MEM_TYPE,
	OBJECT_SLOT_TYPE,
	OBJECT_MISC_TYPE,
};

struct object {
	char name[OBJECT_NAME_LEN];//<!��������
	struct list list;//!<���������
	enum dev_type type; //!<��������
	void* user_data;//!<�û�����
};


void object_init(struct object *obj);
struct object * object_create(void);
void object_free(struct object *obj);
void object_reg(struct object *obj,const char* name);
void object_unreg(struct object *obj);
struct object* object_find(const char* name);

#endif /* INC_OBJECT_H_ */
