/**
 * @file syscall.c
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-11-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */
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
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -1);
    thread_t *th = thread_get_current();
    task_t *tk = thread_get_current_task();
    syscall_prot_t sys_p = syscall_prot_create_raw(entry.r[7]);

    kobject_t *kobj =
        obj_space_lookup_kobj(&tk->obj_space, sys_p.obj_inx);

    if (!kobj)
    {
        if (sys_p.self)
        {
            kobj = &th->kobj;
        }
        else
        {
            entry.r[0] = msg_tag_init4(0, 0, 0, -ENOENT).raw;
            goto end;
        }
    }

    if (kobj->invoke_func)
    {
        kobj->invoke_func(kobj, sys_p, msg_tag_init(entry.r[0]), &entry);
    }
end:;
    addr_t u_sp = arch_get_user_sp();
    pf_s_t *pf_a = (pf_s_t *)u_sp;

    pf_a->rg0[0] = entry.r[0];
    pf_a->rg0[1] = entry.r[1];
    pf_a->rg0[2] = entry.r[2];
    pf_a->rg0[3] = entry.r[3];
}
