#include "map.h"
#include "types.h"
#include "obj_space.h"

bool_t obj_map_root(kobject_t *kobj, obj_space_t *obj_space, ram_limit_t *ram, obj_addr_t addr)
{
    obj_map_entry_t *map = obj_space_insert(obj_space, ram, kobj, addr);

    if (!map)
    {
        return FALSE;
    }
    kobj->mappable.map_cnt++;
    slist_add(&kobj->mappable.node, &map->node);
    return TRUE;
}
