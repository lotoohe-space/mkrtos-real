#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_sys.h"
#include "fs_svr.h"
#include "cons_cli.h"
#include "u_log.h"
#include "u_env.h"
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include "rpc_prot.h"
#include "cpiofs.h"
static fs_t fs;

typedef struct file_desc
{
    pid_t pid;
    addr_t file_addr;
    size_t file_size;
    ssize_t offset;
} file_desc_t;

#define CPIO_FS_FD_NR 8
static file_desc_t fds[CPIO_FS_FD_NR];

static file_desc_t *fd_alloc(addr_t file_addr, size_t file_size, ssize_t offset, int *fd)
{
    for (int i = 0; i < CPIO_FS_FD_NR; i++)
    {
        if (fds[i].file_addr == 0)
        {
            fds[i].pid = thread_get_src_pid();
            fds[i].file_addr = file_addr;
            fds[i].file_size = file_size;
            fds[i].offset = offset;
            if (fd)
            {
                *fd = i;
            }
            return fds + i;
        }
    }
    return NULL;
}
static file_desc_t *fd_get(int fd)
{
    if (fd < 0 || fd >= CPIO_FS_FD_NR)
    {
        return NULL;
    }
    if (fds[fd].file_addr == 0)
    {
        return NULL;
    }
    if (fds[fd].pid != thread_get_src_pid())
    {
        return NULL;
    }
    return &fds[fd];
}
static void fd_free(int fd)
{
    if (fd < 0 || fd >= CPIO_FS_FD_NR)
    {
        return;
    }
    if (fds[fd].file_addr)
    {
        if (fds[fd].pid == thread_get_src_pid())
        {
            fds[fd].file_addr = 0;
            fds[fd].pid = -1;
        }
    }
}

void fs_svr_init(void)
{
    fs_init(&fs);
    meta_reg_svr_obj(&fs.svr, FS_PROT);
}
int fs_svr_open(const char *path, int flags, int mode)
{
    msg_tag_t tag;
    sys_info_t sys_info;

    if (flags & O_RDWR)
    {
        return -EACCES;
    }
    if (flags & O_WRONLY)
    {
        return -EACCES;
    }
    if (path[0]=='/') {
        path++;
    }

    tag = sys_read_info(SYS_PROT, &sys_info);
    if (msg_tag_get_val(tag) < 0)
    {
        return -ENOENT;
    }
    umword_t size;
    umword_t addr = cpio_find_file((umword_t)sys_info.bootfs_start_addr, (umword_t)(-1), path, &size);
    if (!addr)
    {
        return -ENOENT;
    }
    int fd;
    // 找到指定文件
    file_desc_t *fdp = fd_alloc(addr, size, 0, &fd);

    if (!fdp)
    {
        return -ENOMEM;
    }

    return fd;
}

int fs_svr_read(int fd, void *buf, size_t len)
{
    file_desc_t *fdp = fd_get(fd);

    if (!fdp)
    {
        return -ENOENT;
    }
    size_t remain_size = fdp->file_size - fdp->offset;
    size_t read_size = 0;

    read_size = MIN(remain_size, len);
    if (read_size)
    {
        memcpy(buf, (void *)(fdp->file_addr + fdp->offset), read_size);
        fdp->offset += read_size;
    }
    return read_size;
}
int fs_svr_write(int fd, void *buf, size_t len)
{
    file_desc_t *fdp = fd_get(fd);

    if (!fdp)
    {
        return -ENOENT;
    }
    return -EACCES;
}
void fs_svr_close(int fd)
{
    fd_free(fd);
}
int fs_svr_lseek(int fd, int offs, int whence)
{
    file_desc_t *file = fd_get(fd);
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
        new_offs = file->file_size + offs;
    }
    break;
    case SEEK_CUR:
    {
        new_offs = offs + file->offset;
    }
    break;
    default:
        return -EINVAL;
    }
    if (new_offs > file->file_size)
    {
        new_offs = file->file_size;
    }
    if (new_offs < 0)
    {
        new_offs = 0;
    }
    file->offset = new_offs;

    return 0;
}
int fs_svr_ftruncate(int fd, off_t off)
{
    return -ENOSYS;
}
void fs_svr_sync(int fd)
{
}
int fs_svr_readdir(int fd, dirent_t *dir)
{
    return -ENOSYS;
}
int fs_svr_mkdir(char *path)
{
    return -ENOSYS;
}
int fs_svr_unlink(char *path)
{
    return -ENOSYS;
}
int fs_svr_renmae(char *oldname, char *newname)
{
    return -ENOSYS;
}
int fs_svr_fstat(int fd, stat_t *stat)
{
    file_desc_t *file = fd_get(fd);

    if (!file)
    {
        return -ENOENT;
    }
    stat->st_size = file->file_size;
    return 0;
}

void fs_svr_loop(void)
{
    rpc_loop();
}