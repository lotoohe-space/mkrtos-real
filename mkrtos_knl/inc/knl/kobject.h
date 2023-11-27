#pragma once

#include "types.h"
#include "slist.h"
#include "prot.h"
#include "syscall.h"
#include "prot.h"
#include "ram_limit.h"
#include "util.h"
#include "spinlock.h"
#include "assert.h"
struct kobject;
typedef struct kobject kobject_t;
struct obj_space;
typedef struct obj_space obj_space_t;

typedef void (*syscall_func)(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f);
typedef void (*obj_unmap_func)(obj_space_t *obj_space_obj, kobject_t *kobj);
typedef void (*obj_release_stage_1_func)(kobject_t *kobj);
typedef void (*obj_release_stage_2_func)(kobject_t *kobj);
typedef bool_t (*obj_release_put)(kobject_t *kobj);
typedef struct kobj_map
{
    slist_head_t node;
    mword_t map_cnt; //!< 映射计数器
} kobj_map_t;

static inline void kobj_map_init(kobj_map_t *map)
{
    map->map_cnt = 0;
    slist_init(&map->node);
}

enum knl_obj_type
{
    BASE_KOBJ_TYPE,
    TASK_TYPE,
    THREAD_TYPE,
    IRQ_SENDER_TYPE,
    IPC_TYPE,
    LOG_TYPE,
    MM_TYPE,
    FACTORY_TYPE,
    SYS_TYPE,
    FUTEX_TYPE,
};

typedef struct kobject
{
    kobj_map_t mappable;
    spinlock_t lock;
    slist_head_t del_node;
    syscall_func invoke_func;
    obj_release_stage_1_func stage_1_func;
    obj_release_stage_2_func stage_2_func;
    obj_release_put put_func;
    obj_unmap_func unmap_func;
    enum knl_obj_type kobj_type;
} kobject_t;

typedef struct kobj_del_list
{
    slist_head_t head;
} kobj_del_list_t;

static inline void kobj_del_list_init(kobj_del_list_t *list)
{
    slist_init(&list->head);
}
static inline void kobj_del_list_add(kobj_del_list_t *list, slist_head_t *node)
{
    if (!slist_in_list(node))
    {
        slist_add(&list->head, node);
    }
}
static inline void kobj_del_list_clr(kobj_del_list_t *list)
{
    slist_init(&list->head);
}
static inline void kobj_del_list_to_do(kobj_del_list_t *list)
{
    kobject_t *del_kobj;
    slist_foreach(del_kobj, &list->head, del_node)
    {
        del_kobj->stage_1_func(del_kobj);
        /*释放内核对象*/
        if (del_kobj->put_func(del_kobj))
        {
            del_kobj->stage_2_func(del_kobj);
        }
    }
}
static inline void kobject_invalidate(kobject_t *kobj)
{
    assert(!spinlock_is_invalidation(&kobj->lock));
    spinlock_invalidate(&kobj->lock);
}

static inline void kobject_invoke(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    /*TODO:*/
    f->r[0] = msg_tag_init4(0, 0, 0, -ENOSYS).raw;
}
static inline bool_t kobject_put(kobject_t *kobj)
{
    return TRUE;
}
static inline void kobject_release_stage1(kobject_t *kobj)
{
}
static inline void kobject_release_stage2(kobject_t *kobj)
{
}

static inline void kobject_init(kobject_t *kobj, enum knl_obj_type type)
{
    kobj_map_init(&kobj->mappable);
    slist_init(&kobj->del_node);
    spinlock_init(&kobj->lock);
    kobj->invoke_func = kobject_invoke;
    kobj->put_func = kobject_put;
    kobj->stage_1_func = kobject_release_stage1;
    kobj->stage_2_func = kobject_release_stage2;
    kobj->kobj_type = type;
}
