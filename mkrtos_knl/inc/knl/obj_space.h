#pragma once

#include "kobject.h"
#include "ram_limit.h"
#include "assert.h"
#include "types.h"
#include "mm_wrap.h"

#define OBJ_MAP_TAB_SIZE 2
#define OBJ_MAP_ENTRY_SIZE 16

#define OBJ_MAP_MAX_ADDR ((OBJ_MAP_ENTRY_SIZE) * (OBJ_MAP_TAB_SIZE))

typedef struct obj_map_entry
{
    struct
    {
        kobject_t *obj;
        union
        {
            umword_t flags : 2;
            umword_t obj_al : sizeof(umword_t) - 2;
        };
    } kobj;
    slist_head_t node;
} obj_map_entry_t;

#define obj_map_kobj_get(kobj) ((kobject_t *)((((umword_t)(kobj)) >> 2) << 2))

static inline void obj_map_entry_init(obj_map_entry_t *entry, kobject_t *obj)
{
    slist_init(&entry->node);
    entry->kobj.obj = obj;
}

typedef struct obj_map_item
{
    obj_map_entry_t items[OBJ_MAP_ENTRY_SIZE];
} obj_map_item_t;

typedef struct obj_map_tab
{
    obj_map_item_t *tabs[OBJ_MAP_TAB_SIZE];
} obj_map_tab_t;

typedef struct obj_space
{
    obj_map_tab_t tab;
} obj_space_t;

static inline void obj_space_init(obj_space_t *obj_space, ram_limit_t *ram)
{
    for (int i = 0; i < OBJ_MAP_TAB_SIZE; i++)
    {
        obj_space->tab.tabs[i] = NULL;
    }
}
static inline void obj_space_del(obj_space_t *obj_space, obj_addr_t inx)
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
static inline obj_map_entry_t *obj_space_insert(obj_space_t *obj_space, ram_limit_t *ram, kobject_t *kobj, obj_addr_t inx)
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
static inline obj_map_entry_t *obj_space_lookup(obj_space_t *obj_space, obj_addr_t inx)
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