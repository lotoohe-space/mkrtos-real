/*
 * 结合使用条件变量和互斥锁进行线程同步
 */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <CuTest.h>

static pthread_cond_t cond;
static pthread_mutex_t mutex;
static int cond_value;
static int quit;

static void *thread_signal(void *arg)
{
    while (!quit)
    {
        pthread_mutex_lock(&mutex);
        cond_value++;               // 改变条件，使条件满足
        pthread_cond_signal(&cond); // 给线程发信号
        printf("signal send, cond_value: %d\n", cond_value);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}

static void *thread_wait(void *arg)
{
    while (!quit)
    {
        pthread_mutex_lock(&mutex);

        /*通过while (cond is true)来保证从pthread_cond_wait成功返回时，调用线程会重新检查条件*/
        while (cond_value == 0)
            pthread_cond_wait(&cond, &mutex);

        cond_value--;
        printf("signal recv, cond_value: %d\n", cond_value);

        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}

static pthread_t tid1;
static pthread_t tid2;
static void pthread_cond_lock_test(CuTest *cu)
{

    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&tid1, NULL, thread_signal, NULL);
    pthread_create(&tid2, NULL, thread_wait, NULL);

    sleep(5);
    quit = 1;

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);

}
CuSuite *pthread_cond_lock_test_suite(void)
{
    CuSuite *suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, pthread_cond_lock_test);

    return suite;
}
