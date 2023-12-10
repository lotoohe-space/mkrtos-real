#pragma once
#include "u_prot.h"
#include "u_types.h"

enum obj_attrs
{
    KOBJ_DELETE_RIGHT = 1,
    KOBJ_ALL_RIGHTS = 3,
};
msg_tag_t task_set_pid(obj_handler_t dst_task, umword_t pid);
msg_tag_t task_get_pid(obj_handler_t dst_task, umword_t *data);
msg_tag_t task_obj_valid(obj_handler_t dst_task, obj_handler_t obj_inx, int *obj_type);
msg_tag_t task_map(obj_handler_t dst_task, obj_handler_t src_obj, obj_handler_t dst_obj, uint8_t attrs);
msg_tag_t task_unmap(obj_handler_t task_han, vpage_t vpage);
msg_tag_t task_alloc_ram_base(obj_handler_t task_han, umword_t size, addr_t *alloc_addr);
