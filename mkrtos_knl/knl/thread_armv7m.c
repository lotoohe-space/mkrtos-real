/*
 * @Author: zhangzheng 1358745329@qq.com
 * @Date: 2023-08-18 15:03:16
 * @LastEditors: zhangzheng 1358745329@qq.com
 * @LastEditTime: 2023-08-18 16:22:11
 * @FilePath: /mkrtos-real/mkrtos_knl/knl/thread_armv7m.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
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

void thread_user_pf_set(thread_t *cur_th, void *pc, void *user_sp, void *ram, umword_t stack)
{
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
