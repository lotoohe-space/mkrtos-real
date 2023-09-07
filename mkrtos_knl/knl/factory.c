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
enum
{
    FACTORY_CREATE_KOBJ
};

static factory_t root_factory;

static factory_func factory_func_list[FACTORY_FUNC_MAX];

/**
 * @brief 在系统初始化时调用
 *
 */
void factory_mem_init(void)
{
    // Nothing.
}
INIT_KOBJ_MEM(factory_mem_init);
void factory_register(factory_func func, int inx)
{
    assert(inx >= 0);
    assert(inx < FACTORY_FUNC_MAX);
    assert(factory_func_list[inx] == NULL);
    factory_func_list[inx] = func;
}

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
static msg_tag_t factory_create_map(kobject_t *kobj, task_t *tk, entry_frame_t *f)
{
    kobject_t *new_kobj = factory_manu_kobj(kobj, tk->lim, f);

    if (!new_kobj)
    {
        return msg_tag_init4(0, 0, 0, -ENOMEM);
    }
    if (obj_map_root(new_kobj, &tk->obj_space, tk->lim, f->r[2]) == FALSE)
    {
        mm_limit_free(tk->lim, new_kobj);
        return msg_tag_init4(0, 0, 0, -ENOMEM);
    }
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
static void factory_init(factory_t *fac, umword_t max)
{
    kobject_init(&fac->kobj);
    ram_limit_init(&fac->limit, max);
    fac->kobj.invoke_func = factory_syscall;
}
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
 * @brief 该函数创建一个工厂对象
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
static void root_factory_init(void)
{
    factory_init(&root_factory, 0);
    global_reg_kobj(&root_factory.kobj, FACTORY_PROT);
}
INIT_KOBJ(root_factory_init);
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
