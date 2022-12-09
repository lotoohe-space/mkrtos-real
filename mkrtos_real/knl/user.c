/*
 * user.c
 *
 *  Created on: 2022年9月11日
 *      Author: Administrator
 */

#include <mkrtos.h>
#include <mkrtos/list.h>
#include <mkrtos/sched.h>
#include <mkrtos/user.h>
#include <type.h>
#include <arch/atomic.h>
#include <ipc/spin_lock.h>
#include <config.h>

static struct user user_list;
static struct spin_lock _lock = {0};

#define user_name(user) ((user)->name)
#define user_pwd(user)	((user)->pwd)

static inline void ref_inc(struct user *a) {
	atomic_inc(&a->ref);
}
static inline void ref_dec(struct user *a) {
	atomic_dec(&a->ref);
}
static inline void lock(void) {
	spin_lock(&_lock);
}
static inline void unlock(void) {
	spin_unlock(&_lock);
}

static struct user root_user = { .name = "root", .pwd = "1234", .id = 0, .ref =
		{ 0 } };

int init_user(void) {
	list_init(&user_list);
	//TODO:从文件中去读用户信息
	lock();
	list_insert_before(&user_list, &root_user);
	unlock();
	return 0;
}
INIT_LV2(init_user);

struct user* user_get(const char *name) {
	struct task *task;
	if (task->ruid != ROOT_USER_ID) {
		return NULL;
	}
	task = get_current_task();
	lock();
	list_iterator_start(&user_list)
	;
	struct user *temp;
	temp = list_iterator_entry(struct user, ls);
	if (mkrtos_strcmp(temp->name, name) == 0) {
		ref_inc(temp);
		unlock();
		return temp;
	}list_iterator_end();
	unlock();
	return NULL;
}
void user_put(struct user *u) {
	MKRTOS_ASSERT(u != NULL);
	//TODO:
	ref_dec(u);
}
/**
 * 用户登录检测
 */
struct user* user_login_check(const char *name, const char *pwd) {
	struct user *u;
	u = user_get(name);
	if (!u) {
		return NULL;
	}

	if (mkrtos_strcmp(u->pwd, pwd) != 0) {
		return NULL;
	}
	return u;
}

