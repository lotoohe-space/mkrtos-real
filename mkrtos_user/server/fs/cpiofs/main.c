
#include <stdio.h>
#include "u_log.h"
#include "ns_cli.h"
#include "u_rpc_svr.h"
#include "u_prot.h"
#include "u_env.h"
#include "u_drv.h"
#include "cons_cli.h"
#include "fs_rpc.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <getopt.h>
int main(int argc, char *argv[])
{
    obj_handler_t hd;
    int ret;
    char *mount_path = NULL;
    for (int i = 0; i < argc; i++)
    {
        printf("args[%d]:%s\n", i, argv[i]);
    }
    int o;
    const char *optstring = "m:"; // 有三个选项-abc，其中c选项后有冒号，所以后面必须有参数
    while ((o = getopt(argc, argv, optstring)) != -1)
    {
        switch (o)
        {
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

    ret = rpc_meta_init(THREAD_MAIN, &hd);
    assert(ret >= 0);
    fs_svr_init();
    ns_register(mount_path, hd, MOUNT_NODE);
    cons_write_str("cpiofs mount success\n");

    fs_svr_loop();
    return 0;
}
