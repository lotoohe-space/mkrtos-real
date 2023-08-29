#pragma once

#include "u_types.h"

msg_tag_t thread_msg_bug_set(obj_handler_t obj, void *msg);
msg_tag_t thread_msg_bug_get(obj_handler_t obj, umword_t *msg, umword_t *len);
msg_tag_t thread_exec_regs(obj_handler_t obj, umword_t pc, umword_t sp, umword_t ram);
msg_tag_t thread_run(obj_handler_t obj);
msg_tag_t thread_bind_task(obj_handler_t obj, obj_handler_t tk_obj);
