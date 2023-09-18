#pragma once

#include "u_types.h"

msg_tag_t thread_msg_buf_set(obj_handler_t obj, void *msg);
msg_tag_t thread_msg_buf_get(obj_handler_t obj, umword_t *msg, umword_t *len);
msg_tag_t thread_exec_regs(obj_handler_t obj, umword_t pc, umword_t sp, umword_t ram, umword_t cp_stack);
msg_tag_t thread_run(obj_handler_t obj, uint8_t prio);
msg_tag_t thread_bind_task(obj_handler_t obj, obj_handler_t tk_obj);

#define thread_get_cur_ipc_msg()                \
    (                                           \
        {                                       \
            umword_t buf;                       \
            thread_msg_buf_get(-1, &buf, NULL); \
            ((ipc_msg_t *)buf);                 \
        })
