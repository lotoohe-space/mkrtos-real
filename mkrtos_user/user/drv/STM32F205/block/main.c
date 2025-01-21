
#include <stdio.h>
// #include <at32f435_437_conf.h>
#include "flash.h"
#include <assert.h>
#include <mk_dtb_parse.h>
#include <ns_cli.h>
#include <sys/stat.h>
#include <u_fast_ipc.h>
#include <u_sleep.h>
#include <u_sys.h>
#include <u_task.h>
#include <u_thread.h>
#define STACK_COM_ITME_SIZE (1024 + 512)
ATTR_ALIGN(8)
uint8_t stack_coms[STACK_COM_ITME_SIZE];
uint8_t msg_buf_coms[MSG_BUG_LEN];
void fast_ipc_init(void)
{
    u_fast_ipc_init(stack_coms, msg_buf_coms, 1, STACK_COM_ITME_SIZE);
}
void assert_param(int val)
{
    if (!val) {
        while (1)
            ;
    }
}
int main(int argc, char *argv[])
{
    obj_handler_t hd;
    int ret;
    task_set_obj_name(TASK_THIS, TASK_THIS, "tk_blk");
    task_set_obj_name(TASK_THIS, THREAD_MAIN, "th_blk");
    printf("%s init..\n", argv[0]);
    fast_ipc_init();
    printf("%s:%d\n", __func__, __LINE__);
    ret = flash_init();
    if (ret < 0) {
        printf("flash init error.\n");
        return -1;
    }
    printf("%s:%d\n", __func__, __LINE__);
    ret = flash_write(0x8000000 + 512 * 1024, 0x8000000 + 512 * 1024 + 4, "123");
    if (ret < 0) {
        printf("flash init error.\n");
        return -1;
    }
    printf("%s:%d\n", __func__, __LINE__);
    char *read_str = (char *)(0x8000000 + 512 * 1024);
    printf("read str is %s\n", read_str);
    while (1) {
        u_sleep_ms(1000);
    }
    // mk_drv_init();
    // mk_dev_init();
    // // drv_i2c_init();
    // dtb_parse_init();

    // ret = rpc_meta_init(THREAD_MAIN, &hd);
    // assert(ret >= 0);
    // fs_svr_init();
    // ns_register("/block", hd, FILE_NODE);
    // while (1)
    // {
    //     fs_svr_loop();
    // }
}
