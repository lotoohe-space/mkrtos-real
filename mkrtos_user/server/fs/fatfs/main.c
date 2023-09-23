#include "u_log.h"
#include "ns_cli.h"
#include "u_rpc_svr.h"
#include "u_prot.h"
#include "fs_rpc.h"
#include <ff.h>
#include <stdio.h>
#include <assert.h>
static FATFS fs;
static BYTE buff[512];
static MKFS_PARM defopt = {FM_ANY, 0, 0, 0};

int main(int args, char *argv[])
{
    FRESULT res = f_mount(&fs, "0:", 1);

    if (res != FR_OK)
    {
        res = f_mkfs("0:", &defopt, buff, 512); // 第三个参数可以设置成NULL，默认使用heap memory
        if (res != FR_OK)
        {
            printf("f_mkfs err.\n");
            while (1)
                ;
        }
        else
        {
            res = f_mount(&fs, "0:", 1);
            if (res != FR_OK)
            {
                printf("f_mount err.\n");
                while (1)
                    ;
            }
        }
    }
    printf("mount success\n");
    obj_handler_t ipc_hd;
    int ret = rpc_creaite_bind_ipc(THREAD_MAIN, NULL, &ipc_hd);
    assert(ret >= 0);
    ns_register("fs", ipc_hd);

    fs_svr_init(ipc_hd);
    fs_svr_loop();
    while (1)
        ;
    return -1;
}
