#include <rtthread.h>
#include <malloc.h>
#include <stdio.h>
#include <malloc.h>
#include <u_sleep.h>
#include <u_sys.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
extern unsigned long get_thread_area(void);

rt_weak void *rt_calloc(rt_size_t count, rt_size_t size)
{
    return calloc(count, size);
}

void rt_assert_handler(const char *ex, const char *func, rt_size_t line)
{
    printf("%s,%s:%d", ex, func, line);
    assert(0);
}
rt_err_t rt_thread_mdelay(rt_int32_t ms)
{
    u_sleep_ms(ms);
    return 0;
}
int rt_kprintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
void *rt_malloc(rt_size_t size)
{
    return malloc(size);
}
void rt_free(void *ptr)
{
    free(ptr);
}
void *rt_realloc(void *ptr, rt_size_t newsize)
{
    return realloc(ptr, newsize);
}
void rt_schedule(void)
{
    u_sleep_ms(0);
}
static umword_t intr_status = 0;
rt_base_t rt_hw_interrupt_disable(void)
{
    rt_base_t old = intr_status;

    if (intr_status != 0)
    {
        while (intr_status != get_thread_area())
            ;
    }
    intr_status = get_thread_area();

    return old;
}
void rt_hw_interrupt_enable(rt_base_t level)
{
    intr_status = level;
}
rt_bool_t rt_hw_interrupt_is_disabled(void)
{
    return !!intr_status;
}

rt_uint8_t rt_interrupt_get_nest(void)
{
    return 0; /*TODO:在线程中断中使用*/
}

static int sche_lock_nest;
void rt_enter_critical(void)
{
    sche_lock_nest++;
}
void rt_exit_critical(void)
{
    sche_lock_nest--;
}
rt_uint16_t rt_critical_level(void)
{
    return sche_lock_nest;
}
rt_thread_t rt_thread_self(void)
{
    printf("[drv]%s:%d\n", __func__, __LINE__);
    return NULL; /*TODO:*/
}
rt_err_t rt_thread_suspend_with_flag(rt_thread_t thread, int suspend_flag)
{
    printf("[drv]%s:%d\n", __func__, __LINE__);

    return 0; /*TODO:*/
}
rt_err_t rt_thread_resume(rt_thread_t thread)
{
    printf("[drv]%s:%d\n", __func__, __LINE__);

    return 0; /*TODO:*/
}
rt_err_t rt_timer_start(rt_timer_t timer)
{
    printf("[drv]%s:%d\n", __func__, __LINE__);

    return 0; /*TODO:*/
}
rt_err_t rt_timer_stop(rt_timer_t timer)
{
    printf("[drv]%s:%d\n", __func__, __LINE__);

    return 0; /*TODO:*/
}
rt_err_t rt_timer_control(rt_timer_t timer, int cmd, void *arg)
{
    printf("[drv]%s:%d\n", __func__, __LINE__);

    return 0; /*TODO:*/
}
rt_tick_t rt_timer_next_timeout_tick(void)
{
    printf("[drv]%s:%d\n", __func__, __LINE__);

    return 0; /*TODO:*/
}
void rt_timer_check(void)
{
    printf("[drv]%s:%d\n", __func__, __LINE__);

    /*TODO:*/
}
void rt_object_init(struct rt_object *object,
                    enum rt_object_class_type type,
                    const char *name)
{
    printf("[drv]%s:%d\n", __func__, __LINE__);

    /*TODO:*/
}
rt_object_t rt_object_find(const char *name, rt_uint8_t type)
{
    printf("[drv]%s:%d\n", __func__, __LINE__);

    return NULL; /*TODO:*/
}
rt_uint8_t rt_object_get_type(rt_object_t object)
{
    return 0;
}
void rt_set_errno(rt_err_t no)
{
    errno = no;
}
rt_err_t rt_get_errno(void)
{
    return errno;
}
int *_rt_errno(void)
{
    return __errno_location();
}
const char *rt_strerror(rt_err_t error)
{
    return "errno.";
}
void rt_system_heap_init(void *begin_addr, void *end_addr)
{
    /*Nothing*/
}
char *rt_strdup(const char *s)
{
    return strdup(s);
}
int rt_snprintf(char *buf, rt_size_t size, const char *format, ...)
{
    int ret;
    va_list args;
    va_start(args, format);
    ret = vsnprintf(buf, size, format, args);
    va_end(args);
    return ret;
}
rt_size_t rt_strnlen(const char *s, rt_ubase_t maxlen)
{
    return strnlen(s, maxlen);
}

rt_err_t rt_mutex_init(rt_mutex_t mutex, const char *name, rt_uint8_t flag)
{
    int ret = pthread_mutex_init(&mutex->lock, NULL);

    return ret;
}

rt_err_t rt_mutex_take(rt_mutex_t mutex, rt_int32_t timeout)
{
    return pthread_mutex_lock(&mutex->lock);
}
rt_err_t rt_mutex_trytake(rt_mutex_t mutex)
{
    return pthread_mutex_trylock(&mutex->lock);
}
rt_err_t rt_mutex_release(rt_mutex_t mutex)
{
    return pthread_mutex_unlock(&mutex->lock);
}

rt_mq_t rt_mq_create(const char *name,
                     rt_size_t msg_size,
                     rt_size_t max_msgs,
                     rt_uint8_t flag)
{
    /*TODO:*/

    return NULL;
}
rt_err_t rt_mq_delete(rt_mq_t mq)
{
    /*TODO:*/
    return -1;
}
struct rt_object_information *
rt_object_get_information(enum rt_object_class_type type)
{
    /*TODO:*/
    return NULL;
}