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
#include <string.h>
#include <slab.h>
#include <vma.h>
#include "mm_wrap.h"
#include "mpu.h"
#include "init.h"
#include "dlist.h"
#if IS_ENABLED(CONFIG_BUDDY_SLAB)
#include <slab.h>
static slab_t *share_mem_slab;
static slab_t *share_mem_task_node_slab;
#endif

/**
 * @brief 存储task的节点
 *
 */
typedef struct share_mem_task_node
{
    dlist_item_t node;
    task_t *task;
    vaddr_t addr;
} share_mem_task_node_t;

typedef enum share_mem_type
{
    SHARE_MEM_CNT_BUDDY_CNT, //!< buddy分配的连续内存，最大4M，默认的内存分配方式
    SHARE_MEM_CNT_CMA_CNT,   //!< cma分配的连续内存
    SHARE_MEM_CNT_DPD,       //!< 离散内存
} share_mem_type_t;
/**
 * @brief 共享内存对象
 *
 */
typedef struct share_mem
{
    kobject_t kobj; //!< 内核对象
    struct
    {
        void *mem;        //!< 内存指针(连续内存时使用)
        void **mem_array; //!< 离散内存时当数组地址使用，数组内存放离散内存的地址
    };
    size_t size;               //!< 内存大小，(根据arch不同，size大小可能会被限制为，例如：n^2）。
    dlist_head_t task_head;    //!< 哪些任务使用了该共享内存
    share_mem_type_t mem_type; //!< 内存类型
    ram_limit_t *lim;          //!< 内存限额
} share_mem_t;

enum share_mem_op
{
    SHARE_MEM_MAP,
    SHARE_MEM_UNMAP,
};

static void share_mem_slab_init(void)
{
#if IS_ENABLED(CONFIG_BUDDY_SLAB)
    share_mem_slab = slab_create(sizeof(share_mem_t), "share_mem");
    assert(share_mem_slab);
    share_mem_task_node_slab = slab_create(sizeof(share_mem_task_node_t), "share_mem_task_node");
    sizeof(share_mem_task_node_slab);
#endif
}
INIT_KOBJ_MEM(share_mem_slab_init);
/**
 * @brief  共享内存解除映射
 *
 * @param sm
 * @return int
 */
static void *share_mem_unmap_task(share_mem_t *sm)
{
    vaddr_t vaddr = 0;
    share_mem_task_node_t *pos;
    umword_t status = spinlock_lock(&sm->kobj.lock);

    if (status < 0)
    {
        return NULL;
    }
    task_t *tk = thread_get_current_task();
    dlist_foreach(&sm->task_head, pos, share_mem_task_node_t, node)
    {
        if (pos->task == tk)
        {
            dlist_del(&sm->task_head, &pos->node);
#if IS_ENABLED(CONFIG_MMU)
            mm_limit_free_slab(share_mem_task_node_slab, tk->lim, pos);
#else
            mm_limit_free(tk->lim, pos);
#endif
            vaddr = pos->addr;
            break;
        }
    }
    spinlock_set(&sm->kobj.lock, status);
    return (void *)vaddr;
}
/**
 * @brief 共享内存映射
 *
 * @param task
 */
static int share_mem_map_task(share_mem_t *sm, vaddr_t addr)
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
        share_mem_task_node_t *task_node;
#if IS_ENABLED(CONFIG_MMU)
        task_node = mm_limit_alloc_slab(share_mem_task_node_slab, tk->lim);
#else
        task_node = mm_limit_alloc(tk->lim, sizeof(share_mem_task_node_t));
#endif
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
        task_node->addr = addr;
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
static int share_mem_free_pmem(share_mem_t *obj)
{
    assert(obj);
    switch (obj->mem_type)
    {
    case SHARE_MEM_CNT_CMA_CNT:
#if IS_ENABLED(CONFIG_MMU)
        /*TODO:support CMA mem.*/
        return -ENOSYS;
#endif
    case SHARE_MEM_CNT_BUDDY_CNT:
#if IS_ENABLED(CONFIG_MMU)
        mm_limit_free_buddy(obj->lim, obj->mem, obj->size);
#else
        mm_limit_free_align(obj->lim, obj->mem, obj->size);
#endif
        break;
    case SHARE_MEM_CNT_DPD:
    {
#if IS_ENABLED(CONFIG_MMU)
        for (ssize_t st = 0; st < obj->size; st += PAGE_SIZE)
        {
            mm_limit_free_buddy(obj->lim, obj->mem_array[st / PAGE_SIZE], PAGE_SIZE);
        }
        int mem_cnt = ROUND_UP(obj->size, PAGE_SIZE);
        size_t mem_array_size = ALIGN(mem_cnt * sizeof(void *), PAGE_SIZE);

        mm_limit_free_buddy(obj->lim, obj->mem_array, mem_array_size);
#else
        return -ENOSYS;
#endif
    }
    break;
    }
    return 0;
}
/**
 * @brief 申请物理内存
 *
 * @param obj
 * @return int
 */
static int share_mem_alloc_pmem(share_mem_t *obj)
{
    int align_size = 0;

    assert(obj);
    if (obj->mem)
    {
        return 0;
    }
    switch (obj->mem_type)
    {
    case SHARE_MEM_CNT_CMA_CNT:
#if IS_ENABLED(CONFIG_MMU)
        /*TODO:support CMA mem.*/
        return -ENOSYS;
#endif
    case SHARE_MEM_CNT_BUDDY_CNT:
#if IS_ENABLED(CONFIG_MMU)
        obj->mem = mm_limit_alloc_buddy(obj->lim, obj->size);
#else
        align_size = obj->size;

#if CONFIG_MPU
#if CONFIG_MPU_VERSION == 1
        if (obj->size < (1UL << CONFIG_PAGE_SHIFT) || !is_power_of_2(obj->size))
        {
            //!< 大小必须是2的整数倍
            return -EINVAL;
        }
        align_size = obj->size;
#elif CONFIG_MPU_VERSION == 2
        if (obj->size < MPU_ALIGN_SIZE || (obj->size & (MPU_ALIGN_SIZE - 1)))
        {
            //!< 大小必须是2的整数倍
            return -EINVAL;
        }
        obj->size += MPU_ALIGN_SIZE;
        align_size = MPU_ALIGN_SIZE;
#endif
#else
        align_size = sizeof(void *);
#endif

        obj->mem = mm_limit_alloc_align(obj->lim, obj->size, align_size);
#endif
        if (obj->mem == NULL)
        {
            return -ENOMEM;
        }
        memset(obj->mem, 0, obj->size);
        break;
    case SHARE_MEM_CNT_DPD:
    {
#if IS_ENABLED(CONFIG_MMU)
        /** 非连续内存，按页申请 */
        int mem_cnt = ROUND_UP(obj->size, PAGE_SIZE);
        size_t mem_array_size = ALIGN(mem_cnt * sizeof(void *), PAGE_SIZE);

        obj->mem_array = (void **)mm_limit_alloc_buddy(obj->lim, mem_array_size);
        if (!obj->mem_array)
        {
            return -ENOMEM;
        }
        memset(obj->mem_array, 0, mem_array_size);
        for (int i = 0; i < mem_cnt; i++)
        {
            obj->mem_array[i] = mm_limit_alloc_buddy(obj->lim, PAGE_SIZE);
            if (obj->mem_array[i] == NULL)
            {
                /* 内存不足，释放申请的内存 */
                mm_limit_free_buddy(obj->lim, obj->mem_array, mem_array_size);
                for (int j = 0; j < i; j++)
                {
                    mm_limit_free_buddy(obj->lim, obj->mem_array[j], PAGE_SIZE);
                }
                obj->mem_array = NULL;
                return -ENOMEM;
            }
            memset(obj->mem_array[i], 0, PAGE_SIZE);
        }
#else
        return -ENOSYS;
#endif
    }
    break;
    }
    return 0;
}
/**
 * @brief 共享内存执行映射操作
 *
 * @param obj 共享内存对象
 * @param addr 映射到的虚拟地址，以及属性
 * @return int
 */
static ssize_t share_mem_map(share_mem_t *obj, vma_addr_t addr, vaddr_t *ret_vaddr)
{
    int ret;
    assert(obj);
    assert(ret_vaddr);
    ssize_t map_size = 0;
    task_t *task = thread_get_current_task();

#if IS_ENABLED(CONFIG_MMU)
    vaddr_t ret_addr;

    addr.flags |= VMA_ADDR_RESV;                                                  //!< 设置为保留内存，保留内存的物理内存，不在内核中申请
    ret = task_vma_alloc(&task->mm_space.mem_vma, addr, obj->size, 0, &ret_addr); //!< 申请虚拟内存
    if (ret < 0)
    {
        return ret;
    }
    switch (obj->mem_type)
    {
    case SHARE_MEM_CNT_BUDDY_CNT:
    case SHARE_MEM_CNT_CMA_CNT:
    {
        for (ssize_t st = ret_addr; st < ret_addr + obj->size; st += PAGE_SIZE, map_size += PAGE_SIZE)
        {
            ret = task_vma_page_fault(&task->mm_space.mem_vma, st, obj->mem + (st - ret_addr));
            if (ret < 0)
            {
                printk("%s:%d task map failed, pmem:0x%lx vmem:0x%lx.\n", obj->mem + (st - ret_addr), st);
                break;
            }
        }
    }
    break;
    case SHARE_MEM_CNT_DPD:
    {
        for (ssize_t st = ret_addr; st < ret_addr + obj->size; st += PAGE_SIZE, map_size += PAGE_SIZE)
        {
            ret = task_vma_page_fault(&task->mm_space.mem_vma, st, obj->mem_array[(st - ret_addr) / PAGE_SIZE]);
            if (ret < 0)
            {
                printk("%s:%d task map failed, pmem:0x%lx vmem:0x%lx.\n", obj->mem + (st - ret_addr), st);
                break;
            }
        }
    }
    break;
    }
    *ret_vaddr = ret_addr;
    if (map_size == 0)
    {
        return ret;
    }
#else

    vma_addr_set_flags(&addr, vma_addr_get_flags(addr) | VMA_ADDR_RESV | VMA_ADDR_PAGE_FAULT_SIM); // 设置为保留模式
    ret = task_vma_alloc(&task->mm_space.mem_vma, addr, obj->size,
                         (vaddr_t)(obj->mem), ret_vaddr);
    if (ret < 0)
    {
        return ret;
    }
    map_size = obj->size;
    // *ret_vaddr = (vaddr_t)(obj->mem);
#endif
    return map_size;
}
static int share_mem_unmap(share_mem_t *obj, vaddr_t vaddr)
{
    task_t *task = thread_get_current_task();

    task_vma_free_pmem(&task->mm_space.mem_vma, vaddr, obj->size, FALSE);
    return 0;
}
static void share_mem_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    ssize_t ret;
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);
    task_t *task = thread_get_current_task();
    share_mem_t *sm = container_of(kobj, share_mem_t, kobj);

    if (sys_p.prot != SHARE_MEM_PROT)
    {
        f->regs[0] = msg_tag_init4(0, 0, 0, -EPROTO).raw;
        return;
    }
    switch (sys_p.op)
    {
    case SHARE_MEM_MAP:
    {
        vma_addr_t vma_addr;
        vaddr_t ret_vaddr;
        ret = share_mem_alloc_pmem(sm); //!< 如果内存没有申请，则申请内存
        if (ret < 0)
        {
            goto end;
        }

        vma_addr = vma_addr_create_raw(f->regs[0]);
        ret = share_mem_map(sm, vma_addr, &ret_vaddr); //!< 执行映射操作
        if (ret < 0)
        {
            goto end;
        }
        ret = share_mem_map_task(sm, ret_vaddr); //!< 存储那些task使用了该share mem.
        if (ret < 0)
        {
            share_mem_unmap(sm, ret_vaddr);
            ret = -EAGAIN;
            goto end;
        }
        f->regs[1] = (umword_t)ret_vaddr;
        f->regs[2] = sm->size;
    end:
        tag = msg_tag_init4(0, 0, 0, ret);
    }
    break;
    case SHARE_MEM_UNMAP:
    {
        vaddr_t addr;

        // 从记录中删除
        addr = (vaddr_t)share_mem_unmap_task(sm);
        if (addr)
        {
            share_mem_unmap(sm, addr);
            ref_counter_dec_and_release(&task->ref_cn, &task->kobj);
        }
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    }

    f->regs[0] = tag.raw;
}
static void share_mem_obj_unmap(obj_space_t *obj_space, kobject_t *kobj)
{
    task_t *task = container_of(obj_space, task_t, obj_space);
    share_mem_t *sm = container_of(kobj, share_mem_t, kobj);
    vaddr_t addr;

    // 从记录中删除
    addr = (vaddr_t)share_mem_unmap_task(sm);
    if (addr)
    {
        share_mem_unmap(sm, addr);
        ref_counter_dec_and_release(&task->ref_cn, &task->kobj);
    }
}
static void share_mem_release_stage1(kobject_t *kobj)
{
    share_mem_t *sm = container_of(kobj, share_mem_t, kobj);
    kobject_invalidate(kobj);
}
static void share_mem_release_stage2(kobject_t *kobj)
{
    share_mem_t *sm = container_of(kobj, share_mem_t, kobj);

    assert(dlist_is_empty(&sm->task_head));//TODO:有bug

#if IS_ENABLED(CONFIG_MMU)
    share_mem_free_pmem(sm);
    mm_limit_free_slab(share_mem_slab, sm->lim, sm);
#else
    mm_limit_free_align(sm->lim, sm->mem, sm->size);
    mm_limit_free(sm->lim, sm);
#endif
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
    sm->size = max;
    sm->kobj.invoke_func = share_mem_syscall;
    sm->kobj.unmap_func = share_mem_obj_unmap;
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
static share_mem_t *share_mem_create(ram_limit_t *lim, share_mem_type_t type, size_t max)
{
    share_mem_t *mem;

#if IS_ENABLED(CONFIG_MMU)
    mem = mm_limit_alloc_slab(share_mem_slab, lim);

    if (mem == NULL)
    {
        return NULL;
    }
    memset(mem, 0, sizeof(share_mem_t));
    max = ALIGN(max, (1 << CONFIG_PAGE_SHIFT));
#else
    mem = mm_limit_alloc(lim, sizeof(share_mem_t));

    if (!mem)
    {
        return NULL;
    }
#endif
    mem->lim = lim;
    mem->mem_type = type;
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
    share_mem_t *irq = share_mem_create(lim, arg0, arg1);

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
