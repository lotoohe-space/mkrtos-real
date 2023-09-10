/**
 * @file futex_backend.c
 * @author zhangzheng (1358745329@qq.com)
 * @brief 该文件用ipc模拟一个futex锁
 * @version 0.1
 * @date 2023-09-09
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "u_ipc.h"
#include "u_factory.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_hd_man.h"
#include "u_log.h"
#include "futex_queue.h"
#include "u_ipc.h"
#include "u_sys.h"
#include <pthread.h>
#include <futex.h>
#include <assert.h>
#define FT_NR 16

static obj_handler_t hd_futex_ipc;
static obj_handler_t hd_thread;
static char futex_thread_msg_bug[MSG_BUG_LEN];
#define STACK_SIZE 1024
static __attribute__((aligned(8))) uint8_t futex_thread_stack[STACK_SIZE];

/**
 * @brief 存储锁信息
 *
 */
typedef struct futex_lock
{
    uint32_t *uaddr;   //!< 锁的地址
    futex_queue_t fqt; //!< 锁的等待队列
} futex_lock_t;

typedef struct futex
{
    futex_lock_t fl_list[FT_NR]; //!< 存储加锁的地址
} futex_t;

static futex_t ft;
static bool_t futex_find_thread(futex_queue_t *fq, obj_handler_t thread_hd)
{
    int find = 0;
    int len;

    len = fq_queue_len(fq);
    for (int i = 0; i < len; i++)
    {
        umword_t out;

        fq_dequeue(fq, &out);
        if (out == thread_hd)
        {
            find = 1;
        }
        fq_enqueue(fq, out);
    }
    return find;
}
/**
 * @brief 查找一个地址，如果已经有了，则加入等待，否则增加一个信息，并加入等待
 *
 * @param ft
 * @param uaddr
 * @param hd
 * @return futex_lock_t*
 */
static futex_lock_t *futex_set_addr(futex_t *ft, void *uaddr, obj_handler_t hd)
{
    int empty_inx = -1;

    for (int i = 0; i < FT_NR; i++)
    {
        if (ft->fl_list[i].uaddr == uaddr)
        {
            // ft->fl_list[i].uaddr = uaddr;
            if (!futex_find_thread(&ft->fl_list[i], hd))
            {
                fq_enqueue(&ft->fl_list[i].fqt, hd);
            }
            return &ft->fl_list[i];
        }
        else if (ft->fl_list[i].uaddr == NULL)
        {
            empty_inx = i;
        }
    }
    if (empty_inx != -1)
    {
        ft->fl_list[empty_inx].uaddr = uaddr;
        fq_enqueue(&ft->fl_list[empty_inx].fqt, hd);
        return &ft->fl_list[empty_inx];
    }
    return NULL;
}
static futex_lock_t *futex_find(futex_t *ft, void *uaddr)
{
    for (int i = 0; i < FT_NR; i++)
    {
        if (ft->fl_list[i].uaddr == uaddr)
        {
            return &ft->fl_list[i];
        }
    }
    return NULL;
}
void futex_svr_thread(void)
{
    ulog_write_str(LOG_PROT, "futex running..\n");
    while (1)
    {
        ipc_wait(hd_futex_ipc, 0);
        /*TODO:处理锁信息*/

        ipc_reply(hd_futex_ipc, msg_tag_init4(0, 0, 0, 0));
    }
}
int be_futex(uint32_t *uaddr, int futex_op, uint32_t val,
             const struct timespec *timeout, uint32_t uaddr2, uint32_t val3)
{
    obj_handler_t th_obj = 0;
    ipc_timeout_t to = ipc_timeout_create2(0, 0);
    umword_t total = 0;
    sys_info_t sys_info;

    if (timeout && !(futex_op & FUTEX_REQUEUE))
    {
        to = ipc_timeout_create2(timeout->tv_sec * 1000 + timeout->tv_nsec / 1000 / 1000, 0);
        total = timeout->tv_sec * 1000 + timeout->tv_nsec / 1000 / 1000;
    }
    umword_t st_val;
    ipc_msg_t *msg = (ipc_msg_t *)futex_thread_msg_bug;

    msg->msg_buf[0] = (umword_t)uaddr;
    msg->msg_buf[1] = (umword_t)futex_op;
    msg->msg_buf[2] = (umword_t)val;
    msg->msg_buf[3] = (umword_t)timeout /*or val2*/;
    msg->msg_buf[4] = (umword_t)uaddr2;
    msg->msg_buf[5] = (umword_t)val3;
_try_again:

    sys_read_info(SYS_PROT, &sys_info);
    st_val = sys_info.sys_tick;
    msg_tag_t tag = ipc_call(hd_futex_ipc, msg_tag_init4(0, 6, 0, 0), to);

    if (msg_tag_get_val(tag) == -EWTIMEDOUT)
    {
        umword_t en_val;

        sys_read_info(SYS_PROT, &sys_info);
        en_val = sys_info.sys_tick;
        if (en_val - st_val < total)
        {
            total = total - (en_val - st_val);

            to = ipc_timeout_create2(total, 0);
            goto _try_again;
        }
    }
    return msg_tag_get_val(tag);
}
void futex_init(void)
{
    msg_tag_t tag;
    hd_futex_ipc = handler_alloc();
    assert(hd_futex_ipc != HANDLER_INVALID);
    hd_thread = handler_alloc();
    assert(hd_thread != HANDLER_INVALID);

    for (int i = 0; i < FT_NR; i++)
    {
        fq_init(&ft.fl_list[i].fqt);
        ft.fl_list[i].uaddr = NULL;
    }

    tag = factory_create_thread(FACTORY_PROT, vpage_create_raw3(0, 0, hd_thread));
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_msg_buf_set(hd_thread, futex_thread_msg_bug);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_exec_regs(hd_thread, (umword_t)futex_svr_thread,
                           (umword_t)futex_thread_stack + STACK_SIZE, RAM_BASE(), 0);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_bind_task(hd_thread, TASK_THIS);
    assert(msg_tag_get_prot(tag) >= 0);
    tag = thread_run(hd_thread, 2);
    assert(msg_tag_get_prot(tag) >= 0);
}
