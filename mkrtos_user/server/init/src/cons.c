
#include <u_types.h>
#include <u_queue.h>
#include <u_util.h>
#include <u_thread.h>
#include <u_log.h>
#include <u_hd_man.h>
#include <u_err.h>
#include <u_sema.h>
#include <assert.h>
#include <errno.h>
#include <u_sleep.h>
#include <pthread.h>
#include <rpc_prot.h>
#include "cons_svr.h"
#include <stdio.h>
#include <u_thread_util.h>
#include <u_thread.h>
#include <string.h>

#if IS_ENABLED(CONFIG_MMU)
#define CONS_STACK_SIZE 1024
#else
#define CONS_STACK_SIZE 512
#endif
static ATTR_ALIGN(8) uint8_t cons_stack[CONS_STACK_SIZE];
// static uint8_t cons_msg_buf[MSG_BUG_LEN];
static cons_t cons_obj;
static obj_handler_t cons_th;
static obj_handler_t sem_th;
static void console_read_func(void)
{
    while (1)
    {
        int r_len = ulog_read_bytes(LOG_PROT, cons_obj.r_data_buf, sizeof(cons_obj.r_data_buf));

        if (r_len > 0)
        {
            pthread_spin_lock(&cons_obj.r_lock);
            for (int i = 0; i < r_len; i++)
            {
                q_enqueue(&cons_obj.r_queue, cons_obj.r_data_buf[i]);
            }
            if (sem_th)
            {
                u_sema_up(sem_th);
            }
            pthread_spin_unlock(&cons_obj.r_lock);
        }
    }
    handler_free_umap(cons_obj.hd_cons_read);
    while (1)
    {
        u_sleep_ms(1000);
    }
}

void console_init(void)
{
    cons_svr_obj_init(&cons_obj);
    meta_reg_svr_obj(&cons_obj.svr, CONS_PROT);
    u_thread_create(&cons_th, (char *)cons_stack + sizeof(cons_stack) - 8, NULL, console_read_func);
    u_thread_run(cons_th, 15);
    ulog_write_str(LOG_PROT, "cons svr init...\n");
}
/**
 * @brief 向控制台写入数据
 *
 * @param data
 * @param len
 */
int console_write(uint8_t *data, size_t len)
{
    // pid_t src_pid = thread_get_src_pid();

    // if (src_pid != cons_obj.active_pid)
    // {
    //     /*TODO:存储到文件或者通过其他方式*/
    //     return -EACCES;
    // }
    // pthread_mutex_lock(&cons_obj.w_lock);
    ulog_write_bytes(LOG_PROT, data, len);
    // pthread_mutex_unlock(&cons_obj.w_lock);

    return len;
}
/**
 * @brief 向控制台读取数据
 *
 * @param data
 * @param len 如果len为0，则返回数据长度。
 * @return int
 */
int console_read(uint8_t *data, size_t len)
{
    int r_len = 0;
    pid_t src_pid = thread_get_src_pid();

    if (src_pid != cons_obj.active_pid)
    {
        return -EACCES;
    }
    if (len == 0)
    {
        return q_queue_len(&cons_obj.r_queue);
    }
    if (q_queue_len(&cons_obj.r_queue) == 0)
    {
        // 回复没有消息
        return 0;
    }
    else
    {
        pthread_spin_lock(&cons_obj.r_lock);
        if (q_queue_len(&cons_obj.r_queue) == 0)
        {
            // 回复没有消息
            pthread_spin_unlock(&cons_obj.r_lock);
            return 0;
        }
        int i;
        for (i = 0; i < q_queue_len(&cons_obj.r_queue) && i < len; i++)
        {
            uint8_t e;
            if (q_dequeue(&cons_obj.r_queue, &e) < 0)
            {
                break;
            }
            data[i] = e;
        }
        r_len = i;
        pthread_spin_unlock(&cons_obj.r_lock);
    }
    return r_len;
}
/**
 * @brief 激活控制台为发送者进程
 *
 */
void console_active(pid_t pid, obj_handler_t sem)
{
    cons_obj.active_pid = pid;
    if (sem)
    {
        if (sem_th)
        {
            handler_free_umap(sem_th);
        }
        sem_th = sem;
    }
}
