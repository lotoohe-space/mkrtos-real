#include "types.h"
#include "syscall.h"
#include "thread.h"
#include "task.h"
#include "obj_space.h"
#include "err.h"
#include "arch.h"
#include "thread.h"
void syscall_entry(entry_frame_t entry)
{
    msg_tag_t tag = msg_tag_init3(0, 0, -1);
    thread_t *th = thread_get_current();
    task_t *tk = thread_get_current_task();

    obj_map_entry_t *entry_obj =
        obj_space_lookup(&tk->obj_space, entry.r[7]);

    if (!entry_obj)
    {
        tag = msg_tag_init3(0, 0, -ENOENT);
        goto end;
    }
    if (!entry_obj->obj)
    {
        tag = msg_tag_init3(0, 0, -ENOENT);
        goto end;
    }
    kobject_t *kobj = obj_map_kobj_get(entry_obj->obj);

    if (kobj->invoke_func)
    {
        tag = kobj->invoke_func(kobj, NULL, &entry);
    }
end:
    ;
    addr_t u_sp = arch_get_user_sp();
    pf_s_t *pf_a = (pf_s_t *)u_sp;

    pf_a->rg0[0] = tag.raw;
    pf_a->rg0[1] = entry.r[1];
    pf_a->rg0[2] = entry.r[2];
    pf_a->rg0[3] = entry.r[3];
}
