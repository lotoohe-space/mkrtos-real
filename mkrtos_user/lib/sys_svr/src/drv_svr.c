#include "rpc_prot.h"
#include "drv_svr.h"
#include "drv_types.h"
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_hd_man.h"
#include <stdio.h>

/*int mr_dev_open(const char *name, uint32_t oflags);*/
RPC_GENERATION_OP2(drv_t, FS_PROT, DRV_OPEN, open,
                   rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, name,
                   rpc_uint32_t_t, rpc_uint32_t_t, RPC_DIR_IN, RPC_TYPE_DATA, oflags)
{
    name->data[name->len - 1] = 0;
    return dev_open(name->data, oflags->data);
}

RPC_GENERATION_DISPATCH2(drv_t, FS_PROT, DRV_OPEN, open,
                         rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, name,
                         rpc_uint32_t_t, rpc_uint32_t_t, RPC_DIR_IN, RPC_TYPE_DATA, oflags)
/*int mr_dev_close(int desc);*/
RPC_GENERATION_OP1(drv_t, FS_PROT, DRV_CLOSE, close,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, desc)
{
    return dev_close(desc->data);
}

RPC_GENERATION_DISPATCH1(drv_t, FS_PROT, DRV_CLOSE, close,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, desc)

/*ssize_t mr_dev_read(int desc, void *buf, size_t size);*/
RPC_GENERATION_OP3(drv_t, FS_PROT, DRV_READ, read,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, desc,
                   rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_OUT, RPC_TYPE_DATA, data,
                   rpc_size_t_t, rpc_size_t_t, RPC_DIR_IN, RPC_TYPE_DATA, size)
{
    int ret = dev_read(desc->data, data->data, size->data);

    if (ret >= 0)
    {
        data->len = ret;
    }
    return ret;
}

RPC_GENERATION_DISPATCH3(drv_t, FS_PROT, DRV_READ, read,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, desc,
                         rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_OUT, RPC_TYPE_DATA, data,
                         rpc_size_t_t, rpc_size_t_t, RPC_DIR_IN, RPC_TYPE_DATA, size)

/*ssize_t mr_dev_write(int desc, const void *buf, size_t size);*/
RPC_GENERATION_OP3(drv_t, FS_PROT, DRV_WRITE, write,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, desc,
                   rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, data,
                   rpc_size_t_t, rpc_size_t_t, RPC_DIR_IN, RPC_TYPE_DATA, size)
{
    return dev_write(desc->data, data->data, size->data);
}

RPC_GENERATION_DISPATCH3(drv_t, FS_PROT, DRV_WRITE, write,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, desc,
                         rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, data,
                         rpc_size_t_t, rpc_size_t_t, RPC_DIR_IN, RPC_TYPE_DATA, size)

/*int mr_dev_ioctl(int desc, int cmd, void *args);*/
RPC_GENERATION_OP3(drv_t, FS_PROT, DRV_IOCTL, ioctl,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, desc,
                   rpc_mword_t_t, rpc_mword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, data,
                   rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, size)
{
    return dev_ioctl(desc->data, (int)(data->data), (void *)(size->data));
}

RPC_GENERATION_DISPATCH3(drv_t, FS_PROT, DRV_IOCTL, ioctl,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, desc,
                         rpc_mword_t_t, rpc_mword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, data,
                         rpc_umword_t_t, rpc_umword_t_t, RPC_DIR_IN, RPC_TYPE_DATA, size)

/*dispatch*/
RPC_DISPATCH5(drv_t, FS_PROT, typeof(DRV_OPEN), DRV_OPEN, open, DRV_CLOSE, close, DRV_READ, read, DRV_WRITE, write, DRV_IOCTL, ioctl)

void drv_init(drv_t *drv)
{
    rpc_svr_obj_init(&drv->svr, rpc_drv_t_dispatch, DRV_PROT);
}
