
#include <stdio.h>
#include <u_vmam.h>
#include <at32f435_437_conf.h>
#include <u_sleep.h>
#include <u_sys.h>
#include <u_task.h>
#include <mk_dtb_parse.h>
#include <mk_dev.h>
#include <mk_drv.h>
#include "fs_rpc.h"
#include "ns_cli.h"
#include <assert.h>
#include <sys/stat.h>
#include <u_fast_ipc.h>

#include "mk_i2c_drv_impl.h"
#include "u_hd_man.h"
#define STACK_COM_ITME_SIZE (2048)
ATTR_ALIGN(8)
uint8_t stack_coms[STACK_COM_ITME_SIZE];
uint8_t msg_buf_coms[MSG_BUG_LEN];
static obj_handler_t com_th_obj;

void fast_ipc_init(void)
{
    com_th_obj = handler_alloc();
    assert(com_th_obj != HANDLER_INVALID);
    u_fast_ipc_init(stack_coms, msg_buf_coms, 1, STACK_COM_ITME_SIZE, &com_th_obj);
}
int main(int argc, char *argv[])
{
    obj_handler_t hd;
    int ret;
    task_set_obj_name(TASK_THIS, TASK_THIS, "tk_i2c2");
    task_set_obj_name(TASK_THIS, THREAD_MAIN, "th_i2c2");
    printf("%s init..\n", argv[0]);
    fast_ipc_init();
    // u_sleep_ms(500);
    mk_drv_init();
    mk_dev_init();
    drv_i2c_init();
    dtb_parse_init();

    ret = rpc_meta_init(TASK_THIS, &hd);
    assert(ret >= 0);
    fs_svr_init();
    // mkdir("/dev", 0777);
    ns_register("/i2c2", hd, 0);
    while (1)
    {
        u_sleep_ms(0);
    }
}
