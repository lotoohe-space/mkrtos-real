/**
 * @file obj_space.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "obj_space.h"
#include "types.h"
#include "mm_wrap.h"
#include "string.h"
#if IS_ENABLED(CONFIG_BUDDY_SLAB)
#include <buddy.h>
#endif
void obj_space_init(obj_space_t *obj_space, ram_limit_t *ram)
{
    for (int i = 0; i < CONFIG_OBJ_MAP_TAB_SIZE; i++)
    {
        obj_space->tab.tabs[i] = NULL;
    }
}
void obj_space_release(obj_space_t *obj_space, ram_limit_t *ram)
{
    for (int i = 0; i < CONFIG_OBJ_MAP_TAB_SIZE; i++)
    {
#if IS_ENABLED(CONFIG_BUDDY_SLAB)
        buddy_free(buddy_get_alloter(), obj_space->tab.tabs[i]);
#else
        mm_limit_free(ram, obj_space->tab.tabs[i]);
#endif
    }
}
void obj_space_del(obj_space_t *obj_space, obj_addr_t inx)
{
    assert(obj_space);
    if (inx >= OBJ_MAP_MAX_ADDR)
    {
        return;
    }
    int tab_inx = inx / CONFIG_OBJ_MAP_ENTRY_SIZE;
    int entry_inx = inx % CONFIG_OBJ_MAP_ENTRY_SIZE;

    if (!obj_space->tab.tabs[tab_inx])
    {
        return;
    }
    obj_space->tab.tabs[tab_inx]->items[entry_inx].obj = NULL;
}
obj_map_entry_t *obj_space_insert(obj_space_t *obj_space, ram_limit_t *ram, kobject_t *kobj, obj_addr_t inx, uint8_t attrs)
{
    assert(obj_space);
    assert(kobj);
    if (inx >= OBJ_MAP_MAX_ADDR)
    {
        return NULL;
    }
    int tab_inx = inx / CONFIG_OBJ_MAP_ENTRY_SIZE;
    int entry_inx = inx % CONFIG_OBJ_MAP_ENTRY_SIZE;

    if (!obj_space->tab.tabs[tab_inx])
    {
        obj_space->tab.tabs[tab_inx] =
#if IS_ENABLED(CONFIG_BUDDY_SLAB)
            buddy_alloc(buddy_get_alloter(), PAGE_SIZE);
#else
            mm_limit_alloc(ram, sizeof(obj_map_item_t));
#endif
        if (!obj_space->tab.tabs[tab_inx])
        {
            return NULL;
        }
        // memset(obj_space->tab.tabs[tab_inx], 0, sizeof(obj_map_item_t));
    }
    obj_map_entry_t *entry = &obj_space->tab.tabs[tab_inx]->items[entry_inx];

    if (entry->obj == NULL)
    {
        slist_init(&entry->node);
    }
    entry->obj = kobj;
    obj_map_entry_set_attr(entry, attrs);
    return entry;
}
obj_map_entry_t *obj_space_lookup(obj_space_t *obj_space, obj_addr_t inx)
{
    assert(obj_space);
    if (inx >= OBJ_MAP_MAX_ADDR)
    {
        return NULL;
    }
    int tab_inx = inx / CONFIG_OBJ_MAP_ENTRY_SIZE;
    int entry_inx = inx % CONFIG_OBJ_MAP_ENTRY_SIZE;

    if (!obj_space->tab.tabs[tab_inx])
    {
        return NULL;
    }

    return &obj_space->tab.tabs[tab_inx]->items[entry_inx];
}
kobject_t *obj_space_lookup_kobj(obj_space_t *obj_space, obj_addr_t inx)
{
    obj_map_entry_t *entry_obj =
        obj_space_lookup(obj_space, inx);
    if (!entry_obj)
    {
        return NULL;
    }
    if (!entry_obj->obj)
    {
        return NULL;
    }
    return obj_map_kobj_get(entry_obj->obj);
}
kobject_t *obj_space_lookup_kobj_cmp_type(obj_space_t *obj_space, obj_addr_t inx, enum knl_obj_type obj_type)
{
    kobject_t *kobj = obj_space_lookup_kobj(obj_space, inx);

    if (!kobj)
    {
        return NULL;
    }
    if (kobj->kobj_type != obj_type)
    {
        return NULL;
    }
    return kobj;
}