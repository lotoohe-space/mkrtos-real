#include "u_irq_sender.h"
#include "u_factory.h"
#include "u_hd_man.h"
#include "u_prot.h"
#include "u_thread_util.h"
#define IRQ_THREAD_PRIO 4

int u_irq_request(int irq_no, void *stack, void *msg_buf, obj_handler_t *ret_irq_obj, void (*irq_func)(void), u_irq_prio_t prio)
{
    obj_handler_t irq_obj;

    irq_obj = handler_alloc();
    if (irq_obj == HANDLER_INVALID)
    {
        return -1;
    }
    msg_tag_t tag = factory_create_irq_sender(FACTORY_PROT, vpage_create_raw3(0, 0, irq_obj));
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
    obj_handler_t th_hd;

    if (ret_irq_obj)
    {
        *ret_irq_obj = irq_obj;
    }
    int ret = u_thread_create(&th_hd, stack, msg_buf, irq_func);
    if (ret < 0)
    {
        handler_free_umap(irq_obj);
        return ret;
    }
    u_thread_run(th_hd, IRQ_THREAD_PRIO);

    return ret;
}
