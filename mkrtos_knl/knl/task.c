#include "task.h"
#include "kobject.h"
#include "factory.h"
#include "init.h"

enum task_op_code
{
    TASK_OBJ_MAP,
};

static msg_tag_t task_syscall_func(entry_frame_t *f)
{
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

    default:
        break;
    }

    return tag;
}

void task_init(task_t *task, ram_limit_t *ram)
{
    assert(task);
    assert(ram);

    kobject_init(&task->kobj);
    obj_space_init(&task->obj_space, ram);
}

static task_t *task_create(ram_limit_t *lim)
{
    task_t *tk = mm_limit_alloc(lim, sizeof(task_t));

    if (!tk)
    {
        return NULL;
    }
    task_init(tk, lim);

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
    task_t *tk = task_create(lim);

    return &tk->kobj;
}

/**
 * @brief 工厂注册函数 TODO:
 *
 */
INIT_KOBJ void task_factory_register(void)
{
    factory_register(task_create_func, TASK_PROT);
}
