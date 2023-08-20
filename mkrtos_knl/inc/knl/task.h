#pragma once

#include "kobject.h"
#include "slist.h"
#include "mm_wrap.h"
#include "assert.h"
#include "kobject.h"
#include "obj_space.h"

typedef struct task
{
    kobject_t kobj;
    obj_space_t obj_space;
} task_t;

task_t *thread_get_current_task(void);
void task_init(task_t *task, ram_limit_t *ram);
