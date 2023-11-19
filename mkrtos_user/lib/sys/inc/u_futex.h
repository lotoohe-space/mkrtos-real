#pragma once

#include <u_types.h>

msg_tag_t futex_ctrl(obj_handler_t obj, uint32_t *uaddr, int futex_op, uint32_t val,
                     void *timeout, uint32_t uaddr2, uint32_t val3, int tid);
