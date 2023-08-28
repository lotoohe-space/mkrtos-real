#pragma once
#include "u_prot.h"
#include "u_types.h"

msg_tag_t task_map(obj_handler_t dst_task, obj_handler_t src_obj, obj_handler_t dst_obj);
msg_tag_t task_unmap(obj_handler_t task_han, obj_handler_t obj);
msg_tag_t task_alloc_ram_base(obj_handler_t task_han, umword_t size, addr_t *alloc_addr);
