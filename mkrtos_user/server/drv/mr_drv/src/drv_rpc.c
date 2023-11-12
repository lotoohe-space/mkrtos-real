#include <drv_svr.h>
#include <mr_api.h>
#include <drv_types.h>
#include "drv_rpc.h"
static drv_t drv;

void drv_svr_init(obj_handler_t ipc)
{
    drv_init(&drv);
    drv.ipc = ipc;
}

int dev_open(const char *name, uint32_t oflags)
{
    printf("%s:%d\n", __func__, __LINE__);
    return mr_dev_open(name[0] == '/' ? name + 1 : name, oflags);
}
int dev_close(int desc)
{
    printf("%s:%d\n", __func__, __LINE__);

    return mr_dev_close(desc);
}
int dev_read(int desc, void *buf, size_t size)
{
    printf("%s:%d\n", __func__, __LINE__);
    return mr_dev_read(desc, buf, size);
}
int dev_write(int desc, const void *buf, size_t size)
{
    printf("%s:%d\n", __func__, __LINE__);

    return mr_dev_write(desc, buf, size);
}
int dev_ioctl(int desc, int cmd, void *args)
{
    printf("%s:%d\n", __func__, __LINE__);

    return mr_dev_ioctl(desc, cmd, args);
}

void drv_svr_loop(void)
{
    rpc_loop(drv.ipc, &drv.svr);
}
