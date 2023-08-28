#pragma once

#include "kobject.h"
#include "obj_space.h"
bool_t obj_map_root(kobject_t *kobj, obj_space_t *obj_space, ram_limit_t *ram, obj_addr_t addr);
void obj_unmap(obj_space_t *obj_space, obj_handler_t inx, kobj_del_list_t *del_list);
void obj_unmap_all(obj_space_t *obj_space, kobj_del_list_t *del_list);
