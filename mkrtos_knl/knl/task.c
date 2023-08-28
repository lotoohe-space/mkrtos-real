#include "task.h"
#include "kobject.h"
#include "factory.h"
#include "init.h"
#include "map.h"
#include "thread.h"
enum task_op_code
{
    TASK_OBJ_MAP,
    TASK_OBJ_UNMAP,
};
static bool_t task_put(kobject_t *kobj);
static void task_release_stage1(kobject_t *kobj);
static void task_release_stage2(kobject_t *kobj);
static msg_tag_t task_syscall_func(kobject_t *kobj, ram_limit_t *ram, entry_frame_t *f)
{
    task_t *cur_task = thread_get_current_task();
    task_t *tag_task = container_of(kobj, task_t, kobj);
    msg_tag_t tag = msg_tag_init(f->r[0]);

    if (tag.prot != TASK_PROT)
    {
        return msg_tag_init3(0, 0, -EINVAL);
    }

    switch (tag.type)
    {
    case TASK_OBJ_MAP:
        /* code */
        break;
    case TASK_OBJ_UNMAP:
    {
        kobject_t *del_kobj;
        kobj_del_list_t kobj_list;

        umword_t status = cpulock_lock();

        kobj_del_list_init(&kobj_list);
        obj_unmap(&tag_task->obj_space, f->r[1], &kobj_list);
        kobj_del_list_to_do(&kobj_list);
        cpulock_set(status);
    }
    break;
    default:
        break;
    }

    return tag;
}

void task_init(task_t *task, ram_limit_t *ram, int is_knl)
{
    assert(task);
    assert(ram);

    kobject_init(&task->kobj);
    obj_space_init(&task->obj_space, ram);
    mm_space_init(&task->mm_space, is_knl);
    ref_counter_init(&task->ref_cn);
    ref_counter_inc(&task->ref_cn);
    task->lim = ram;
    task->kobj.invoke_func = task_syscall_func;
    task->kobj.put_func = task_put;
    task->kobj.stage_1_func = task_release_stage1;
    task->kobj.stage_2_func = task_release_stage2;
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

    kobj_del_list_init(&kobj_list);
    obj_unmap_all(&tk->obj_space, &kobj_list);
    kobj_del_list_to_do(&kobj_list);
}
static void task_release_stage2(kobject_t *kobj)
{
    task_t *tk = container_of(kobj, task_t, kobj);
    kobj_del_list_t kobj_list;
    task_t *cur_tk = thread_get_current_task();

    mm_limit_free(tk->lim, tk);
    if (cur_tk == tk)
    {
        thread_sched();
    }
    printk("release tk %x\n", tk);
}

task_t *task_create(ram_limit_t *lim, int is_knl)
{
    task_t *tk = mm_limit_alloc(lim, sizeof(task_t));

    if (!tk)
    {
        return NULL;
    }
    task_init(tk, lim, is_knl);

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
