#include "u_log.h"
#include "u_prot.h"
#include "u_mm.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_ipc.h"
#include "u_hd_man.h"
#include <errno.h>

int thread_create(int prio, void *(*fn)(void *arg), umword_t stack_top, obj_handler_t *ret_hd)
{
    obj_handler_t th1_hd;
    th1_hd = handler_alloc();
    if (th1_hd == HANDLER_INVALID)
    {
        return -ENOMEM;
    }
    msg_tag_t tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, th1_hd));
    if (msg_tag_get_prot(tag) < 0)
    {
        return msg_tag_get_prot(tag);
    }
    tag = thread_exec_regs(th1_hd, (umword_t)fn, stack_top, TASK_RAM_BASE(), 0);
    if (msg_tag_get_prot(tag) < 0)
    {
        handler_free_umap(th1_hd);
        return msg_tag_get_prot(tag);
    }
    tag = thread_bind_task(th1_hd, TASK_THIS);
    if (msg_tag_get_prot(tag) < 0)
    {
        handler_free_umap(th1_hd);
        return msg_tag_get_prot(tag);
    }
    tag = thread_run(th1_hd, prio);
    if (msg_tag_get_prot(tag) < 0)
    {
        handler_free_umap(th1_hd);
        return msg_tag_get_prot(tag);
    }
    if (ret_hd)
    {
        *ret_hd = th1_hd;
    }
    return 0;
}
