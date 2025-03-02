#include "cons_cli.h"
#include "fs_rpc.h"
#include "ns_cli.h"
#include "u_env.h"
#include "u_hd_man.h"
#include "u_log.h"
#include "u_prot.h"
#include "u_rpc_svr.h"
#include "u_sleep.h"
#include <assert.h>
#include <ff.h>
#include <stdio.h>
#include <stdlib.h>
#include "u_hd_man.h"
#include <u_fast_ipc.h>

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

static FATFS fs;
static MKFS_PARM defopt = {FM_ANY, 0, 0, 0};

int main(int args, char *argv[])
{
    obj_handler_t hd;
    int ret;
    fast_ipc_init();

    ret = rpc_meta_init(TASK_THIS, &hd);
    assert(ret >= 0);
    fs_svr_init();
    ns_register("/mnt", hd, 0);

    FRESULT res = f_mount(&fs, "0:/", 1);

    if (res != FR_OK) {
        assert(sizeof(fs.win) >= FF_MAX_SS);
        res = f_mkfs("0:/", &defopt, (void *)(fs.win), FF_MAX_SS); // 第三个参数可以设置成NULL，默认使用heap memory
        if (res != FR_OK) {
            cons_write_str("f_mkfs err.\n");
            exit(-1);
        } else {
            res = f_mount(&fs, "0:/", 1);
            if (res != FR_OK) {
                cons_write_str("f_mount err.\n");
                exit(-1);
            }
        }
    }
    cons_write_str("fatfs mount success\n");

    while(1) {
        u_sleep_ms(U_SLEEP_ALWAYS);
    }
    return 0;
}
