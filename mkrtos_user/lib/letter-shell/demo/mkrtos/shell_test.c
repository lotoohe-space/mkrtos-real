#include "shell.h"
#include "cons_cli.h"
#include "u_sig.h"
#include "cons_cli.h"
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>
#ifdef CONFIG_USING_SIG
static int sig_cn;
static int shell_sig_call_back(pid_t pid, umword_t sig_val)
{
    /*TODO:这个消息是init发送的，这里不能给init发送消息，否导致卡死除非init增加多进程支持*/
    cons_write_str("test sig.\n");
    sig_cn++;
    return 0;
}
int shell_test_sig(int argc, char *argv[])
{
    int ret;

    if (argc < 2)
    {
        return -1;
    }
    pm_sig_func_set(shell_sig_call_back);
    pid_t pid = atoi(argv[1]);

    ret = pm_sig_watch(pid, 0 /*TODO:现在只有kill */);

    printf("sig_cn:%d\n", sig_cn);
    return ret;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), test_sig, shell_test_sig, shell_test_sig command);
#endif
