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

void get_syscall_handler(void)
{
}
void thread_exit(void)
{
    printk("thread exit.\n");
    /*TODO:删除这个线程*/
}

void thread_to_user(void)
{
    pf_t *cur_pf = thread_get_current_pf();

    cur_pf->xpsr = 0x01000000L;
    cur_pf->lr = (umword_t)thread_exit; //!< 线程退出时调用的函数

    arch_set_user_sp((umword_t)cur_pf);
    thread_ready(thread_get_current(), TRUE);
}
