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
#include <u_thread.h>
#include <u_ipc.h>

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
    ipc_msg_t *i_msg;

    thread_msg_buf_get(-1, (umword_t *)(&i_msg), NULL);

    return (rt_thread_t)(i_msg->user[1]);
}
rt_err_t rt_thread_bind_mkrtos(rt_thread_t th)
{
    ipc_msg_t *i_msg;

    pthread_mutex_lock(&th->suspend_lock);
    th->tlist.next = th->tlist.prev = &th->tlist;
    th->stat = 0;

    thread_msg_buf_get(-1, (umword_t *)(&i_msg), NULL);
    i_msg->user[1] = (umword_t)th;
}
rt_err_t rt_thread_suspend_with_flag(rt_thread_t thread, int suspend_flag)
{
    //! 这里锁两次，第二次加锁将会导致挂起
    printf("[drv]%s:%d\n", __func__, __LINE__);
    thread->stat = RT_THREAD_SUSPEND;
    pthread_mutex_lock(&thread->suspend_lock);
    thread->stat = 0;
    return 0;
}
rt_err_t rt_thread_resume(rt_thread_t thread)
{
    pthread_mutex_unlock(&thread->suspend_lock);
    pthread_mutex_lock(&thread->suspend_lock);
    printf("[drv]%s:%d\n", __func__, __LINE__);
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
#include <u_thread_util.h>
#include <u_hd_man.h>
#include <u_ipc.h>
#include <u_sys.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#define STACK_SIZE 1024

static obj_handler_t timer_hd = HANDLER_INVALID;
static char timer_thmsg_buf[MSG_BUG_LEN];
static __attribute__((aligned(8))) uint8_t timer_stack[STACK_SIZE];

#define TIMER_LIST_NR 16 //!< TODO:放到kconfig中
static struct rt_timer *timer_list[TIMER_LIST_NR];
static pthread_spinlock_t timer_lock;

static struct rt_timer *timer_alloc(struct rt_timer *tim, void (*func_cb)(void *arg), size_t exp_times, void *data, uint8_t flags)
{
    pthread_spin_lock(&timer_lock);
    for (int i = 0; i < TIMER_LIST_NR; i++)
    {
        if (timer_list[i] == 0)
        {
            timer_list[i] = tim;
            timer_list[i]->inx = i;
            timer_list[i]->data = data;
            timer_list[i]->func_cb = func_cb;
            timer_list[i]->exp_times = exp_times;
            timer_list[i]->flags = flags;
            pthread_spin_unlock(&timer_lock);
            return timer_list[i];
        }
    }
    pthread_spin_unlock(&timer_lock);
    return NULL;
}
static void timer_free(struct rt_timer *entry)
{
    assert(entry);
    timer_list[entry->inx] = NULL;
}
static void timer_func(void)
{
    while (1)
    {
        usleep(1000);
        rt_timer_check();
    }
}
void rt_timer_check(void)
{
    umword_t now_tick;

    now_tick = sys_read_tick();
    pthread_spin_lock(&timer_lock);
    for (int i = 0; i < TIMER_LIST_NR; i++)
    {
        if (timer_list[i] &&
            timer_list[i]->start_times != 0 &&
            timer_list[i]->exp_times + timer_list[i]->start_times >= now_tick)
        {
            if ((timer_list[i]->flags & RT_TIMER_FLAG_ONE_SHOT) == 0)
            {
                pthread_spin_unlock(&timer_lock);
                timer_list[i]->func_cb(timer_list[i]->data);
                pthread_spin_lock(&timer_lock);
                timer_list[i] = NULL;
            }
            else if (timer_list[i]->flags & RT_TIMER_FLAG_PERIODIC)
            {
                pthread_spin_unlock(&timer_lock);
                timer_list[i]->func_cb(timer_list[i]->data);
                pthread_spin_lock(&timer_lock);
                if (timer_list[i])
                {
                    timer_list[i]->start_times = sys_read_tick();
                }
            }
            else
            {
                /*Error.*/
            }
        }
    }
    pthread_spin_unlock(&timer_lock);
}
void rt_timer_init(rt_timer_t timer,
                   const char *name,
                   void (*timeout)(void *parameter),
                   void *parameter,
                   rt_tick_t time,
                   rt_uint8_t flag)
{
    assert(timeout);
    if (timer_hd == HANDLER_INVALID)
    {
        int ret = u_thread_create(&timer_hd, timer_stack, STACK_SIZE, timer_thmsg_buf, timer_func, 2);

        if (ret < 0)
        {
            errno = ret;
            return;
        }
    }
    struct rt_timer *entry = timer_alloc(timer, timeout, time, parameter, flag);

    if (!entry)
    {
        errno = -ENOMEM;
        return;
    }
    printf("%s name %s tick %d flag 0x%x.\n", name, time, flag);
}
rt_err_t rt_timer_start(rt_timer_t timer)
{
    timer->start_times = sys_read_tick();
    return 0;
}
rt_err_t rt_timer_stop(rt_timer_t timer)
{
    timer_free(timer);
    return 0;
}
rt_err_t rt_timer_control(rt_timer_t timer, int cmd, void *arg)
{
    assert(timer);
    switch (cmd)
    {
    case RT_TIMER_CTRL_SET_TIME:
        timer->exp_times = *(rt_tick_t *)arg;
        break;
    case RT_TIMER_CTRL_GET_TIME:
        *(rt_tick_t *)arg = timer->exp_times;
        break;
    case RT_TIMER_CTRL_GET_PARM:
        *(void **)arg = timer->data;
        break;
    case RT_TIMER_CTRL_SET_PARM:
        timer->data = arg;
        break;
    case RT_TIMER_CTRL_GET_FUNC:
        *(void **)arg = (void *)timer->func_cb;
        break;

    case RT_TIMER_CTRL_SET_FUNC:
        timer->func_cb = (void (*)(void *))arg;
        break;
    case RT_TIMER_CTRL_GET_REMAIN_TIME:
        *(rt_tick_t *)arg = timer->exp_times - (sys_read_tick() - timer->start_times);
        break;
    case RT_TIMER_CTRL_GET_STATE:
        if (timer->start_times)
        {
            /*timer is start and run*/
            *(rt_uint32_t *)arg = RT_TIMER_FLAG_ACTIVATED;
        }
        else
        {
            /*timer is stop*/
            *(rt_uint32_t *)arg = RT_TIMER_FLAG_DEACTIVATED;
        }
        break;
    case RT_TIMER_CTRL_SET_ONESHOT:
        timer->flags &= ~RT_TIMER_FLAG_PERIODIC;
        break;
    case RT_TIMER_CTRL_SET_PERIODIC:
        timer->flags |= RT_TIMER_FLAG_PERIODIC;
        break;
    default:
        return -EINVAL;
    }

    return 0;
}
rt_tick_t rt_timer_next_timeout_tick(void)
{
    pthread_spin_lock(&timer_lock);
    umword_t sys_tick;
    int min = INT32_MAX;

    sys_tick = sys_read_tick();
    for (int i = 0; i < TIMER_LIST_NR; i++)
    {
        if (timer_list[i] &&
            timer_list[i]->start_times != 0)
        {
            int val = timer_list[i]->exp_times + timer_list[i]->start_times - sys_tick;

            val = val < 0 ? 0 : val;
            if (val < min)
            {
                min = val;
            }
        }
    }
    pthread_spin_unlock(&timer_lock);
    return min;
}
