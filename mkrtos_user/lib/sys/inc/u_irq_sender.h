#pragma once

#include "u_types.h"

msg_tag_t uirq_bind(obj_handler_t obj_inx, umword_t irq_no, umword_t prio_sub_pre);
msg_tag_t uirq_wait(obj_handler_t obj_inx, int flags);
msg_tag_t uirq_ack(obj_handler_t obj_inx, umword_t irq_no);