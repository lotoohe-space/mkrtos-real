
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <shell_port.h>
#include <pm_cli.h>
#include <u_sleep.h>
#include <u_task.h>
#include <u_thread.h>
#include <u_env.h>
#include <pthread.h>
void *test_func(void *arg)
{
    msg_tag_t tag;
    ipc_msg_t *msg;

    tag = thread_msg_buf_get(-1, (void *)(&msg), NULL);
    strcpy((void *)msg->msg_buf, "hello");

    for (int i = 0; i < 10000; i++)
    {

        thread_ipc_fast_call(msg_tag_init4(0, 2, 0, 0), u_get_global_env()->ns_hd, 1111, 2222, 3333);
        // printf("%s\n", (void *)msg->msg_buf);
    }
    // while (1)
    // {
    //     u_sleep_ms(100000);
    // }
    return NULL;
}
pthread_t pth1;
pthread_t pth2;
pthread_t pth3;
pthread_t pth4;
void fast_ipc_test(void)
{
    pthread_create(&pth1, NULL, test_func, NULL);
    pthread_create(&pth2, NULL, test_func, NULL);
    pthread_create(&pth3, NULL, test_func, NULL);
    pthread_create(&pth4, NULL, test_func, NULL);
}

int main(int argc, char *args[])
{
    task_set_obj_name(TASK_THIS, TASK_THIS, "tk_sh");
    task_set_obj_name(TASK_THIS, THREAD_MAIN, "th_sh");
#if 0
    thread_run(-1, 3);
#endif
    fast_ipc_test();
    for (int i = 0; i < argc; i++)
    {
        printf("args[%d]:%s\n", i, args[i]);
    }
    userShellInit();
    return 0;
}
