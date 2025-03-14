#pragma  once

#include "u_hd_man.h"
#include "u_irq_sender.h"
int u_irq_request(int irq_no, void *stack, void *msg_buf, obj_handler_t *irq_obj, void (*irq_func)(void), u_irq_prio_t prio);
