
#include <u_thread.h>
#include <u_thread_util.h>
#include <u_sleep.h>
#include <assert.h>
static ATTR_ALIGN(8) uint8_t stack_test[1024];
static uint8_t msg_buf[MSG_BUG_LEN];
static obj_handler_t pth;
static int test_cn;
static void thread_test_func(void)
{
    test_cn++;
    u_thread_del(pth);
}
AUTO_CALL(102)
static void thread_test_01(void)
{
    int old_test_cn = test_cn;
    assert(u_thread_create(&pth, (char *)stack_test + sizeof(stack_test), msg_buf, thread_test_func) >= 0);
    assert(u_thread_run(pth, 2) >= 0);
    u_sleep_ms(100);
    assert(test_cn > old_test_cn);
}

static ATTR_ALIGN(8) uint8_t stack2[1024];
static uint8_t msg_buf2[MSG_BUG_LEN];
static obj_handler_t pth2;

static void thread_test_func2(void)
{
    assert(u_thread_create(&pth, (char *)stack_test + sizeof(stack_test), msg_buf, thread_test_func) >= 0);
    assert(u_thread_run(pth, 2) >= 0);
    u_sleep_ms(100);
    assert(test_cn == 1);
    u_thread_del(pth2);
}
/**
 * @brief 测试优先级是否生效
 *
 */
static AUTO_CALL(102) void thread_test_02(void)
{
    test_cn = 0;
    assert(u_thread_create(&pth2, (char *)stack2 + sizeof(stack2), msg_buf2, thread_test_func2) >= 0);
    assert(u_thread_run(pth2, 3) >= 0);
    u_sleep_ms(100);
    while (test_cn == 0)
    {
        u_sleep_ms(1);
    }
    test_cn = 0;
}
static void thread_test_func3(void)
{
    assert(u_thread_create(&pth, (char *)stack_test + sizeof(stack_test), msg_buf, thread_test_func) >= 0);
    assert(u_thread_run(pth, 2) >= 0);
    assert(u_thread_run(pth, 3) >= 0);
    u_sleep_ms(100);
    assert(test_cn == 1);
    u_thread_del(pth2);
}
/**
 * @brief 测试优先级修改是否生效
 *
 */
static AUTO_CALL(102) void thread_test_03(void)
{
    test_cn = 0;
    assert(u_thread_create(&pth2, (char *)stack2 + sizeof(stack2), msg_buf2, thread_test_func3) >= 0);
    assert(u_thread_run(pth2, 3) >= 0);
    u_sleep_ms(100);
    while (test_cn == 0)
        ;
    test_cn = 0;
}

#define THREAD_TEST_NR 128
#define THREAD_TEST_03_SIZE 1024
static ATTR_ALIGN(8) uint8_t stack_array[THREAD_TEST_NR][THREAD_TEST_03_SIZE];
static uint8_t msgbuf_array[THREAD_TEST_NR][MSG_BUG_LEN];
static obj_handler_t th_array[THREAD_TEST_NR];
static void thread_test(void)
{
    while (1)
    {
        u_sleep_ms(1);
    }
}
/**
 * @brief 线程压力测试
 *
 */
static AUTO_CALL(102) void thread_test_04(void)
{
    for (int i = 0; i < THREAD_TEST_NR; i++)
    {
        assert(u_thread_create(&th_array[i], (char *)stack_array[i] + THREAD_TEST_03_SIZE,
                               msgbuf_array[i], thread_test) >= 0);
        assert(u_thread_run(th_array[i], 2) >= 0);
    }
    for (int i = 0; i < THREAD_TEST_NR; i++)
    {
        u_thread_del(th_array[i]);
    }
}
