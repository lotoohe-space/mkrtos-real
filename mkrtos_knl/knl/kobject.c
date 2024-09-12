#include "kobject.h"
#include "string.h"
void kobject_set_name(kobject_t *kobj, const char *name)
{
    assert(kobj);
    strncpy(kobj->dbg.name, name, KOBJ_NAME_SIZE);
    kobj->dbg.name[KOBJ_NAME_SIZE - 1] = 0;
}
char *kobject_get_name(kobject_t *kobj)
{
    return kobj->dbg.name;
}
void kobject_invalidate(kobject_t *kobj)
{
    assert(!spinlock_is_invalidation(&kobj->lock));
    spinlock_invalidate(&kobj->lock);
}
/**
 * @brief 默认的系统调用接口
 *
 * @param kobj
 * @param sys_p
 * @param in_tag
 * @param f
 */
void kobject_invoke(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    /*TODO:*/
    f->regs[0] = msg_tag_init4(0, 0, 0, -ENOSYS).raw;
}
bool_t kobject_put(kobject_t *kobj)
{
    return TRUE;
}
void kobject_release_stage1(kobject_t *kobj)
{
}
void kobject_release_stage2(kobject_t *kobj)
{
}

void kobject_init(kobject_t *kobj, enum knl_obj_type type)
{
    kobj_map_init(&kobj->mappable);
    slist_init(&kobj->del_node);
    spinlock_init(&kobj->lock);
    kobj->invoke_func = kobject_invoke;
    kobj->put_func = kobject_put;
    kobj->stage_1_func = kobject_release_stage1;
    kobj->stage_2_func = kobject_release_stage2;
    kobj->kobj_type = type;
}
