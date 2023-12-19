#include "drv_cli.h"
#include <mr_api.h>
#include <assert.h>
#include <stdio.h>
void mr_drv_test(void)
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
