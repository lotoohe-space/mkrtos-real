
// #include <stdio.h>
#include "u_log.h"
#include "u_env.h"
#include "u_sleep.h"
#include "pm_cli.h"
#include "drv_cli.h"
#include <unistd.h>
#include <mr_api.h>
#include <assert.h>
void drv_test(void)
{
    char data[10] = "mkrtos..\n";

    int fd = dev_fs_open("/dev/uart1", MR_OFLAG_RDWR);
    assert(fd > 0);
    int ret = dev_fs_read(fd, data, sizeof(data));
    assert(ret >= 0);
    printf("%s:%d data:%s.\n", __func__, __LINE__, data);
    ret = dev_fs_write(fd, data, sizeof(data));
    assert(ret >= 0);
    dev_fs_close(fd);
}

int main(int argc, char *args[])
{
    printf("Hello world.\n");
    usleep(100000);
    // while (pm_run_app("hello", 0) < 0)
    //     usleep(100000);
    ulog_write_str(u_get_global_env()->log_hd, "hello is runing...\n");
    drv_test();
    ulog_write_str(u_get_global_env()->log_hd, "Hello world.\n");

    return 0;
}
