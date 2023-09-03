#pragma once

#include "kobject.h"
#include "slist.h"
#include "mm_wrap.h"
#include "assert.h"
#include "kobject.h"
#include "obj_space.h"
#include "mm_space.h"
#include "ref.h"

typedef struct task
{
    kobject_t kobj;
    obj_space_t obj_space;
    mm_space_t mm_space;
    ram_limit_t *lim;
    ref_counter_t ref_cn;
} task_t;

void task_init(task_t *task, ram_limit_t *ram, int is_knl);
task_t *task_create(ram_limit_t *lim, int is_knl);
int task_alloc_base_ram(task_t *tk, ram_limit_t *lim, size_t size);
