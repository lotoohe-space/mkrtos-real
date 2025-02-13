/**
 * @file task.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "task.h"
#include "access.h"
#include "assert.h"
#include "factory.h"
#include "init.h"
#include "knl_misc.h"
#include "kobject.h"
#include "map.h"
#include "printk.h"
#include "spinlock.h"
#include "string.h"
#include "thread.h"
#include "thread_arch.h"
#include "thread_task_arch.h"
#if IS_ENABLED(CONFIG_MMU)
#include "arch.h"
#endif
/**
 * @brief 任务的操作码
 *
 */
enum task_op_code
{
    TASK_OBJ_MAP,        //!< 进行映射操作
    TASK_OBJ_UNMAP,      //!< 进行解除映射操作
    TASK_ALLOC_RAM_BASE, //!< 分配task的基础内存
    TASK_OBJ_VALID,      //!< 判断一个对象是否有效
    TASK_SET_PID,        //!< 设置task的pid
    TASK_GET_PID,        //!< 获取task的pid
    TASK_COPY_DATA,      //!< 拷贝数据到task的数据区域
    TASK_SET_OBJ_NAME,   //!< 设置对象的名字
    TASK_COPY_DATA_TO,   //!< 从当前task拷贝数据到目的task
    TASK_SET_COM_POINT,  //!< 通信点
    TASK_COM_UNLOCK,     //!< 任务通信解锁
    TASK_COM_LOCK,       //!< 任务通信加锁
};
static bool_t task_put(kobject_t *kobj);
static void task_release_stage1(kobject_t *kobj);
static void task_release_stage2(kobject_t *kobj);
#if IS_ENABLED(CONFIG_BUDDY_SLAB)
#include <slab.h>
static slab_t *task_slab;
#endif

/**
 * @brief 在系统初始化时调用，初始化task的内存
 *
 */
static void task_mem_init(void)
{
#if IS_ENABLED(CONFIG_BUDDY_SLAB)
    task_slab = slab_create(sizeof(task_t), "task");
    assert(task_slab);
#endif
}
INIT_KOBJ_MEM(task_mem_init);

#if !IS_ENABLED(CONFIG_MMU)
/**
 * @brief 为task分配其可以使用的内存空间
 *
 * @param tk
 * @param lim
 * @param size
 * @param mem_block 使用的内存块
 * @return int
 */
int task_alloc_base_ram(task_t *tk, ram_limit_t *lim, size_t size, int mem_block)
{
    if (tk->mm_space.mm_block)
    {
        return -EACCES;
    }
    // 申请init的ram内存
    void *ram = mpu_ram_alloc(&tk->mm_space, lim, size + THREAD_MSG_BUG_LEN, mem_block);
    if (!ram)
    {
        printk("Failed to request process memory.\n");
        return -ENOMEM;
    }
    memset(ram, 0, size + THREAD_MSG_BUG_LEN);
    mm_space_set_ram_block(&tk->mm_space, ram, size + THREAD_MSG_BUG_LEN);
    printk("task alloc [0x%x - 0x%x]\n", ram, (umword_t)ram + size + THREAD_MSG_BUG_LEN - 1);
    return 0;
}
#endif
/**
 * @brief 获取线程绑定的task
 *
 * @param th
 * @return task_t*
 */
task_t *thread_get_bind_task(thread_t *th)
{
    return container_of(th->task, task_t, kobj);
}
/**
 * @brief 同时解锁两个进程，防止死锁
 *
 * @param sp0
 * @param sp1
 * @param status0
 * @param status1
 */
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
/**
 * @brief 同时加锁两个进程
 *
 * @param sp0
 * @param sp1
 * @param st0
 * @param st1
 * @return int
 */
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
/**
 * @brief 设置进程的pid，只有pid为0的进程才能够设置
 *
 * @param task 任务对象
 * @param pid 任务的pid
 * @return int
 */
int task_set_pid(task_t *task, pid_t pid)
{
    task_t *cur_task = thread_get_current_task();

    if (cur_task->pid == 0)
    {
        task->pid = pid;
        return 0;
    }
    else
    {
        return -EACCES;
    }
}
/**
 * @brief 进程的系统调用函数
 *
 * @param kobj
 * @param sys_p
 * @param in_tag
 * @param f
 */
static void task_syscall_func(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    task_t *cur_task = thread_get_current_task();
    task_t *tag_task = container_of(kobj, task_t, kobj);
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);

    if (sys_p.prot != TASK_PROT)
    {
        f->regs[0] = msg_tag_init4(0, 0, 0, -EINVAL).raw;
        return;
    }

    switch (sys_p.op)
    {
    case TASK_SET_OBJ_NAME:
    {
        mword_t status = spinlock_lock(&tag_task->kobj.lock);
        if (status < 0)
        {
            tag = msg_tag_init4(0, 0, 0, -EINVAL);
            break;
        }
        kobject_t *source_kobj = obj_space_lookup_kobj(&cur_task->obj_space, f->regs[0]);

        if (!source_kobj)
        {
            spinlock_set(&tag_task->kobj.lock, status);
            tag = msg_tag_init4(0, 0, 0, -EINVAL);
            break;
        }
        kobject_set_name(source_kobj, (char *)(&f->regs[1]));
        spinlock_set(&tag_task->kobj.lock, status);
        tag = msg_tag_init4(0, 0, 0, 1);
    }
    break;
    case TASK_OBJ_VALID: //!< 查看某个obj在task中是否存在
    {
        mword_t status = spinlock_lock(&tag_task->kobj.lock);
        if (status < 0)
        {
            tag = msg_tag_init4(0, 0, 0, -EINVAL);
            break;
        }
        kobject_t *source_kobj = obj_space_lookup_kobj(&cur_task->obj_space, f->regs[1]);

        if (!source_kobj)
        {
            spinlock_set(&tag_task->kobj.lock, status);
            tag = msg_tag_init4(0, 0, 0, 0);
            break;
        }
        f->regs[1] = source_kobj->kobj_type;
        spinlock_set(&tag_task->kobj.lock, status);
        tag = msg_tag_init4(0, 0, 0, 1);
    }
    break;
    case TASK_OBJ_MAP: //!< 从一个task中映射一个对象到目标进程
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
                                  f->regs[2], f->regs[1],
                                  tag_task->lim, f->regs[3], &del);
        kobj_del_list_to_do(&del);
        task_unlock_2(&tag_task->kobj.lock, &cur_task->kobj.lock, st0, st1);
        tag = msg_tag_init4(0, 0, 0, ret);
    }
    break;
    case TASK_OBJ_UNMAP: //!< 解除task中一个进程的创建
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
        obj_unmap(&tag_task->obj_space, vpage_create_raw(f->regs[1]), &kobj_list);
        spinlock_set(&tag_task->kobj.lock, status);
        status = cpulock_lock();
        kobj_del_list_to_do(&kobj_list);
        cpulock_set(status);
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
#if !IS_ENABLED(CONFIG_MMU)
    case TASK_ALLOC_RAM_BASE: //!< 分配task所拥有的内存空间
    {
        mword_t status = spinlock_lock(&tag_task->kobj.lock);
        if (status < 0)
        {
            tag = msg_tag_init4(0, 0, 0, -EINVAL);
            break;
        }
        int ret = task_alloc_base_ram(tag_task, tag_task->lim, f->regs[1], f->regs[2]);
        tag = msg_tag_init4(0, 0, 0, ret);
        f->regs[1] = (umword_t)(tag_task->mm_space.mm_block);
        spinlock_set(&tag_task->kobj.lock, status);
    }
    break;
#endif
    case TASK_COPY_DATA: //!< 拷贝数据到task的内存区域
    {
        void *mem;
        size_t size;

        umword_t st_addr = f->regs[0];
        size_t cp_size = f->regs[1];

        if (cp_size > THREAD_MSG_BUG_LEN)
        {
            tag = msg_tag_init4(0, 0, 0, -EINVAL);
            break;
        }
        if (!is_rw_access(tag_task, (void *)st_addr, cp_size, FALSE))
        {
            tag = msg_tag_init4(0, 0, 0, -EPERM);
            break;
        }
        // printk("addr:0x%x %d.\n", st_addr, cp_size);
        void *msg = thread_get_msg_buf(thread_get_current());

        memcpy((void *)st_addr, msg, cp_size);
        tag = msg_tag_init4(0, 0, 0, 0);
        break;
    }
    case TASK_SET_PID: //!< 设置pid
    {
        tag = msg_tag_init4(0, 0, 0, task_set_pid(tag_task, f->regs[0]));
    }
    break;
    case TASK_GET_PID: //!< 获取pid
    {
        f->regs[1] = tag_task->pid;
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    case TASK_COPY_DATA_TO: //!< 从当前task拷贝到目的task
    {
        int st0, st1;
        int ret = 0;
        obj_handler_t dst_task_hd;
        addr_t src_addr;
        addr_t dst_addr;
        size_t copy_len;
        int suc;

        dst_task_hd = f->regs[0];
        src_addr = f->regs[1];
        dst_addr = f->regs[2];
        copy_len = f->regs[3];
        kobject_t *source_kobj = obj_space_lookup_kobj_cmp_type(&cur_task->obj_space, dst_task_hd, TASK_TYPE);

        if (!source_kobj)
        {
            ret = -EINVAL;
            goto copy_data_to_end;
        }
        task_t *dst_task_obj = container_of(source_kobj, task_t, kobj);

        suc = task_lock_2(&tag_task->kobj.lock, &dst_task_obj->kobj.lock, &st0, &st1);
        if (!suc)
        {
            tag = msg_tag_init4(0, 0, 0, -EINVAL);
            break;
        }
        if (!is_rw_access(tag_task, (void *)src_addr, copy_len, FALSE))
        {
            ret = -EPERM;
            goto copy_data_to_end;
        }
        if (!is_rw_access(dst_task_obj, (void *)dst_addr, copy_len, FALSE))
        {
            ret = -EPERM;
            goto copy_data_to_end;
        }

        paddr_t src_paddr = (paddr_t)task_get_currnt_paddr((vaddr_t)src_addr);
        paddr_t dst_paddr = (paddr_t)task_get_currnt_paddr((vaddr_t)dst_addr);

        memcpy((void *)dst_paddr, (void *)src_paddr, copy_len);

    copy_data_to_end:
        task_unlock_2(&tag_task->kobj.lock, &dst_task_obj->kobj.lock, st0, st1);
        tag = msg_tag_init4(0, 0, 0, ret);
    }
    break;
    case TASK_SET_COM_POINT: //!< 设置通信点
    {
        if (!is_rw_access(tag_task, (void *)(f->regs[1]), f->regs[2], FALSE))
        {
            tag = msg_tag_init4(0, 0, 0, -EPERM);
            break;
        }
        if (f->regs[4] >= WORD_BITS)
        {
            tag = msg_tag_init4(0, 0, 0, -EINVAL);
            break;
        }
        if (!is_rw_access(tag_task, (void *)(f->regs[3]),
                          ROUND_UP(f->regs[4], 8), FALSE))
        {
            tag = msg_tag_init4(0, 0, 0, -EPERM);
            break;
        }
        if (!is_rw_access(tag_task, (void *)(f->regs[5]),
                          THREAD_MSG_BUG_LEN + CONFIG_THREAD_MAP_BUF_LEN * WORD_BYTES, FALSE))
        {
            tag = msg_tag_init4(0, 0, 0, -EPERM);
            break;
        }
        int stack_size = f->regs[2];

        tag_task->nofity_point = (void *)(f->regs[0]);
        tag_task->nofity_stack = (addr_t)(f->regs[1] + stack_size);
        tag_task->nofity_bitmap = (void *)(f->regs[3]);
        tag_task->nofity_bitmap_len = (f->regs[4]);
        tag_task->nofity_msg_buf = (addr_t)f->regs[5];
        tag_task->nofity_map_buf = (umword_t *)((addr_t)f->regs[5] + THREAD_MSG_BUG_LEN);
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    case TASK_COM_UNLOCK:
    {
        task_t *cur_task = thread_get_current_task();

        mutex_unlock(&cur_task->nofity_lock);
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    case TASK_COM_LOCK:
    {
        task_t *cur_task = thread_get_current_task();

        mutex_lock(&cur_task->nofity_lock, 0);
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    default:
        break;
    }

    f->regs[0] = tag.raw;
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
    slist_init(&task->del_node);
    slist_init(&task->nofity_theads_head);
    mutex_init(&task->nofity_lock);
    task->pid = -1;
    task->lim = ram;
    task->kobj.invoke_func = task_syscall_func;
    task->kobj.put_func = task_put;
    task->kobj.stage_1_func = task_release_stage1;
    task->kobj.stage_2_func = task_release_stage2;

#if IS_ENABLED(CONFIG_MMU)
    knl_pdir_init(&task->mm_space.mem_dir, task->mm_space.mem_dir.dir, 3 /*TODO:*/);
#else
    assert(task_vma_alloc(&task->mm_space.mem_vma, vma_addr_create(VPAGE_PROT_RO, 0, 0),
                          align_power_of_2(CONFIG_SYS_TEXT_SIZE), CONFIG_SYS_TEXT_ADDR, NULL) >= 0);
#endif
}

static bool_t task_put(kobject_t *kobj)
{
    task_t *tk = container_of(kobj, task_t, kobj);
    return ref_counter_dec(&tk->ref_cn) == 1;
}
static void task_release_stage1(kobject_t *kobj)
{
    int ret;
    task_t *tk = container_of(kobj, task_t, kobj);
    kobj_del_list_t kobj_list;
    kobject_invalidate(kobj);
    kobj_del_list_init(&kobj_list);
    obj_unmap_all(&tk->obj_space, &kobj_list);
    kobj_del_list_to_do(&kobj_list);

    thread_t *restore_th;
    thread_fast_ipc_item_t ipc_item;

    slist_foreach(restore_th->com, &tk->nofity_theads_head, fast_ipc_node)
    {
        ret = thread_fast_ipc_pop(restore_th, &ipc_item);/*TODO:这里有问题*/
        if (ret >= 0)
        {
            // 还原栈和usp TODO: arch相关的
            restore_th->ipc_status = THREAD_NONE;
            if (restore_th->status == THREAD_SUSPEND)
            {
                thread_ready(restore_th, TRUE);
            }
            restore_th->task = ipc_item.task_bk;
            thread_user_pf_restore(restore_th, ipc_item.usp_backup);
            pf_t *cur_pf = ((pf_t *)((char *)restore_th + CONFIG_THREAD_BLOCK_SIZE + 8)) - 1;
            cur_pf->regs[5] = (umword_t)(thread_get_bind_task(restore_th)->mm_space.mm_block);
            ref_counter_dec_and_release(&tk->ref_cn, &tk->kobj);
        }
    }

    task_vma_clean(&tk->mm_space.mem_vma);
}
static void task_release_stage2(kobject_t *kobj)
{
    task_t *tk = container_of(kobj, task_t, kobj);
    kobj_del_list_t kobj_list;
    // task_t *cur_tk = thread_get_current_task();

    obj_space_release(&tk->obj_space, tk->lim);

#if !IS_ENABLED(CONFIG_MMU)
    if (tk->mm_space.mm_block)
    {
#if CONFIG_MPU
        mm_limit_free_align(tk->lim, tk->mm_space.mm_block, tk->mm_space.mm_block_size);
#else
        mm_limit_free(tk->lim, tk->mm_space.mm_block);
#endif
    }
#endif
#if IS_ENABLED(CONFIG_BUDDY_SLAB)
    mm_limit_free_slab(task_slab, tk->lim, tk);
#else
    mm_limit_free(tk->lim, tk);
#endif
    // if (cur_tk == tk)
    // {
    thread_sched(TRUE);
    // arch_to_sche();
    // }
    // mm_trace();
    printk("release tk %x, name is:%s\n", tk, kobject_get_name(&tk->kobj));
}
void task_kill(task_t *tk)
{
    kobj_del_list_t kobj_list;
    kobj_del_list_init(&kobj_list);
    obj_unmap(&tk->obj_space, vpage_create3(KOBJ_DELETE_RIGHT, 0, TASK_PROT), &kobj_list);
    kobj_del_list_to_do(&kobj_list);
    thread_sched(TRUE);
    // arch_to_sche();
}
task_t *task_create(ram_limit_t *lim, int is_knl)
{
    task_t *tk = NULL;
#if IS_ENABLED(CONFIG_BUDDY_SLAB)
    tk = mm_limit_alloc_slab(task_slab, lim);
#else
    tk = mm_limit_alloc(lim, sizeof(task_t));
#endif

    if (!tk)
    {
        return NULL;
    }
    task_init(tk, lim, is_knl);
    // printk("create task is 0x%x\n", tk);
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
 * @brief 工厂注册函数
 *
 */
void task_factory_register(void)
{
    factory_register(task_create_func, TASK_PROT);
}
INIT_KOBJ(task_factory_register);
