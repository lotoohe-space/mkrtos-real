#include "types.h"
#include "kobject.h"
#include "init.h"
#include "mm_page.h"
#include "thread.h"
#include "task.h"
#include "globals.h"
typedef struct mm_man
{
    kobject_t kobj;
} mm_man_t;
static mm_man_t mm_man;

static void mm_man_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f);

static void mm_man_reg(void)
{
    kobject_init(&mm_man.kobj);
    mm_man.kobj.invoke_func = mm_man_syscall;
    global_reg_kobj(&mm_man.kobj, MM_PROT);
}
INIT_KOBJ(mm_man_reg);

enum mm_op
{
    MM_ALLOC,
    MM_FREE,
    MM_MOD_ATTRS,
};

static void mm_man_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    task_t *cur_task = thread_get_current_task();
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);

    if (sys_p.prot != MM_PROT)
    {
        f->r[0] = msg_tag_init4(0, 0, 0, -EPROTO).raw;
        return;
    }
    switch (sys_p.op)
    {
    case MM_ALLOC:
    {
        addr_t ret_addr;
        int ret = mm_pages_alloc_page(&cur_task->mm_space.mm_pages, cur_task->lim, f->r[1], &ret_addr, f->r[2]);
        if (ret < 0)
        {
            tag = msg_tag_init4(0, 0, 0, ret);
        }
        else
        {
            tag = msg_tag_init4(0, 0, 0, 0);
            f->r[1] = ret_addr;
        }
    }
    break;
    case MM_FREE:
    {
        mm_pages_free_page(&cur_task->mm_space.mm_pages, cur_task->lim, f->r[1], f->r[2]);
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    case MM_MOD_ATTRS:
    {
        tag = msg_tag_init4(0, 0, 0, -ENOSYS); // TODO:
    }
    break;
    default:
        tag = msg_tag_init4(0, 0, 0, -ENOSYS);
        break;
    }
    f->r[0] = tag.raw;
}
void mm_man_dump(void)
{
}
