#pragma once

#include "kobject.h"
#include "obj_space.h"

bool_t obj_map_root(kobject_t *kobj, obj_space_t *obj_space, ram_limit_t *ram, vpage_t page);
int obj_map_src_dst(obj_space_t *dst_space, obj_space_t *src_space,
                    obj_handler_t dst_inx, obj_handler_t src_inx,
                    ram_limit_t *ram, uint8_t del_attrs, kobj_del_list_t *del_list);
int obj_map(obj_space_t *obj_space, obj_handler_t inx, kobject_t *insert_kobj, ram_limit_t *ram, uint8_t attrs);
void obj_unmap(obj_space_t *obj_space, vpage_t vpage, kobj_del_list_t *del_list);
void obj_unmap_all(obj_space_t *obj_space, kobj_del_list_t *del_list);
