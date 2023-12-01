
#include <u_types.h>
#include <u_queue.h>
#include <u_util.h>
#include <pthread.h>
#include <u_thread.h>
#include <u_log.h>
#include <assert.h>
#include <errno.h>
#define CONS_WRITE_BUF_SIZE (128 + 1)

static queue_t r_queue;
static uint8_t r_data[CONS_WRITE_BUF_SIZE];
static pthread_mutex_t r_lock;
static pthread_mutex_t w_lock;
static pid_t active_pid = -1;
AUTO_CALL(101)
static void console_init(void)
{
    q_init(&r_queue, r_data, CONS_WRITE_BUF_SIZE);
    pthread_mutex_init(&r_lock, NULL);
    pthread_mutex_init(&w_lock, NULL);
}
/**
 * @brief 向控制台写入数据
 *
 * @param data
 * @param len
 */
int console_write(uint8_t *data, size_t len)
{
    pid_t src_pid = thread_get_src_pid();

    if (src_pid != active_pid)
    {
        /*TODO:存储到文件或者通过其他方式*/
        return -EACCES;
    }
    pthread_mutex_lock(&w_lock);
    ulog_write_bytes(LOG_PROT, data, len);
    pthread_mutex_unlock(&w_lock);

    return len;
}
/**
 * @brief 向控制台读取数据
 *
 * @param data
 * @param len
 * @return int
 */
int console_read(uint8_t *data, size_t len)
{
    int r_len;
    pid_t src_pid = thread_get_src_pid();

    if (src_pid != active_pid)
    {
        return -EACCES;
    }

    r_len = ulog_read_bytes(LOG_PROT, data, len);
    return r_len;
}
/**
 * @brief 激活控制台为发送者进程
 *
 */
void console_active(void)
{
    active_pid = thread_get_src_pid();
}
