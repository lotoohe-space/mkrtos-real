#include <assert.h>
#include <errno.h>
#include <u_factory.h>
#include <u_thread.h>
#include <u_types.h>
#include <u_prot.h>
#include <u_task.h>
#include <u_hd_man.h>
#include <u_thread_util.h>
void u_thread_del(obj_handler_t th_hd)
{
    task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, th_hd));
}
int u_thread_create(obj_handler_t *th_hd, void *stack, umword_t stack_size, void *msg_buf, void (*thread_func)(void))
{
    assert(th_hd);
    msg_tag_t tag;
    obj_handler_t th1_hd;

    th1_hd = handler_alloc();
    if (th1_hd == HANDLER_INVALID)
    {
        return -ENOENT;
    }

    tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, th1_hd));
    if (msg_tag_get_prot(tag) < 0)
    {
        handler_free(th1_hd);
        return msg_tag_get_prot(tag);
    }

    tag = thread_exec_regs(th1_hd, (umword_t)thread_func, (umword_t)stack + stack_size - sizeof(void *), RAM_BASE(), 0);
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
    if (msg_buf)
    {
        //!< 有些线程不需要msg_buf
        tag = thread_msg_buf_set(th1_hd, msg_buf);
        if (msg_tag_get_prot(tag) < 0)
        {
            handler_free_umap(th1_hd);
            return msg_tag_get_prot(tag);
        }
    }
    *th_hd = th1_hd;
    return 0;
}
void u_thread_run(obj_handler_t th_hd, int prio)
{
    thread_run(th_hd, prio);
}