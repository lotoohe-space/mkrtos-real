/**
 * @file thread_armv7m.c
 * @author ATShining (135874329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "types.h"
#include "thread.h"
#include "printk.h"
#include "app.h"
#include "mm_wrap.h"
#include "arch.h"
#include "string.h"
#include <asm/system.h>
#include <early_boot.h>
#include <task.h>
extern void ret_form_run(void);
syscall_entry_func syscall_handler_get(void)
{
    return syscall_entry;
}
void thread_knl_pf_set(thread_t *cur_th, void *pc)
{
    pf_t *pt = ((pf_t *)((char *)cur_th + THREAD_BLOCK_SIZE)) - 1;

    pt->pstate = PSR_MODE_EL2h;
    // pt->pc = (umword_t)pc;
    cur_th->sp.x19 = (umword_t)pc;
    cur_th->sp.x20 = 0 /*arg*/;
    cur_th->sp.pc = (mword_t)ret_form_run;
    cur_th->sp.sp = (umword_t)pt;
}
void thread_user_pf_set(thread_t *cur_th, void *pc, void *user_sp, void *ram, umword_t stack)
{
    pf_t *pt = ((pf_t *)((char *)cur_th + THREAD_BLOCK_SIZE)) - 1;

    pt->pstate = PSR_MODE_EL1h;
    pt->pc = (umword_t)pc;
    pt->sp = (umword_t)user_sp;
    cur_th->sp.x19 = 0;
    cur_th->sp.x20 = 0;
    cur_th->sp.pc = (mword_t)ret_form_run;
    cur_th->sp.sp = (umword_t)pt;
    cur_th->sp.u_sp = (umword_t)user_sp;
}
void task_knl_init(task_t *knl_tk)
{
    knl_tk->mm_space.mem_dir = *boot_get_pdir();
}
