/*
 * object.h
 *
 *  Created on: 2022��7��23��
 *      Author: Administrator
 */

#ifndef INC_OBJECT_H_
#define INC_OBJECT_H_

#include <mkrtos/list.h>

#define OBJECT_NAME_LEN 32



struct object{
	/**
	 * @brief ��������
	 */
	char name[OBJECT_NAME_LEN];

	/**
	 * @brief ���������
	 */
	struct list list;

	
	void *user_data;
};

void object_init(struct object *obj);
void object_reg(struct object *obj);
void object_unreg(struct object *obj);
struct object* object_find(const char* name);

#endif /* INC_OBJECT_H_ */
