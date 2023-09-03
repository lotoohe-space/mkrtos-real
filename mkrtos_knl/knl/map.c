#include "map.h"
#include "types.h"
#include "obj_space.h"
#include "kobject.h"
#include "assert.h"

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
int obj_map_src_dst(obj_space_t *dst_space, obj_space_t *src_space,
                    obj_handler_t dst_inx, obj_handler_t src_inx,
                    ram_limit_t *ram)
{
    kobject_t *source_kobj = obj_space_lookup_kobj(src_space, src_inx);

    if (!source_kobj)
    {
        return -ENOENT;
    }
    return obj_map(dst_space, dst_inx, source_kobj, ram);
}
int obj_map(obj_space_t *obj_space, obj_handler_t inx, kobject_t *insert_kobj, ram_limit_t *ram)
{
    obj_map_entry_t *entry = NULL;

    entry = obj_space_lookup(obj_space, inx);
    if (entry)
    {
        if (obj_map_kobj_get(entry->obj))
        {
            return -EACCES;
        }
    }

    obj_map_entry_t *map = obj_space_insert(obj_space, ram, insert_kobj, inx);

    if (!map)
    {
        return -ENOMEM;
    }
    insert_kobj->mappable.map_cnt++;
    slist_add(&insert_kobj->mappable.node, &map->node);
    return 0;
}
void obj_unmap(obj_space_t *obj_space, obj_handler_t inx, kobj_del_list_t *del_list)
{
    obj_map_entry_t *entry = NULL;

    entry = obj_space_lookup(obj_space, inx);
    if (!entry)
    {
        return;
    }
    kobject_t *kobj = obj_map_kobj_get(entry->obj);
    if (!kobj)
    {
        return;
    }
    slist_del(&entry->node);
    obj_space_del(obj_space, inx);
    kobj->mappable.map_cnt--;
    if (kobj->mappable.map_cnt <= 0)
    {
        if (del_list)
        {
            kobj_del_list_add(del_list, &kobj->del_node);
        }
    }
}
void obj_unmap_all(obj_space_t *obj_space, kobj_del_list_t *del_list)
{
    assert(obj_space);
    for (umword_t i = 0; i < OBJ_MAP_MAX_ADDR; i++)
    {
        obj_unmap(obj_space, i, del_list);
    }
}
