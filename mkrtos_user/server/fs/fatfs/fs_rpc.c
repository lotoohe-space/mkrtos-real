#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "fs_svr.h"
#include "ff.h"
#include "u_log.h"
#include "u_env.h"
#include <stdio.h>
#include <fcntl.h>
static fs_t fs;

void fs_svr_init(obj_handler_t ipc)
{
    fs_init(&fs);
    fs.ipc = ipc;
}
typedef struct file_desc
{
    FIL fp;
} file_desc_t;

#define FILE_DESC_NR 8
static file_desc_t files[FILE_DESC_NR];

void *file_temp_buf_get(void)
{
    return (void *)(&files[0]);
}

static file_desc_t *alloc_file(int *fd)
{
    for (int i = 0; i < FILE_DESC_NR; i++)
    {
        if (files[i].fp.obj.fs == NULL)
        {
            *fd = i;
            return &files[i];
        }
    }
    return NULL;
}
static void free_file(int fd)
{
    files[fd].fp.obj.fs = NULL;
}
static file_desc_t *file_get(int fd)
{
    if (fd < 0 || fd >= FILE_DESC_NR)
    {
        return NULL;
    }
    return files + fd;
}

int fs_svr_open(const char *path, int flags, int mode)
{
    // printf("open %s.\n", path);
    int fd;
    file_desc_t *file = alloc_file(&fd);

    if (!file)
    {
        return -ENOMEM;
    }
    int new_mode = 0;

    switch (flags & O_ACCMODE)
    {
    case O_RDWR:
        new_mode |= FA_READ;
        new_mode |= FA_WRITE;
        break;
    case O_RDONLY:
        new_mode |= FA_READ;
        break;
    case O_WRONLY:
        new_mode |= FA_WRITE;
        break;
    }
    if ((flags & O_CREAT) && (flags & O_EXCL))
    {
        new_mode |= FA_CREATE_NEW;
    }
    else if ((flags & O_CREAT))
    {
        new_mode |= FA_OPEN_ALWAYS;
    }
    if (flags & O_APPEND)
    {
        new_mode |= FA_OPEN_APPEND;
    }

    FRESULT ret = f_open(&file->fp, path, new_mode);

    if (ret != FR_OK)
    {
        ulog_write_str(u_get_global_env()->log_hd, "open fail..\n");
        free_file(fd);
    }

    return -ret;
}

int fs_svr_read(int fd, void *buf, size_t len)
{
    UINT br;
    file_desc_t *file = file_get(fd);

    if (!file)
    {
        return -ENOENT;
    }
    FRESULT ret = f_read(&file->fp, buf, len, &br);

    if (ret != FR_OK)
    {
        return -ret;
    }
    return br;
}
int fs_svr_write(int fd, void *buf, size_t len)
{
    UINT bw;
    file_desc_t *file = file_get(fd);

    if (!file)
    {
        return -ENOENT;
    }
    FRESULT ret = f_write(&file->fp, buf, len, &bw);

    if (ret != FR_OK)
    {
        return -ret;
    }
    return bw;
}
void fs_svr_close(int fd)
{
    file_desc_t *file = file_get(fd);

    if (!file)
    {
        return;
    }
    f_close(&file->fp);
}
int fs_svr_lseek(int fd, int offs, int whence)
{
    UINT bw;
    file_desc_t *file = file_get(fd);
    int new_offs = 0;

    if (!file)
    {
        return -ENOENT;
    }
    switch (whence)
    {
    case SEEK_SET:
        new_offs = offs;
        break;
    case SEEK_END:
    {
        new_offs = f_size(&file->fp) + offs;
    }
    break;
    case SEEK_CUR:
    {
        new_offs = offs + f_tell(&file->fp);
    }
    break;
    default:
        return -EINVAL;
    }
    if (new_offs > f_size(&file->fp))
    {
        new_offs = f_size(&file->fp);
    }
    if (new_offs < 0)
    {
        new_offs = 0;
    }
    FRESULT ret = f_lseek(&file->fp, new_offs);

    return -ret;
}
void fs_svr_loop(void)
{
    rpc_loop(fs.ipc, &fs.svr);
}