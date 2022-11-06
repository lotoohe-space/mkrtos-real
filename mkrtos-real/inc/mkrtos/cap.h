/*
 * cap.h
 *
 *  Created on: 2022年9月3日
 *      Author: Administrator
 */

#include "type.h"
//capability
struct capability {
	//cap_inx;
	int cap_inx:31;
	char cap_vaild:1;
	//cap所包含的obj
	ptr_t obj:30;
	//权限
	char rights:2;
};



#define cap_get_obj(cap)	((struct object*)(((cap)->obj)<<2))
#define cap_set_obj(cap,o)	(cap)->obj=((o)>>2)
#define cap_get_rights(cap)	((cap)->rights)
#define cap_set_rights(cap,r)	(cap)->rights=((r)&0x3)
#define cap_is_vaild(cap)	(cap)->cap_vaild
#define cap_set_vaild(cap,v)	(cap)->cap_vaild=((v)&0x1)
#define cap_get_inx(cap)	(cap)->cap_inx
#define cap_set_inx(cap,i)	(cap)->cap_inx=((i)>>0x1)
