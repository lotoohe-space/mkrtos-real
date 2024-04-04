#pragma once

#include "kobject.h"
#include "ram_limit.h"
#include "assert.h"
#include "types.h"
#include "mm_wrap.h"
#include <arch.h>
// #define CONFIG_OBJ_MAP_TAB_SIZE 4
// #define CONFIG_OBJ_MAP_ENTRY_SIZE 8

#define OBJ_MAP_MAX_ADDR ((CONFIG_OBJ_MAP_ENTRY_SIZE) * (CONFIG_OBJ_MAP_TAB_SIZE))

enum obj_attrs
{
    KOBJ_DELETE_RIGHT = 1,
    KOBJ_ALL_RIGHTS = 3,
};

typedef struct obj_map_entry
{
    union
    {
        void *obj;
        struct
        {
            umword_t flags : 2;
            umword_t obj_al : sizeof(umword_t) - 2;
        };
    };
    slist_head_t node;
} obj_map_entry_t;

#define obj_map_kobj_get(kobj) ((kobject_t *)((((umword_t)(kobj)) >> 2) << 2))
#define obj_map_entry_kobj_get(entry) ((kobject_t *)((((umword_t)((entry)->obj)) >> 2) << 2))

static inline void obj_map_entry_init(obj_map_entry_t *entry, kobject_t *obj)
{
    slist_init(&entry->node);
    entry->obj = (void *)((umword_t)obj & (~3UL));
}
static inline void obj_map_entry_set_attr(obj_map_entry_t *entry, uint8_t attrs)
{
    entry->flags |= attrs & 0x3UL;
}
static inline uint8_t obj_map_entry_get_attr(obj_map_entry_t *entry)
{
    return entry->flags;
}

typedef struct obj_map_item
{
    obj_map_entry_t items[CONFIG_OBJ_MAP_ENTRY_SIZE];
} obj_map_item_t;

typedef struct obj_map_tab
{
    obj_map_item_t *tabs[CONFIG_OBJ_MAP_TAB_SIZE];
} obj_map_tab_t;

typedef struct obj_space
{
    obj_map_tab_t tab;
} obj_space_t;

void obj_space_init(obj_space_t *obj_space, ram_limit_t *ram);
void obj_space_release(obj_space_t *obj_space, ram_limit_t *ram);
void obj_space_del(obj_space_t *obj_space, obj_addr_t inx);
obj_map_entry_t *obj_space_insert(obj_space_t *obj_space, ram_limit_t *ram, kobject_t *kobj, obj_addr_t inx, uint8_t attrs);
obj_map_entry_t *obj_space_lookup(obj_space_t *obj_space, obj_addr_t inx);
kobject_t *obj_space_lookup_kobj(obj_space_t *obj_space, obj_addr_t inx);
kobject_t *obj_space_lookup_kobj_cmp_type(obj_space_t *obj_space, obj_addr_t inx, enum knl_obj_type obj_type);
