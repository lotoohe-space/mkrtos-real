/**
 * @file map.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "map.h"
#include "types.h"
#include "obj_space.h"
#include "kobject.h"
#include "assert.h"
#include "cpulock.h"
bool_t obj_map_root(kobject_t *kobj, obj_space_t *obj_space, ram_limit_t *ram, vpage_t page)
{
    kobj_del_list_t kobj_list;

    kobj_del_list_init(&kobj_list);
    if (obj_space_lookup_kobj(obj_space, vpage_get_obj_handler(page)))
    { //!< 已经存在则解除注释
        obj_unmap(obj_space, page, &kobj_list);
    }
    umword_t status = cpulock_lock();

    kobj_del_list_to_do(&kobj_list);
    cpulock_set(status);
    obj_map_entry_t *map = obj_space_insert(obj_space, ram, kobj, vpage_get_obj_handler(page), vpage_get_attrs(page));

    if (!map)
    {
        return FALSE;
    }
    kobj->mappable.map_cnt++;
    slist_add(&kobj->mappable.node, &map->node);
    return TRUE;
}
/**
 * @brief 从源映射到目的，如果目的中已经存在，则先解除目的映射然后在影视
 *
 * @param dst_space
 * @param src_space
 * @param dst_inx
 * @param src_inx
 * @param ram
 * @param del_attrs
 * @return int
 */
int obj_map_src_dst(obj_space_t *dst_space, obj_space_t *src_space,
                    obj_handler_t dst_inx, obj_handler_t src_inx,
                    ram_limit_t *ram, uint8_t del_attrs, kobj_del_list_t *del_list)
{
    obj_map_entry_t *entry_obj =
        obj_space_lookup(src_space, src_inx);
    if (!entry_obj)
    {
        return -ENOENT;
    }
    if (!entry_obj->obj)
    {
        return -ENOENT;
    }

    if (obj_space_lookup_kobj(dst_space, dst_inx))
    { //!< 已经存在则解除注释
        obj_unmap(dst_space, vpage_create3(0, 0, dst_inx), del_list);
    }

    return obj_map(dst_space, dst_inx, obj_map_entry_kobj_get(entry_obj),
                   ram, obj_map_entry_get_attr(entry_obj) & (~(del_attrs & 0x3UL)));
}
int obj_map(obj_space_t *obj_space, obj_handler_t inx, kobject_t *insert_kobj, ram_limit_t *ram, uint8_t attrs)
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

    obj_map_entry_t *map = obj_space_insert(obj_space, ram, insert_kobj, inx, attrs);

    if (!map)
    {
        return -ENOMEM;
    }
    insert_kobj->mappable.map_cnt++;
    slist_add(&insert_kobj->mappable.node, &map->node);
    return 0;
}
void obj_unmap(obj_space_t *obj_space, vpage_t vpage, kobj_del_list_t *del_list)
{
    obj_map_entry_t *entry = NULL;

    entry = obj_space_lookup(obj_space, vpage_get_obj_handler(vpage));
    if (!entry)
    {
        return;
    }
    kobject_t *kobj = obj_map_entry_kobj_get(entry);
    if (!kobj)
    {
        return;
    }
    if (kobj->unmap_func)
    {
        kobj->unmap_func(obj_space, kobj);
    }
    if ((vpage.attrs & KOBJ_DELETE_RIGHT) &&
        (obj_map_entry_get_attr(entry) & KOBJ_DELETE_RIGHT))
    {
        //!< 代表删除所有
        obj_map_entry_t *pos;

        slist_foreach_not_next(pos, &kobj->mappable.node, node)
        {
            obj_map_entry_t *next = slist_next_entry(pos, &kobj->mappable.node, node);
            slist_del(&pos->node);
            pos->obj = NULL;
            // 删除一个
            kobj->mappable.map_cnt--;
            if (kobj->mappable.map_cnt <= 0)
            {
                if (del_list)
                {
                    kobj_del_list_add(del_list, &kobj->del_node);
                }
            }
            pos = next;
        }
    }
    else
    {
        assert(!slist_is_empty(&entry->node));
        slist_del(&entry->node);
        entry->obj = NULL;
        // 删除一个
        kobj->mappable.map_cnt--;
        if (kobj->mappable.map_cnt <= 0)
        {
            if (del_list)
            {
                kobj_del_list_add(del_list, &kobj->del_node);
            }
        }
    }
}
void obj_unmap_all(obj_space_t *obj_space, kobj_del_list_t *del_list)
{
    assert(obj_space);
    for (umword_t i = 0; i < OBJ_MAP_MAX_ADDR; i++)
    {
        obj_unmap(obj_space, vpage_create3(KOBJ_DELETE_RIGHT /*执行删除操作*/, 0, i), del_list);
    }
}
