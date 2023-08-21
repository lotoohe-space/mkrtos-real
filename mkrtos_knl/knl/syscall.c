#include "types.h"
#include "syscall.h"
#include "thread.h"
#include "task.h"
#include "obj_space.h"
#include "err.h"
int syscall_entry(entry_frame_t entry)
{
    msg_tag_t tag = msg_tag_init3(0, 0, -1);
    thread_t *th = thread_get_current();
    task_t *tk = thread_get_current_task();

    obj_map_entry_t *entry_obj =
        obj_space_lookup(&tk->obj_space, entry.r[7]);

    if (!entry_obj)
    {
        tag = msg_tag_init3(0, 0, -ENOENT);
        return tag.raw;
    }
    if (!entry_obj->kobj.obj)
    {
        tag = msg_tag_init3(0, 0, -ENOENT);
        return tag.raw;
    }
    kobject_t *kobj = obj_map_kobj_get(entry_obj->kobj.obj);

    if (kobj->invoke_func)
    {
        kobj->invoke_func(kobj, NULL, &entry);
    }

    return tag.raw;
}
