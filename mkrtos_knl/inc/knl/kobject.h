#pragma once

#include "types.h"
#include "slist.h"
#include "prot.h"
#include "syscall.h"
#include "prot.h"
#include "ram_limit.h"
struct kobject;
typedef struct kobject kobject_t;

typedef msg_tag_t (*syscall_func)(kobject_t *kobj, ram_limit_t *ram, entry_frame_t *f);

typedef struct kobj_map
{
    slist_head_t node;
    umword_t map_cnt; //!< 映射计数器
} kobj_map_t;

static inline void kobj_map_init(kobj_map_t *map)
{
    map->map_cnt = 0;
    slist_init(&map->node);
}

typedef struct kobject
{
    kobj_map_t mappable;
    syscall_func invoke_func;
} kobject_t;

static inline msg_tag_t kobject_invoke(kobject_t *kobj, ram_limit_t *ram, entry_frame_t *f)
{
    /*TODO:*/
    return msg_tag_init3(0, 0, -ENOSYS);
}

static inline void kobject_init(kobject_t *kobj)
{
    kobj_map_init(&kobj->mappable);
    kobj->invoke_func = kobject_invoke;
}
