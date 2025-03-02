

#include <stdio.h>
#include "u_log.h"
#include "ns_cli.h"
#include "u_rpc_svr.h"
#include "u_prot.h"
#include "u_env.h"
#include "u_task.h"
#include "cons_cli.h"
#include "fs_rpc.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <getopt.h>
#include <u_fast_ipc.h>
#include "appfs.h"
#include "hw_block.h"
#include "appfs_open.h"
#include "u_sleep.h"
#include "u_hd_man.h"
#define STACK_COM_ITME_SIZE (2 * 1024)
ATTR_ALIGN(8)
uint8_t stack_coms[STACK_COM_ITME_SIZE];
uint8_t msg_buf_coms[MSG_BUG_LEN];
static obj_handler_t com_th_obj;
static fs_info_t fs_obj;
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
    char *mount_path = NULL;
    char *dev_path = NULL;

    task_set_obj_name(TASK_THIS, TASK_THIS, "tk_appfs");
    task_set_obj_name(TASK_THIS, THREAD_MAIN, "th_appfs");
    for (int i = 0; i < argc; i++)
    {
        printf("args[%d]:%s\n", i, argv[i]);
    }
    fast_ipc_init();
    int o;
    const char *optstring = "d:m:"; // 有三个选项-abc，其中c选项后有冒号，所以后面必须有参数
    while ((o = getopt(argc, argv, optstring)) != -1)
    {
        switch (o)
        {
        case 'd':
            dev_path = optarg;
            break;
        case 'm':
            printf("mount path:%s\n", optarg);
            mount_path = optarg;
            break;
        case '?':
            printf("error optopt: %c\n", optopt);
            printf("error opterr: %d\n", opterr);
            break;
        }
    }
    if (dev_path == NULL || mount_path == NULL)
    {
        printf("dev or mout path don't setting.\n");
        printf("example:appfs -m /bin -d /block\n");
        return -1;
    }
    ret = hw_init_block(&fs_obj, dev_path);
    assert(ret >= 0);
    ret = appfs_init(&fs_obj);
    assert(ret >= 0);
    ret = appfs_open_init(&fs_obj);
    assert(ret >= 0);

    ret = rpc_meta_init(TASK_THIS, &hd);
    assert(ret >= 0);
    fs_svr_init();
    ns_register(mount_path, hd, 0);
    cons_write_str("appfs mount success\n");
#if 0
    fs_svr_loop();
#endif
    while (1)
    {
        u_sleep_ms((umword_t)(-1));
    }
    return 0;
}