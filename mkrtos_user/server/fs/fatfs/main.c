#include "u_log.h"
#include "ns_cli.h"
#include "u_rpc_svr.h"
#include "u_prot.h"
#include "u_env.h"
#include "u_drv.h"
#include "cons_cli.h"
#include "fs_rpc.h"
#include <ff.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
static FATFS fs;
static MKFS_PARM defopt = {FM_ANY, 0, 0, 0};

int main(int args, char *argv[])
{
    obj_handler_t ipc_hd;
    int ret = rpc_creaite_bind_ipc(THREAD_MAIN, NULL, &ipc_hd);
    assert(ret >= 0);
    ns_register("/mnt", ipc_hd);

    fs_svr_init(ipc_hd);
    FRESULT res = f_mount(&fs, "0:", 1);

    if (res != FR_OK)
    {
        assert(sizeof(fs.win) >= FF_MAX_SS);
        res = f_mkfs("0:", &defopt, (void *)(fs.win), FF_MAX_SS); // 第三个参数可以设置成NULL，默认使用heap memory
        if (res != FR_OK)
        {
            cons_write_str("f_mkfs err.\n");
            exit(-1);
        }
        else
        {
            res = f_mount(&fs, "0:", 1);
            if (res != FR_OK)
            {
                cons_write_str("f_mount err.\n");
                exit(-1);
            }
        }
    }
    cons_write_str("mount success\n");

    fs_svr_loop();
    return 0;
}
