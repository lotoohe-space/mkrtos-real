#pragma once

#include "assert.h"
#include "prot.h"
#include "ram_limit.h"
#include "slist.h"
#include "spinlock.h"
#include "syscall.h"
#include "types.h"
#include "util.h"
struct kobject;
typedef struct kobject kobject_t;
struct obj_space;
typedef struct obj_space obj_space_t;

typedef void (*syscall_func)(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f);
typedef void (*obj_unmap_func)(obj_space_t *obj_space_obj, kobject_t *kobj);
typedef void (*obj_release_stage_1_func)(kobject_t *kobj);
typedef void (*obj_release_stage_2_func)(kobject_t *kobj);
typedef bool_t (*obj_release_put)(kobject_t *kobj);

#define KOBJ_NAME_SIZE 8

/**
 * @brief 对象的类型
 *
 */
enum knl_obj_type {
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
    SHARE_MEM_TYPE,
    VMAM_TYPE,
};
/**
 * @brief 以下信息用于调试
 *
 */
typedef struct kobj_dbg {
    char name[KOBJ_NAME_SIZE]; //!< 8字节的对象名字
} kobj_dbg_t;

/**
 * @brief 映射节点
 *
 */
typedef struct kobj_map {
    slist_head_t node;
    mword_t map_cnt; //!< 映射计数器
} kobj_map_t;

static inline void kobj_map_init(kobj_map_t *map)
{
    map->map_cnt = 0;
    slist_init(&map->node);
}

/**
 * @brief 内核对象
 *
 */
typedef struct kobject {
    kobj_map_t mappable;                   //!< 映射对象
    spinlock_t lock;                       //!< 锁对象
    kobj_dbg_t dbg;                        //!< 调试用对象
    slist_head_t del_node;                 //!< 删除链表
    syscall_func invoke_func;              //!< 系统调用
    obj_release_stage_1_func stage_1_func; //!< 删除的stage1
    obj_release_stage_2_func stage_2_func; //!< 删除的stage2
    obj_release_put put_func;              //!< put函数
    obj_unmap_func unmap_func;             //!< unmap时调用
    enum knl_obj_type kobj_type;           //!< 当前对象类型
} kobject_t;

typedef struct kobj_del_list {
    slist_head_t head;
} kobj_del_list_t;

static inline void kobj_del_list_init(kobj_del_list_t *list)
{
    slist_init(&list->head);
}
static inline void kobj_del_list_add(kobj_del_list_t *list, slist_head_t *node)
{
    if (!slist_in_list(node)) {
        slist_add(&list->head, node);
    }
}
static inline void kobj_del_list_clr(kobj_del_list_t *list)
{
    slist_init(&list->head);
}
/**
 * @brief 遍历并删除每个对象
 *
 * @param list
 */
static inline void kobj_del_list_to_do(kobj_del_list_t *list)
{
    kobject_t *del_kobj;

    slist_foreach(del_kobj, &list->head, del_node)
    {
        del_kobj->stage_1_func(del_kobj);
        /*释放内核对象*/
        if (del_kobj->put_func(del_kobj)) {
            del_kobj->stage_2_func(del_kobj);
        }
    }
}
void kobject_set_name(kobject_t *kobj, const char *name);
char *kobject_get_name(kobject_t *kobj);
void kobject_invalidate(kobject_t *kobj);
void kobject_invoke(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f);
bool_t kobject_put(kobject_t *kobj);
void kobject_release_stage1(kobject_t *kobj);
void kobject_release_stage2(kobject_t *kobj);
void kobject_init(kobject_t *kobj, enum knl_obj_type type);
