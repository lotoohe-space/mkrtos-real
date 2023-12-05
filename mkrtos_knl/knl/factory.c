/**
 * @file factory.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "factory.h"
#include "kobject.h"
#include "prot.h"
#include "assert.h"
#include "mm_wrap.h"
#include "globals.h"
#include "task.h"
#include "map.h"
#include "init.h"
#include "globals.h"
#include "thread.h"
/**
 * @brief factory的操作码
 *
 */
enum
{
    FACTORY_CREATE_KOBJ
};
/**
 * @brief 根factory
 *
 */
static factory_t root_factory;
/**
 * @brief 内核对象注册的factory函数
 *
 */
static factory_func factory_func_list[FACTORY_FUNC_MAX];

/**
 * @brief 在系统初始化时调用，初始化factoyr的内存
 *
 */
static void factory_mem_init(void)
{
    // Nothing.
}
INIT_KOBJ_MEM(factory_mem_init);
/**
 * @brief 注册一个内核对象的工厂函数
 *
 * @param func 注册的工厂函数
 * @param inx 注册的工厂函数类型
 * @pre inx>=0 && inx < FACTORY_FUNC_MAX && factory_func_list[inx] == NULL
 */
void factory_register(factory_func func, int inx)
{
    assert(inx >= 0);
    assert(inx < FACTORY_FUNC_MAX);
    assert(factory_func_list[inx] == NULL);
    factory_func_list[inx] = func;
}
/**
 * @brief 生成一个内核对象该接口调用注册的内核对象函数创建一个内核对象。
 *
 * @param kobj 函数的调用者
 * @param lim 内存限额
 * @param f 系统调用传递的寄存器参数
 * @return kobject_t* ==NULL 创建失败 !=NULL创建成功
 */
static kobject_t *factory_manu_kobj(kobject_t *kobj, ram_limit_t *lim, entry_frame_t *f)
{
    kobject_t *new_kobj = NULL;

    if (f->r[1] < 0 || f->r[1] >= FACTORY_FUNC_MAX)
    {
        return NULL;
    }
    if (factory_func_list[f->r[1]])
    {
        new_kobj = factory_func_list[f->r[1]](lim, f->r[3], f->r[4], f->r[5], f->r[6]);
    }
    return new_kobj;
}
/**
 * @brief 创建一个内核对象并map到进程
 *
 * @param kobj 当前的factory
 * @param tk 映射到那个task
 * @param f 系统调用参数
 * @return msg_tag_t 参见 @see msg_tag_t
 */
static msg_tag_t factory_create_map(kobject_t *kobj, task_t *tk, entry_frame_t *f)
{
    vpage_t page = vpage_create_raw(f->r[2]);
    mword_t status = spinlock_lock(&tk->kobj.lock);

    if (status < 0)
    {
        return msg_tag_init4(0, 0, 0, -EINVAL);
    }
    kobject_t *new_kobj = factory_manu_kobj(kobj, tk->lim, f);

    if (!new_kobj)
    {
        spinlock_set(&tk->kobj.lock, status);
        return msg_tag_init4(0, 0, 0, -ENOMEM);
    }

    page.attrs |= KOBJ_ALL_RIGHTS;
    if (obj_map_root(new_kobj, &tk->obj_space, tk->lim, page) == FALSE)
    {
        if (new_kobj->kobj_type == THREAD_TYPE)
        {
            mm_limit_free_align(tk->lim, new_kobj, THREAD_BLOCK_SIZE);
        }
        else
        {
            mm_limit_free(tk->lim, new_kobj);
        }
        spinlock_set(&tk->kobj.lock, status);
        return msg_tag_init4(0, 0, 0, -ENOMEM);
    }
    spinlock_set(&tk->kobj.lock, status);
    return msg_tag_init4(0, 0, 0, 0);
}
/**
 * @brief factory的系统调用函数
 *
 * @param kobj
 * @param ram
 * @param f
 * @return msg_tag_t
 */
static void
factory_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);
    task_t *task = thread_get_current_task();

    if (sys_p.prot != FACTORY_PROT)
    {
        f->r[0] = msg_tag_init4(0, 0, 0, -EPROTO).raw;
        return;
    }
    switch (sys_p.op)
    {
    case FACTORY_CREATE_KOBJ:
    {
        tag = factory_create_map(kobj, task, f);
    }
    break;
    }

    f->r[0] = tag.raw;
}
/**
 * @brief factory对象的初始化函数
 *
 * @param fac 初始化的factory对象
 * @param max 最大限额值
 */
static void factory_init(factory_t *fac, umword_t max)
{
    kobject_init(&fac->kobj, FACTORY_TYPE);
    ram_limit_init(&fac->limit, max);
    fac->kobj.invoke_func = factory_syscall;
}
/**
 * @brief 创建并初始化一个工厂对象
 *
 * @param lim
 * @param max
 * @return factory_t*
 */
static factory_t *fatory_create(ram_limit_t *lim, umword_t max)
{
    factory_t *kobj = mm_limit_alloc(lim, sizeof(factory_t));

    if (!kobj)
    {
        return NULL;
    }
    factory_init(kobj, max);

    return kobj;
}
/**
 * @brief factory的工厂函数，该函数创建一个工厂对象
 *
 * @param lim
 * @param arg0
 * @param arg1
 * @param arg2
 * @param arg3
 * @return kobject_t*
 */
static kobject_t *factory_create_func(ram_limit_t *lim, umword_t arg0, umword_t arg1,
                                      umword_t arg2, umword_t arg3)
{
    factory_t *kobj = fatory_create(lim, arg0);

    if (!kobj)
    {
        return NULL;
    }

    return &kobj->kobj;
}
/**
 * @brief 初始化一个根工厂对象
 *
 */
static void root_factory_init(void)
{
    factory_init(&root_factory, 0);
    global_reg_kobj(&root_factory.kobj, FACTORY_PROT);
}
INIT_KOBJ(root_factory_init);
/**
 * @brief 获取根工厂对象
 *
 * @return factory_t*
 */
factory_t *root_factory_get(void)
{
    return &root_factory;
}
/**
 * @brief 工厂注册函数
 *
 */
static void factory_factory_register(void)
{
    factory_register(factory_create_func, FACTORY_PROT);
}
INIT_KOBJ(factory_factory_register);
