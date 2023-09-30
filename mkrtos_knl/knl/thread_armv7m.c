/**
 * @file thread_armv7m.c
 * @author zhangzheng (135874329@qq.com)
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
#include "thread_armv7m.h"
#include "app.h"
#include "mm_wrap.h"
#include "arch.h"
#include "string.h"
syscall_entry_func syscall_handler_get(void)
{
    return syscall_entry;
}
void thread_knl_pf_set(thread_t *cur_th, void *pc)
{
    pf_t *cur_pf = ((pf_t *)((char *)cur_th + THREAD_BLOCK_SIZE)) - 1;

    cur_pf->pf_s.xpsr = 0x01000000L;
    cur_pf->pf_s.lr = (umword_t)NULL; //!< 线程退出时调用的函数
    cur_pf->pf_s.pc = (umword_t)pc | 0x1;
    // cur_pf->rg1[5] = (umword_t)0;

    cur_th->sp.knl_sp = (char *)cur_pf;
    cur_th->sp.user_sp = 0;
    cur_th->sp.sp_type = 0;
}
void thread_user_pf_set(thread_t *cur_th, void *pc, void *user_sp, void *ram, umword_t stack)
{
    // assert((((umword_t)user_sp) & 0x7UL) == 0);
    umword_t usp = ((umword_t)(user_sp - 8) & ~0x7UL);

    if (stack)
    {
        usp -= THREAD_MSG_BUG_LEN;
        memcpy((void *)usp, (void *)stack, THREAD_MSG_BUG_LEN);
    }

    pf_t *cur_pf = (pf_t *)(usp)-1; // thread_get_pf(cur_th);

    cur_pf->pf_s.xpsr = 0x01000000L;
    cur_pf->pf_s.lr = (umword_t)NULL; //!< 线程退出时调用的函数
    cur_pf->pf_s.pc = (umword_t)pc | 0x1;
    cur_pf->rg1[5] = (umword_t)ram;

    cur_th->sp.knl_sp = ((char *)cur_th + THREAD_BLOCK_SIZE - 8);
    cur_th->sp.user_sp = cur_pf;
    cur_th->sp.sp_type = 1;

    printk("exc_regs:%x %x %x\n", cur_pf->pf_s.pc, cur_th->sp.user_sp, ram);
}
