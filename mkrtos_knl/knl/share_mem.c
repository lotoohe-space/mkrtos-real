/**
 * @file share_mem.c
 * @author zhangzheng (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-12-31
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <types.h>
#include <kobject.h>
#include <task.h>
#include <thread.h>
#include <factory.h>
#include "mm_wrap.h"
#include "mpu.h"
#include "init.h"
#include "dlist.h"

/**
 * @brief 存储task的节点
 *
 */
typedef struct share_mem_task_node
{
    dlist_item_t node;
    task_t *task;
} share_mem_task_node_t;
/**
 * @brief 共享内存对象
 *
 */
typedef struct share_mem
{
    kobject_t kobj;         //!< 内核对象
    void *mem;              //!< 内存指针
    size_t size;            //!< 内存大小，为n^2。
    dlist_head_t task_head; //!< 哪些任务使用了该共享内存
    ram_limit_t *lim;       //!< 内存限额
} share_mem_t;

enum share_mem_op
{
    SHARE_MEM_MAP,
    SHARE_MEM_UNMAP,
};
/**
 * @brief  共享内存解除映射
 *
 * @param sm
 * @return int
 */
static void share_mem_unmap_task(share_mem_t *sm)
{
    share_mem_task_node_t *pos;
    umword_t status = spinlock_lock(&sm->kobj.lock);

    if (status < 0)
    {
        return;
    }
    task_t *tk = thread_get_current_task();
    dlist_foreach(&sm->task_head, pos, share_mem_task_node_t, node)
    {
        if (pos->task == tk)
        {
            dlist_del(&sm->task_head, &pos->node);
            mm_limit_free(tk->lim, pos);
            break;
        }
    }
    spinlock_set(&sm->kobj.lock, status);
}
/**
 * @brief 共享内存映射
 *
 * @param task
 */
static int share_mem_map_task(share_mem_t *sm)
{
    int ret = -1;
    int flag = 0;
    share_mem_task_node_t *pos;
    umword_t status = spinlock_lock(&sm->kobj.lock);

    if (status < 0)
    {
        return status;
    }
    task_t *tk = thread_get_current_task();

    dlist_foreach(&sm->task_head, pos, share_mem_task_node_t, node)
    {
        if (pos->task == tk)
        {
            flag = 1;
            break;
        }
    }

    if (!flag)
    {
        // 没有找到则插入一个新的
        share_mem_task_node_t *task_node = mm_limit_alloc(tk->lim, sizeof(share_mem_task_node_t));

        if (!task_node)
        {
            // 内存分配失败
            ref_counter_dec_and_release(&tk->ref_cn, &tk->kobj);
            spinlock_set(&sm->kobj.lock, status);
            ret = -ENOMEM;
            goto end;
        }
        dlist_item_init(&task_node->node);
        task_node->task = tk;
        dlist_add_head(&sm->task_head, &task_node->node);
        ref_counter_inc(&tk->ref_cn);
        ret = 0;
    }
    else
    {
        ret = 1;
    }
end:
    spinlock_set(&sm->kobj.lock, status);
    return ret;
}

static void share_mem_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);
    task_t *task = thread_get_current_task();
    share_mem_t *sm = container_of(kobj, share_mem_t, kobj);

    if (sys_p.prot != SHARE_MEM_PROT)
    {
        f->r[0] = msg_tag_init4(0, 0, 0, -EPROTO).raw;
        return;
    }
    switch (sys_p.op)
    {
    case SHARE_MEM_MAP:
    {
        uint8_t attr;

        attr = f->r[0] & 0xff;
        int map_ret = share_mem_map_task(sm);

        if (map_ret >= 0)
        {
            if (map_ret == 0)
            {
                // 共享内存映射
                bool_t ret = mm_space_add(&task->mm_space, (umword_t)(sm->mem), sm->size, attr);

                if (ret)
                {
                    mpu_switch_to_task(task);
                }
                else
                {
                    share_mem_unmap_task(sm);
                    map_ret == -EAGAIN;
                }
            }
        }
        f->r[1] = (umword_t)sm->mem;
        f->r[2] = sm->size;
        tag = msg_tag_init4(0, 0, 0, map_ret);
    }
    break;
    case SHARE_MEM_UNMAP:
    {
        // 从记录中删除
        share_mem_unmap_task(sm);
        // 共享内存解除映射
        mm_space_del(&task->mm_space, (umword_t)sm->mem);
        mpu_switch_to_task(task);
        ref_counter_dec_and_release(&task->ref_cn, &task->kobj);
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    }

    f->r[0] = tag.raw;
}
static void share_mem_unmap(obj_space_t *obj_space, kobject_t *kobj)
{
    task_t *task = container_of(obj_space, task_t, obj_space);
    share_mem_t *sm = container_of(kobj, share_mem_t, kobj);

    // 从记录中删除
    share_mem_unmap_task(sm);
    // 共享内存解除映射
    mm_space_del(&task->mm_space, (umword_t)sm->mem);
    mpu_switch_to_task(task);
    ref_counter_dec_and_release(&task->ref_cn, &task->kobj);
}
static void share_mem_release_stage1(kobject_t *kobj)
{
    share_mem_t *sm = container_of(kobj, share_mem_t, kobj);
    kobject_invalidate(kobj);
}
static void share_mem_release_stage2(kobject_t *kobj)
{
    share_mem_t *sm = container_of(kobj, share_mem_t, kobj);

    assert(dlist_is_empty(&sm->task_head));

    mm_limit_free_align(sm->lim, sm->mem, sm->size);
    mm_limit_free(sm->lim, sm);
    printk("share mem 0x%x free.\n", sm);
}
/**
 * @brief share_mem init.
 *
 * @param fac 初始化的share_mem对象
 * @param max 最大限额值
 */
static void share_mem_init(share_mem_t *sm, umword_t max)
{
    kobject_init(&sm->kobj, SHARE_MEM_TYPE);
    sm->kobj.invoke_func = share_mem_syscall;
    sm->kobj.unmap_func = share_mem_unmap;
    sm->kobj.stage_1_func = share_mem_release_stage1;
    sm->kobj.stage_2_func = share_mem_release_stage2;
}
/**
 * @brief 创建共享内存对象
 *
 * @param lim
 * @param max
 * @return share_mem_t*
 */
static share_mem_t *share_mem_create(ram_limit_t *lim, size_t max)
{

#if CONFIG_MPU_VERSION == 1
    if (max < PAGE_SIZE || !is_power_of_2(max))
    {
        //!< 大小必须是2的整数倍
        return NULL;
    }
#elif CONFIG_MPU_VERSION == 2
    if (max < MPU_ALIGN_SIZE || (max & (MPU_ALIGN_SIZE - 1)))
    {
        //!< 大小必须是2的整数倍
        return NULL;
    }
    max += MPU_ALIGN_SIZE;
#endif

    share_mem_t *mem = mm_limit_alloc(lim, sizeof(share_mem_t));

    if (!mem)
    {
        return NULL;
    }
    mem->mem = mm_limit_alloc_align(lim, max, CONFIG_MPU_VERSION == 2 ? MPU_ALIGN_SIZE : max);
    if (!mem->mem)
    {
        mm_limit_free(lim, mem);
        return NULL;
    }
    mem->size = max;
    mem->lim = lim;
    share_mem_init(mem, max);
    return mem;
}
/**
 * @brief 共享内存对象创建的回调函数
 *
 * @param lim
 * @param arg0
 * @param arg1
 * @param arg2
 * @param arg3
 * @return kobject_t*
 */
static kobject_t *share_mem_func(ram_limit_t *lim, umword_t arg0, umword_t arg1,
                                 umword_t arg2, umword_t arg3)
{
    share_mem_t *irq = share_mem_create(lim, arg0);

    if (!irq)
    {
        return NULL;
    }
    return &irq->kobj;
}
/**
 * @brief 工厂注册函数
 *
 */
static void share_mem_register(void)
{
    factory_register(share_mem_func, SHARE_MEM_PROT);
}
INIT_KOBJ(share_mem_register);
