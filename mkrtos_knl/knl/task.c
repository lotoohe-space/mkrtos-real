/**
 * @file task.c
 * @author zhangzheng (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "task.h"
#include "kobject.h"
#include "factory.h"
#include "init.h"
#include "map.h"
#include "thread.h"
#include "misc.h"
#include "spinlock.h"
enum task_op_code
{
    TASK_OBJ_MAP,
    TASK_OBJ_UNMAP,
    TASK_ALLOC_RAM_BASE,
    TASK_OBJ_VALID,
};
static bool_t task_put(kobject_t *kobj);
static void task_release_stage1(kobject_t *kobj);
static void task_release_stage2(kobject_t *kobj);

int task_alloc_base_ram(task_t *tk, ram_limit_t *lim, size_t size)
{
    if (tk->mm_space.mm_block)
    {
        return -EACCES;
    }
    // 申请init的ram内存
    void *ram = mpu_ram_alloc(&tk->mm_space, lim, size + THREAD_MSG_BUG_LEN);
    if (!ram)
    {
        printk("Failed to request process memory.\n");
        return -ENOMEM;
    }
    mm_space_set_ram_block(&tk->mm_space, ram, size + THREAD_MSG_BUG_LEN);
    printk("task alloc size is %d, base is 0x%x\n", size + THREAD_MSG_BUG_LEN, ram);
    return 0;
}
task_t *thread_get_bind_task(thread_t *th)
{
    return container_of(th->task, task_t, kobj);
}
static void task_unlock_2(spinlock_t *sp0, spinlock_t *sp1, int status0, int status1)
{
    if (sp0 < sp1)
    {
        spinlock_set(sp1, status1);
        spinlock_set(sp0, status0);
    }
    else
    {
        spinlock_set(sp0, status0);
        spinlock_set(sp1, status1);
    }
}
static int task_lock_2(spinlock_t *sp0, spinlock_t *sp1, int *st0, int *st1)
{
    int status0;
    int status1;
    if (sp0 < sp1)
    {
        status0 = spinlock_lock(sp0);
        if (status0 < 0)
        {
            return FALSE;
        }
        status1 = spinlock_lock(sp1);
        if (status1 < 0)
        {
            spinlock_set(sp0, status0);
            return FALSE;
        }
        *st0 = status0;
        *st1 = status1;
    }
    else
    {
        status0 = spinlock_lock(sp1);
        if (status0 < 0)
        {
            return FALSE;
        }
        status1 = spinlock_lock(sp0);
        if (status1 < 0)
        {
            spinlock_set(sp1, status0);
            return FALSE;
        }
        *st0 = status1;
        *st1 = status0;
    }
    return TRUE;
}
static void task_syscall_func(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    task_t *cur_task = thread_get_current_task();
    task_t *tag_task = container_of(kobj, task_t, kobj);
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);

    if (sys_p.prot != TASK_PROT)
    {
        f->r[0] = msg_tag_init4(0, 0, 0, -EINVAL).raw;
        return;
    }

    switch (sys_p.op)
    {
    case TASK_OBJ_VALID:
    {
        kobject_t *source_kobj = obj_space_lookup_kobj(&cur_task->obj_space, f->r[1]);

        if (!source_kobj)
        {
            tag = msg_tag_init4(0, 0, 0, -ENOENT);
            break;
        }
        tag = msg_tag_init4(0, 0, 0, 1);
    }
    break;
    case TASK_OBJ_MAP:
    {
        kobj_del_list_t del;
        int st0, st1;

        kobj_del_list_init(&del);
        int suc = task_lock_2(&tag_task->kobj.lock, &cur_task->kobj.lock, &st0, &st1);
        if (!suc)
        {
            tag = msg_tag_init4(0, 0, 0, -EINVAL);
            break;
        }
        int ret = obj_map_src_dst(&tag_task->obj_space, &cur_task->obj_space,
                                  f->r[2], f->r[1],
                                  tag_task->lim, f->r[3], &del);
        kobj_del_list_to_do(&del);
        task_unlock_2(&tag_task->kobj.lock, &cur_task->kobj.lock, st0, st1);
        tag = msg_tag_init4(0, 0, 0, ret);
    }
    break;
    case TASK_OBJ_UNMAP:
    {
        kobject_t *del_kobj;
        kobj_del_list_t kobj_list;

        mword_t status = spinlock_lock(&tag_task->kobj.lock);
        if (status < 0)
        {
            tag = msg_tag_init4(0, 0, 0, -EINVAL);
            break;
        }
        kobj_del_list_init(&kobj_list);
        obj_unmap(&tag_task->obj_space, vpage_create_raw(f->r[1]), &kobj_list);
        kobj_del_list_to_do(&kobj_list);
        spinlock_set(&tag_task->kobj.lock, status);
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    case TASK_ALLOC_RAM_BASE:
    {
        mword_t status = spinlock_lock(&tag_task->kobj.lock);
        if (status < 0)
        {
            tag = msg_tag_init4(0, 0, 0, -EINVAL);
            break;
        }
        int ret = task_alloc_base_ram(tag_task, tag_task->lim, f->r[1]);
        tag = msg_tag_init4(0, 0, 0, ret);
        f->r[1] = (umword_t)(tag_task->mm_space.mm_block);
        spinlock_set(&tag_task->kobj.lock, status);
    }
    break;
    default:
        break;
    }

    f->r[0] = tag.raw;
}

void task_init(task_t *task, ram_limit_t *ram, int is_knl)
{
    assert(task);
    assert(ram);

    kobject_init(&task->kobj, TASK_TYPE);
    obj_space_init(&task->obj_space, ram);
    mm_space_init(&task->mm_space, is_knl);
    ref_counter_init(&task->ref_cn);
    ref_counter_inc(&task->ref_cn);
    task->lim = ram;
    task->kobj.invoke_func = task_syscall_func;
    task->kobj.put_func = task_put;
    task->kobj.stage_1_func = task_release_stage1;
    task->kobj.stage_2_func = task_release_stage2;
    mm_space_add(&task->mm_space, KNL_TEXT, 64 * 1024 * 1024, REGION_RO); // TODO:
}

static bool_t task_put(kobject_t *kobj)
{
    task_t *tk = container_of(kobj, task_t, kobj);
    return ref_counter_dec(&tk->ref_cn) == 1;
}
static void task_release_stage1(kobject_t *kobj)
{
    task_t *tk = container_of(kobj, task_t, kobj);
    kobj_del_list_t kobj_list;
    kobject_invalidate(kobj);
    kobj_del_list_init(&kobj_list);
    obj_unmap_all(&tk->obj_space, &kobj_list);
    kobj_del_list_to_do(&kobj_list);
}
static void task_release_stage2(kobject_t *kobj)
{
    task_t *tk = container_of(kobj, task_t, kobj);
    kobj_del_list_t kobj_list;
    task_t *cur_tk = thread_get_current_task();

    obj_space_release(&tk->obj_space, tk->lim);
    mm_limit_free_align(tk->lim, tk->mm_space.mm_block, tk->mm_space.mm_block_size);
    mm_limit_free(tk->lim, tk);
    // if (cur_tk == tk)
    // {
    thread_sched();
    // }
    // mm_trace();
    printk("release tk %x\n", tk);
}
void task_kill(task_t *tk)
{
    kobj_del_list_t kobj_list;
    kobj_del_list_init(&kobj_list);
    obj_unmap(&tk->obj_space, vpage_create3(KOBJ_DELETE_RIGHT, 0, TASK_PROT), &kobj_list);
    kobj_del_list_to_do(&kobj_list);
    thread_sched();
}
task_t *task_create(ram_limit_t *lim, int is_knl)
{
    task_t *tk = mm_limit_alloc(lim, sizeof(task_t));

    if (!tk)
    {
        return NULL;
    }
    task_init(tk, lim, is_knl);
    printk("create task is 0x%x\n", tk);
    return tk;
}

/**
 * @brief 该函数创建一个工厂对象
 *
 * @param lim
 * @param arg0
 * @param arg1
 * @param arg2
 * @param arg3
 * @return kobject_t*
 */
static kobject_t *task_create_func(ram_limit_t *lim, umword_t arg0, umword_t arg1,
                                   umword_t arg2, umword_t arg3)
{
    task_t *tk = task_create(lim, FALSE);

    return &tk->kobj;
}

/**
 * @brief 工厂注册函数 TODO:
 *
 */
void task_factory_register(void)
{
    factory_register(task_create_func, TASK_PROT);
}
INIT_KOBJ(task_factory_register);
