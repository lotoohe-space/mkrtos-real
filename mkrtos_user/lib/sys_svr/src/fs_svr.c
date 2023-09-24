#include "rpc_prot.h"
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_hd_man.h"
#include "fs_svr.h"
#include <stdio.h>

/*open*/
RPC_GENERATION_OP3(fs_t, FS_OPEN, open,
                   rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, mode)
{
    path->data[path->len - 1] = 0;
    int ret = fs_svr_open((char *)(path->data), flags->data, mode->data);
    return ret;
}

RPC_GENERATION_DISPATCH3(fs_t, FS_OPEN, open,
                         rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, path,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, flags,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, mode)
/*read*/
RPC_GENERATION_OP3(fs_t, FS_READ, read,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                   rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_OUT, RPC_TYPE_DATA, buf,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)
{
    int ret = fs_svr_read(fd->data, buf->data, len->data);

    if (ret >= 0)
    {
        buf->len = ret;
    }
    return ret;
}

// RPC_GENERATION_DISPATCH3(fs_t, FS_READ, read,
//                          rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
//                          rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_OUT, RPC_TYPE_DATA, buf,
//                          rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)
msg_tag_t fs_t_read_dispatch(fs_t *obj, msg_tag_t tag, ipc_msg_t *ipc_msg)
{
    rpc_int_t var0;
    rpc_array_uint32_t_uint8_t_32_t var1;
    rpc_int_t var2;
    size_t op_val;
    uint8_t *value = (uint8_t *)(ipc_msg->msg_buf);
    int off = 0;
    rpc_var_rpc_int_t_init(&var0);
    rpc_var_rpc_array_uint32_t_uint8_t_32_t_init(&var1);
    rpc_var_rpc_int_t_init(&var2);
    op_val = *((typeof(((uint16_t)2)) *)value);
    if (op_val != ((uint16_t)2))
    {
        return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (-71)});
    }
    off += sizeof(typeof(((uint16_t)2)));
    off = rpc_align(off, __alignof(typeof(((uint16_t)2))));
    do
    {
        if (1 == 1)
        {
            if (1 == 1 || 1 == 4)
            {
                int ret = rpc_svr_buf_to_msg_rpc_int_t(&var0, (uint8_t *)(value), off, tag.msg_buf_len * (sizeof(void *)));
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    do
    {
        if (1 == 1)
        {
            if (2 == 1 || 2 == 4)
            {
                int ret = rpc_svr_buf_to_msg_rpc_array_uint32_t_uint8_t_32_t(&var1, (uint8_t *)(value), off, tag.msg_buf_len * (sizeof(void *)));
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    do
    {
        if (1 == 1)
        {
            if (1 == 1 || 1 == 4)
            {
                int ret = rpc_svr_buf_to_msg_rpc_int_t(&var2, (uint8_t *)(value), off, tag.msg_buf_len * (sizeof(void *)));
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    short ret_val = fs_t_read_op(obj, &var0, &var1, &var2);
    if (ret_val < 0)
    {
        return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret_val)});
    }
    off = 0;
    int off_map = 0;
    do
    {
        if (1 == 1)
        {
            if (1 == 2 || 1 == 4)
            {
                int ret = rpc_svr_msg_to_buf_rpc_int_t(&var0, (uint8_t *)(value), off);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    do
    {
        if (1 == 1)
        {
            if (2 == 2 || 2 == 4)
            {
                int ret = rpc_svr_msg_to_buf_rpc_array_uint32_t_uint8_t_32_t(&var1, (uint8_t *)(value), off);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    do
    {
        if (1 == 1)
        {
            if (1 == 2 || 1 == 4)
            {
                int ret = rpc_svr_msg_to_buf_rpc_int_t(&var2, (uint8_t *)(value), off);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off = ret;
            }
        }
    } while (0);
    do
    {
        if (1 == 2)
        {
            if (1 == 2 || 1 == 4)
            {
                int ret = rpc_svr_msg_to_buf_rpc_int_t(&var0, (uint8_t *)(ipc_msg->map_buf), off_map);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off_map = ret;
            }
        }
    } while (0);
    do
    {
        if (1 == 2)
        {
            if (2 == 2 || 2 == 4)
            {
                int ret = rpc_svr_msg_to_buf_rpc_array_uint32_t_uint8_t_32_t(&var1, (uint8_t *)(ipc_msg->map_buf), off_map);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off_map = ret;
            }
        }
    } while (0);
    do
    {
        if (1 == 2)
        {
            if (1 == 2 || 1 == 4)
            {
                int ret = rpc_svr_msg_to_buf_rpc_int_t(&var2, (uint8_t *)(ipc_msg->map_buf), off_map);
                if (ret < 0)
                {
                    return ((msg_tag_t){.flags = (0), .msg_buf_len = (0), .map_buf_len = (0), .prot = (ret)});
                }
                off_map = ret;
            }
        }
    } while (0);
    return ((msg_tag_t){.flags = (0), .msg_buf_len = ((((off) / ((sizeof(void *)))) + (((off) % ((sizeof(void *)))) ? 1 : 0))), .map_buf_len = ((((off_map) / ((sizeof(void *)))) + (((off_map) % ((sizeof(void *)))) ? 1 : 0))), .prot = (ret_val)});
}
/*write*/
RPC_GENERATION_OP3(fs_t, FS_WRITE, write,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                   rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, buf,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)
{
    int ret = fs_svr_write(fd->data, buf->data, len->data);
    return ret;
}

RPC_GENERATION_DISPATCH3(fs_t, FS_WRITE, write,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                         rpc_ref_array_uint32_t_uint8_t_32_t, rpc_array_uint32_t_uint8_t_32_t, RPC_DIR_IN, RPC_TYPE_DATA, buf,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len)

/*close*/
RPC_GENERATION_OP1(fs_t, FS_CLOSE, close,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd)
{
    fs_svr_close(fd->data);
    return 0;
}

RPC_GENERATION_DISPATCH1(fs_t, FS_CLOSE, close,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd)

/*lseek*/
RPC_GENERATION_OP3(fs_t, FS_LSEEK, lseek,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, offs,
                   rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, whence)
{
    int ret = fs_svr_lseek(fd->data, offs->data, whence->data);
    return ret;
}

RPC_GENERATION_DISPATCH3(fs_t, FS_LSEEK, lseek,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, fd,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, len,
                         rpc_int_t, rpc_int_t, RPC_DIR_IN, RPC_TYPE_DATA, whence)

/*dispatch*/
RPC_DISPATCH5(fs_t, typeof(FS_OPEN), FS_OPEN, open, FS_READ, read,
              FS_WRITE, write, FS_CLOSE, close, FS_LSEEK, lseek)

void fs_init(fs_t *fs)
{
    rpc_svr_obj_init(&fs->svr, rpc_fs_t_dispatch, FS_PROT);
}
