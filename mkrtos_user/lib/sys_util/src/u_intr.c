
#include <u_types.h>
#include <u_hd_man.h>
#include <u_prot.h>
#include <u_factory.h>
#include <u_irq_sender.h>
#include <u_task.h>
#include <errno.h>
#include <u_thread_util.h>

int u_intr_bind(int irq_no, u_irq_prio_t prio, int th_prio,
                void *stack, void *msg_buf, void (*thread_func)(void), obj_handler_t *irq)
{
    obj_handler_t irq_obj = handler_alloc();
    obj_handler_t th_hd;
    msg_tag_t tag;
    int ret;

    if (irq_obj == HANDLER_INVALID)
    {
        return -ENOMEM;
    }
    tag = factory_create_irq_sender(FACTORY_PROT, vpage_create_raw3(0, 0, irq_obj));

    if (msg_tag_get_val(tag) < 0)
    {
        handler_free(irq_obj);
        return msg_tag_get_val(tag);
    }
    tag = uirq_bind(irq_obj, irq_no, prio);
    if (msg_tag_get_val(tag) < 0)
    {
        handler_free_umap(irq_obj);
        return msg_tag_get_val(tag);
    }
    if (thread_func)
    {
        ret = u_thread_create(&th_hd, (char *)stack, msg_buf, thread_func);

        if (ret < 0)
        {
            handler_free_umap(irq_obj);
            return ret;
        }
        if (irq)
        {
            *irq = irq_obj;
        }
        u_thread_run(th_hd, th_prio);
    }
    if (irq)
    {
        *irq = irq_obj;
    }

    return ret;
}
