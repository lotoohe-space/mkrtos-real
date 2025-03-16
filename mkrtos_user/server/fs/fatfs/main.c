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
#include <unistd.h>
#include "u_hd_man.h"
#include <u_fast_ipc.h>
#include <getopt.h>
#define STACK_COM_ITME_SIZE (2048)
ATTR_ALIGN(8)
static uint8_t stack_coms[STACK_COM_ITME_SIZE];
static uint8_t msg_buf_coms[MSG_BUG_LEN];
static obj_handler_t com_th_obj;

static void fast_ipc_init(void)
{
    com_th_obj = handler_alloc();
    assert(com_th_obj != HANDLER_INVALID);
    u_fast_ipc_init(stack_coms, msg_buf_coms, 1, STACK_COM_ITME_SIZE, &com_th_obj);
}

static FATFS fs;
static MKFS_PARM defopt = {FM_ANY, 0, 0, 0};

extern int disk_set_dev_path(int pdrv, const char *dev);
int main(int argc, char *argv[])
{
    obj_handler_t hd;
    int ret;
    char *dev_path = NULL;
    char *mount_path = NULL;
    int opt;
    fast_ipc_init();

    while ((opt = getopt(argc, argv, "m:d:")) != -1)
    {
        switch (opt)
        {
        case 'm':
            mount_path = optarg;
            break;
        case 'd':
            dev_path = optarg;
            break;
        }
    }
   
    if (mount_path == NULL || dev_path == NULL)
    {
        printf("example:fatfs -m /mnt -d /dev/ram_block\n");
        return -1;
    }
    if (disk_set_dev_path(0, dev_path) < 0)
    {
        printf("dev open failed.\n");
        return -1;
    }

    ret = rpc_meta_init_def(TASK_THIS, &hd);
    assert(ret >= 0);
    fs_svr_init();

    FRESULT res = f_mount(&fs, "0:/", 1);

    if (res != FR_OK)
    {
        assert(sizeof(fs.win) >= FF_MAX_SS);
        res = f_mkfs("0:/", &defopt, (void *)(fs.win), FF_MAX_SS); // 第三个参数可以设置成NULL，默认使用heap memory
        if (res != FR_OK)
        {
            printf("f_mkfs err.\n");
            exit(-1);
        }
        else
        {
            res = f_mount(&fs, "0:/", 1);
            if (res != FR_OK)
            {
                printf("f_mount err.\n");
                exit(-1);
            }
        }
    }
    ns_register(mount_path, hd, 0);
    printf("fatfs mount success\n");

    while (1)
    {
        u_sleep_ms(U_SLEEP_ALWAYS);
    }
    return 0;
}
