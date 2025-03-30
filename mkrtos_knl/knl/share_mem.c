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
#include "printk.h"
#include "ref.h"
#if IS_ENABLED(CONFIG_BUDDY_SLAB)
#include <slab.h>
static slab_t *share_mem_slab;
static slab_t *share_mem_task_node_slab;
#endif

/**
 * @brief store node of task.
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
    SHARE_MEM_CNT_BUDDY_CNT, //!< Buddy allocated continuous memory, maximum 4M, default memory allocation method
    SHARE_MEM_CNT_CMA_CNT,   //!< Continuous memory allocated by CMA
    SHARE_MEM_CNT_DPD,       //!< discrete memory
} share_mem_type_t;
/**
 * @brief share mem kernel object.
 *
 */
typedef struct share_mem
{
    kobject_t kobj; //!< kernel object.
    struct
    {
        void *mem;        //!< Memory pointer (used when using continuous memory)
        void **mem_array; //!< When using array addresses in discrete memory, array memory stores the address of discrete memory
    };
    size_t size;               //!< Memory size, (depending on arch, size may be limited to, for example: n^2）
    dlist_head_t task_head;    //!< Which tasks use the shared memory
    share_mem_type_t mem_type; //!< memory type
    ram_limit_t *lim;          //!< memory limit
    ref_counter_t ref;         //!< Reference count must be zero to free kobj memory
} share_mem_t;

enum share_mem_op
{
    SHARE_MEM_MAP,    //!< share mem map to task.
    SHARE_MEM_UNMAP,  //!< share mem unmap to task.
    SHARE_MEM_RESIZE, //!< share mem resize.
};
#if IS_ENABLED(CONFIG_BUDDY_SLAB)
/**
 * Initialize the memory allocator for shared memory objects
 */
static void share_mem_slab_init(void)
{
    share_mem_slab = slab_create(sizeof(share_mem_t), "share_mem");
    assert(share_mem_slab);
    share_mem_task_node_slab = slab_create(sizeof(share_mem_task_node_t), "share_mem_task_node");
    sizeof(share_mem_task_node_slab);
}
INIT_KOBJ_MEM(share_mem_slab_init);
#endif
/**
 * @brief  Unmapping shared memory
 *
 * @param sm
 * @return int
 */
static void *share_mem_unmap_task(share_mem_t *sm, task_t *tk)
{
    vaddr_t vaddr = 0;
    share_mem_task_node_t *pos;
    umword_t status = spinlock_lock(&sm->kobj.lock);

    if (status < 0)
    {
        return NULL;
    }
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
 * @brief Mapping shared memory
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
        // If not found, insert a new one
        share_mem_task_node_t *task_node;
#if IS_ENABLED(CONFIG_MMU)
        task_node = mm_limit_alloc_slab(share_mem_task_node_slab, tk->lim);
#else
        task_node = mm_limit_alloc(tk->lim, sizeof(share_mem_task_node_t));
#endif
        if (!task_node)
        {
            // memory alloc failed.
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
    task_t *cur_task = thread_get_current_task();
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
        mm_limit_free_align_raw(cur_task->mm_space.mem_block_inx, obj->lim, obj->mem, obj->size);
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
 * @brief alloc phy memory.
 *
 * @param obj
 * @return int
 */
static int share_mem_alloc_pmem(share_mem_t *obj)
{
    int align_size = 0;
    task_t *cur_task = thread_get_current_task();

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
            //!< The size must be an integer multiple of 2
            return -EINVAL;
        }
        align_size = obj->size;
#elif CONFIG_MPU_VERSION == 2
        if (obj->size < MPU_ALIGN_SIZE || (obj->size & (MPU_ALIGN_SIZE - 1)))
        {
            return -EINVAL;
        }
        obj->size += MPU_ALIGN_SIZE;
        align_size = MPU_ALIGN_SIZE;
#endif
#else
        align_size = sizeof(void *);
#endif

        obj->mem = mm_limit_alloc_align_raw(cur_task->mm_space.mem_block_inx, obj->lim, obj->size, align_size);
#endif
        if (obj->mem == NULL)
        {
            return -ENOMEM;
        }
        printk("share mem:[0x%x 0x%x]\n", obj->mem, (char *)obj->mem + obj->size);
        memset(obj->mem, 0, obj->size);
        break;
    case SHARE_MEM_CNT_DPD:
    {
#if IS_ENABLED(CONFIG_MMU)
        /** Non contiguous memory, requested by page */
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
                /* Insufficient memory, release the requested memory */
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
static int share_mem_pmem_resize(share_mem_t *obj, size_t new_size)
{
    int ret;
    task_t *cur_task = thread_get_current_task();

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
        mm_limit_free_buddy(obj->lim, obj->mem, ojb->size);
#else
        mm_limit_free_align_raw(cur_task->mm_space.mem_block_inx, obj->lim, obj->mem, obj->size);
#endif
        obj->mem = NULL;
        break;
    case SHARE_MEM_CNT_DPD:
    {
#if IS_ENABLED(CONFIG_MMU)
        int mem_cnt = ROUND_UP(obj->size, PAGE_SIZE);
        size_t mem_array_size = ALIGN(mem_cnt * sizeof(void *), PAGE_SIZE);

        for (int i = 0; i < mem_cnt; i++)
        {
            mm_limit_free_buddy(obj->lim, obj->mem_array[i], PAGE_SIZE);
        }
        mm_limit_free_buddy(obj->lim, obj->mem_array, mem_array_size);
        obj->mem_array = NULL;
#else
        return -ENOSYS;
#endif
    }
    break;
    }
    size_t old_size = obj->size;

    obj->size = new_size;
    ret = share_mem_alloc_pmem(obj);
    if (ret < 0)
    {
        obj->size = old_size;
        ret = share_mem_alloc_pmem(obj);
    }
    return ret;
}
/**
 * @brief Shared memory performs mapping operations
 *
 * @param obj shared memory kernel object.
 * @param addr The virtual address mapped to, as well as its attributes
 * @return ssize_t
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

    addr.flags |= VMA_ADDR_RESV;
    //!< Set to reserve memory, physical memory that reserves memory and does not apply for it in the kernel
    ret = task_vma_alloc(&task->mm_space.mem_vma, addr, obj->size, 0, &ret_addr); //!< Apply for virtual memory
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

    vma_addr_set_flags(&addr, vma_addr_get_flags(addr) | VMA_ADDR_RESV | VMA_ADDR_PAGE_FAULT_SIM); // Set to reserve mode
    ret = task_vma_alloc(&task->mm_space.mem_vma, addr, obj->size,
                         (vaddr_t)(obj->mem), ret_vaddr);
    if (ret < 0)
    {
        printk("share mem task map failed, pmem:0x%x vmem:0x%x.\n", obj->mem, *ret_vaddr);
        return ret;
    }
    // printk("share mem task map, pmem:0x%x vmem:0x%x.\n", obj->mem, *ret_vaddr);
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
static void share_mem_unmap_op(task_t *tk, share_mem_t *sm)
{
    vaddr_t addr;

    // Remove from records
    addr = (vaddr_t)share_mem_unmap_task(sm, tk);
    if (addr)
    {
        share_mem_unmap(sm, addr);
        ref_counter_dec_and_release(&tk->ref_cn, &tk->kobj);
        ref_counter_dec_and_release(&sm->ref, &sm->kobj);
        // printk("share mem task unmap, pmem:0x%x vmem:0x%x.\n", sm->mem, addr);
    }
}
static void share_mem_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    ssize_t ret = 0;
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

        ref_counter_inc(&sm->ref);
        ret = share_mem_alloc_pmem(sm); //!< If there is no memory request, apply for memory
        if (ret < 0)
        {
            ref_counter_dec_and_release(&sm->ref, &sm->kobj);
            goto end;
        }

        vma_addr = vma_addr_create_raw(f->regs[0]);
        ret = share_mem_map(sm, vma_addr, &ret_vaddr); //!< Perform mapping operation
        if (ret < 0)
        {
            ref_counter_dec_and_release(&sm->ref, &sm->kobj);
            goto end;
        }
        ret = share_mem_map_task(sm, ret_vaddr); //!< Store tasks that use this share mem
        if (ret < 0)
        {
            share_mem_unmap(sm, ret_vaddr);
            ref_counter_dec_and_release(&sm->ref, &sm->kobj);
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
        share_mem_unmap_op(task, sm);
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    case SHARE_MEM_RESIZE:
    {
        mword_t status = spinlock_lock(&sm->kobj.lock);

        if (status < 0)
        {
            ret = -EPERM;
            goto resize_end;
        }
        if (ref_counter_val(&sm->ref) == 1)
        {
            ret = share_mem_pmem_resize(sm, f->regs[0]);
        }
        spinlock_set(&sm->kobj.lock, status);
    resize_end:
        tag = msg_tag_init4(0, 0, 0, ret);
    }
    }

    f->regs[0] = tag.raw;
}
static void share_mem_obj_unmap(obj_space_t *obj_space, kobject_t *kobj)
{
    task_t *task = container_of(obj_space, task_t, obj_space);
    share_mem_t *sm = container_of(kobj, share_mem_t, kobj);

    share_mem_unmap_op(task, sm);
}
static void share_mem_release_stage1(kobject_t *kobj)
{
    share_mem_t *sm = container_of(kobj, share_mem_t, kobj);
    kobject_invalidate(kobj);
}
static void share_mem_release_stage2(kobject_t *kobj)
{
    share_mem_t *sm = container_of(kobj, share_mem_t, kobj);
    task_t *cur_task = thread_get_current_task();
    assert(dlist_is_empty(&sm->task_head));

#if IS_ENABLED(CONFIG_MMU)
    share_mem_free_pmem(sm);
    mm_limit_free_slab(share_mem_slab, sm->lim, sm);
#else
    mm_limit_free_align_raw(cur_task->mm_space.mem_block_inx, sm->lim, sm->mem, sm->size);
    mm_limit_free(sm->lim, sm);
#endif
    printk("share mem 0x%x free.\n", sm);
}
bool_t share_mem_put(kobject_t *kobj)
{
    share_mem_t *sm = container_of(kobj, share_mem_t, kobj);

    return ref_counter_dec(&sm->ref) == 1;
}
/**
 * @brief share_mem init.
 *
 * @param sm Initialize share_cem object
 * @param max Maximum limit value
 */
static void share_mem_init(share_mem_t *sm, umword_t max)
{
    kobject_init(&sm->kobj, SHARE_MEM_TYPE);
    ref_counter_init(&sm->ref);
    ref_counter_inc(&sm->ref);
    sm->size = max;
    sm->kobj.invoke_func = share_mem_syscall;
    sm->kobj.unmap_func = share_mem_obj_unmap;
    sm->kobj.stage_1_func = share_mem_release_stage1;
    sm->kobj.stage_2_func = share_mem_release_stage2;
    sm->kobj.put_func = share_mem_put;
}
/**
 * @brief Create a shared memory object
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
 * @brief Callback function for creating shared memory objects
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
 * @brief Factory registration function
 *
 */
static void share_mem_register(void)
{
    factory_register(share_mem_func, SHARE_MEM_PROT);
}
INIT_KOBJ(share_mem_register);
