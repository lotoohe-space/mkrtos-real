#pragma once

#include "u_types.h"

typedef union u_irq_prio
{
    struct
    {
        uhmword_t prio_p;
        uhmword_t prio_s;
    };
    umword_t raw;
} u_irq_prio_t;

static inline u_irq_prio_t u_irq_prio_create(uhmword_t p, uhmword_t s)
{
    return (u_irq_prio_t){
        .prio_p = p,
        .prio_s = s,
    };
}

msg_tag_t uirq_bind(obj_handler_t obj_inx, umword_t irq_no, u_irq_prio_t prio_sub_pre);
msg_tag_t uirq_wait(obj_handler_t obj_inx, int flags);
msg_tag_t uirq_ack(obj_handler_t obj_inx, umword_t irq_no);