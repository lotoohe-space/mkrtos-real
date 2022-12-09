//
// Created by Administrator on 2022/3/13.
//

#ifndef UNTITLED1_USER_H
#define UNTITLED1_USER_H
#include <mkrtos.h>
#include <mkrtos/list.h>
#include <mkrtos/sched.h>
#include <type.h>
#include <arch/atomic.h>
#include <ipc/spin_lock.h>
#include <config.h>

#define ROOT_USER_ID 0

struct user {
	char name[16];
	char pwd[16];
	uid_t id;
	struct list ls;
	atomic_t ref; //!<引用计数
};


struct user* user_login_check(const char *name,const char *pwd) ;
void user_put(struct user* u) ;

#endif //UNTITLED1_USER_H
