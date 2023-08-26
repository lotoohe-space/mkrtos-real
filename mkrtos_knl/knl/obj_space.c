
#include "obj_space.h"
#include "types.h"

void obj_space_init(obj_space_t *obj_space, ram_limit_t *ram)
{
    for (int i = 0; i < OBJ_MAP_TAB_SIZE; i++)
    {
        obj_space->tab.tabs[i] = NULL;
    }
}
void obj_space_del(obj_space_t *obj_space, obj_addr_t inx)
{
    assert(obj_space);
    if (inx > OBJ_MAP_MAX_ADDR)
    {
        return;
    }
    int tab_inx = inx / OBJ_MAP_ENTRY_SIZE;
    int entry_inx = inx % OBJ_MAP_ENTRY_SIZE;

    if (!obj_space->tab.tabs[tab_inx])
    {
        return;
    }
    obj_space->tab.tabs[tab_inx]->items[entry_inx].kobj.obj = NULL;
}
obj_map_entry_t *obj_space_insert(obj_space_t *obj_space, ram_limit_t *ram, kobject_t *kobj, obj_addr_t inx)
{
    assert(obj_space);
    assert(kobj);
    if (inx > OBJ_MAP_MAX_ADDR)
    {
        return NULL;
    }
    int tab_inx = inx / OBJ_MAP_ENTRY_SIZE;
    int entry_inx = inx % OBJ_MAP_ENTRY_SIZE;

    if (!obj_space->tab.tabs[tab_inx])
    {
        obj_space->tab.tabs[tab_inx] = mm_limit_alloc(ram, sizeof(obj_map_item_t));
        if (!obj_space->tab.tabs[tab_inx])
        {
            return NULL;
        }
    }
    obj_map_entry_t *entry = &obj_space->tab.tabs[tab_inx]->items[entry_inx];

    if (entry->kobj.obj == NULL)
    {
        slist_init(&entry->node);
    }
    entry->kobj.obj = kobj;
    return entry;
}
obj_map_entry_t *obj_space_lookup(obj_space_t *obj_space, obj_addr_t inx)
{
    assert(obj_space);
    if (inx > OBJ_MAP_MAX_ADDR)
    {
        return NULL;
    }
    int tab_inx = inx / OBJ_MAP_ENTRY_SIZE;
    int entry_inx = inx % OBJ_MAP_ENTRY_SIZE;

    if (!obj_space->tab.tabs[tab_inx])
    {
        return NULL;
    }

    return &obj_space->tab.tabs[tab_inx]->items[entry_inx];
}
