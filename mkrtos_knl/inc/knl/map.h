#pragma once

#include "kobject.h"
#include "obj_space.h"
bool_t obj_map_root(kobject_t *kobj, obj_space_t *obj_space, ram_limit_t *ram, obj_addr_t addr);
int obj_map_src_dst(obj_space_t *dst_space, obj_space_t *src_space,
                    obj_handler_t dst_inx, obj_handler_t src_inx,
                    ram_limit_t *ram);
int obj_map(obj_space_t *obj_space, obj_handler_t inx, kobject_t *kobj, ram_limit_t *ram);
void obj_unmap(obj_space_t *obj_space, obj_handler_t inx, kobj_del_list_t *del_list);
void obj_unmap_all(obj_space_t *obj_space, kobj_del_list_t *del_list);
