#include "cons_cli.h"
#include "ff.h"
#include "fs_svr.h"
#include "kstat.h"
#include "rpc_prot.h"
#include "u_env.h"
#include "u_log.h"
#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "u_sig.h"
#include "pm_cli.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <u_malloc.h>
#include "fd.h"
#include "u_path.h"

static fs_t fs;
static int fs_sig_call_back(pid_t pid, umword_t sig_val);

typedef struct file_desc_priv
{
    union
    {
        FIL fp;
        FATFS_DIR dir;
    };
    uint8_t type; //!< 0:file 1:dir
} file_desc_priv_t;

void fs_svr_close(int fd);
static int fatfs_err_conv(FRESULT res)
{
    switch (res)
    {
    case FR_OK:
        return 0;
    case FR_DISK_ERR:
        return -EIO;
    case FR_NO_FILE:
        return -ENOENT;
    case FR_NO_PATH:
        return -ENOENT;
    case FR_INVALID_NAME:
        return -EINVAL;
    case FR_DENIED:
        return -EPERM;
    case FR_EXIST:
        return -EEXIST;
    case FR_WRITE_PROTECTED:
        return -EIO;
    case FR_TIMEOUT:
        return -ETIMEDOUT;
    case FR_NOT_ENOUGH_CORE:
        return -ENOMEM;
    case FR_INVALID_PARAMETER:
        return -EINVAL;
    default:
        return -1;
    }
}
int fs_svr_open(const char *path, int flags, int mode)
{
    int fd;
    pid_t pid = thread_get_src_pid();
    file_desc_priv_t *file = NULL;

    file = u_malloc(sizeof(*file));
    if (file == NULL)
    {
        return -ENOMEM;
    }

    fd = file_desc_alloc(pid, file, fs_svr_close);

    if (fd < 0)
    {
        u_free(file);
        return fd;
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
        if (ret == FR_NO_FILE || ret == FR_INVALID_NAME)
        {
            // 打开的是一个目录，则作为一个目录打开
            ret = f_opendir(&file->dir, path);
            if (ret != FR_OK)
            {
                cons_write_str("open fail..\n");
                file_desc_free(fd);
                u_free(file);
                return fatfs_err_conv(ret);
            }
            file->type = 1;
            // cons_write_str("open dir..\n");
        }
    }
    else
    {
        file->type = 0;
        // cons_write_str("open file..\n");
    }

    if (ret != FR_OK)
    {
        file_desc_free(fd);
        u_free(file);
        return fatfs_err_conv(ret);
    }
#ifdef CONFIG_USING_SIG
    int w_ret = pm_sig_watch(pid, 0 /*TODO:现在只有kill */);
    if (w_ret < 0)
    {
        printf("pm wath pid %d err.\n", w_ret);
    }
#endif
    return fd;
}

int fs_svr_read(int fd, void *buf, size_t len)
{
    UINT br;
    file_desc_t *file = file_desc_get(fd);
    file_desc_priv_t *file_priv = NULL;

    if (!file || file->priv == NULL)
    {
        return -ENOENT;
    }
    file_priv = file->priv;
    if (file_priv->type != 0)
    {
        return -EACCES;
    }
    FRESULT ret = f_read(&file_priv->fp, buf, len, &br);

    if (ret != FR_OK)
    {
        return fatfs_err_conv(ret);
    }
    return br;
}
int fs_svr_write(int fd, void *buf, size_t len)
{
    UINT bw;
    file_desc_t *file = file_desc_get(fd);
    file_desc_priv_t *file_priv = NULL;

    if (!file || file->priv == NULL)
    {
        return -ENOENT;
    }
    file_priv = file->priv;
    if (file_priv->type != 0)
    {
        return -EACCES;
    }
    FRESULT ret = f_write(&file_priv->fp, buf, len, &bw);

    if (ret != FR_OK)
    {
        return fatfs_err_conv(ret);
    }
    return bw;
}
void fs_svr_close(int fd)
{
    file_desc_t *file = file_desc_get(fd);
    file_desc_priv_t *file_priv = NULL;

    if (!file || file->priv == NULL)
    {
        return;
    }
    file_priv = file->priv;

    switch (file_priv->type)
    {
    case 0:
        f_close(&file_priv->fp);
        break;
    case 1:
        f_closedir(&file_priv->dir);
        break;
    }
    file_priv->fp.obj.fs = NULL;
    file_desc_free_unlock(fd);
    u_free(file_priv);
}
int fs_svr_readdir(int fd, dirent_t *dir)
{
    file_desc_t *file = file_desc_get(fd);
    file_desc_priv_t *file_priv = NULL;

    if (!file || file->priv == NULL)
    {
        return -ENOENT;
    }
    file_priv = file->priv;
    FILINFO info;
    FRESULT ret = f_readdir(&file_priv->dir, &info);

    if (ret != FR_OK || info.fname[0] == 0)
    {
        return -ENOENT;
    }
    strncpy(dir->d_name, info.fname, sizeof(dir->d_name));
    dir->d_name[sizeof(dir->d_name) - 1] = 0;
    dir->d_reclen = sizeof(*dir);
    dir->d_off = 0;
    if (info.fattrib & AM_DIR)
    { /* Directory */
        dir->d_type = DT_DIR;
    }
    else
    { /* File */
        dir->d_type = DT_CHR;
    }
    return sizeof(*dir);
}
int fs_svr_lseek(int fd, int offs, int whence)
{
    UINT bw;
    file_desc_t *file = file_desc_get(fd);
    file_desc_priv_t *file_priv = NULL;

    int new_offs = 0;

    if (!file || file->priv == NULL)
    {
        return -ENOENT;
    }
    file_priv = file->priv;

    if (file_priv->type != 0)
    {
        return -EACCES;
    }
    switch (whence)
    {
    case SEEK_SET:
        new_offs = offs;
        break;
    case SEEK_END:
    {
        new_offs = f_size(&file_priv->fp) + offs;
    }
    break;
    case SEEK_CUR:
    {
        new_offs = offs + f_tell(&file_priv->fp);
    }
    break;
    default:
        return -EINVAL;
    }
#if 0
    if (new_offs > f_size(&file_priv->fp)) {
        new_offs = f_size(&file_priv->fp);
    }
#endif
    if (new_offs < 0)
    {
        new_offs = 0;
    }
    FRESULT ret = f_lseek(&file_priv->fp, new_offs);

    return fatfs_err_conv(ret);
}
int fs_svr_ftruncate(int fd, off_t off)
{
    file_desc_t *file = file_desc_get(fd);
    file_desc_priv_t *file_priv = NULL;

    if (!file || file->priv == NULL)
    {
        return -ENOENT;
    }
    file_priv = file->priv;
    if (file_priv->type != 0)
    {
        return -EACCES;
    }
    FRESULT ret = f_truncate(&file_priv->fp);

    return fatfs_err_conv(ret);
}
int fs_svr_fstat(int fd, void *_stat)
{
    struct kstat *stat = _stat;
    file_desc_t *file = file_desc_get(fd);
    file_desc_priv_t *file_priv = NULL;

    if (!file || file->priv == NULL)
    {
        return -ENOENT;
    }
    file_priv = file->priv;
    memset(stat, 0, sizeof(*stat));
    stat->st_size = file_priv->type == 1 ? 0 : f_size(&file_priv->fp);
    stat->st_mode = file_priv->type == 1 ? S_IFDIR : S_IFREG;
    stat->st_blksize = 0;
    return 0;
}
int fs_svr_ioctl(int fd, int req, void *arg)
{
    return 0; /*TODO:*/
}
int fs_svr_fsync(int fd)
{
    file_desc_t *file = file_desc_get(fd);
    file_desc_priv_t *file_priv = NULL;

    if (!file || file->priv == NULL)
    {
        return -EBADFD;
    }
    file_priv = file->priv;
    if (file_priv->type != 0)
    {
        return -EBADFD;
    }
    f_sync(&file_priv->fp);
    return 0;
}
int fs_svr_unlink(const char *path)
{
    FRESULT ret = f_unlink(path);

    return fatfs_err_conv(ret);
}
int fs_svr_symlink(const char *existing, const char *new)
{
    return -ENOSYS;
}
int fs_svr_mkdir(char *path)
{
    FRESULT ret = f_mkdir(path);

    return fatfs_err_conv(ret);
}
int fs_svr_rmdir(char *path)
{
    return -ENOSYS;
}
int fs_svr_rename(char *oldname, char *newname)
{
    return fatfs_err_conv(f_rename(oldname, newname));
}
int fs_svr_stat(const char *path, void *_buf)
{
    FILINFO INFO;
    FRESULT ret;
    struct kstat *buf = (struct kstat *)_buf;

    if (u_is_root_path(path))
    {
        buf->st_size = 0;
        buf->st_mode = S_IFDIR;
        return 0;
    }
    ret = f_stat(path, &INFO);
    if (ret != FR_OK)
    {
        return fatfs_err_conv(ret);
    }
    memset(buf, 0, sizeof(*buf));
    buf->st_size = INFO.fsize;
    buf->st_mode = (INFO.fattrib & AM_DIR) ? S_IFDIR : S_IFREG;
    return 0;
}
ssize_t fs_svr_readlink(const char *path, char *buf, size_t bufsize)
{
    return -ENOSYS;
}

int fs_svr_statfs(const char *path, struct statfs *buf)
{
    return -ENOSYS;
}
static const fs_operations_t ops =
    {
        .fs_svr_open = fs_svr_open,
        .fs_svr_read = fs_svr_read,
        .fs_svr_write = fs_svr_write,
        .fs_svr_close = fs_svr_close,
        .fs_svr_readdir = fs_svr_readdir,
        .fs_svr_lseek = fs_svr_lseek,
        .fs_svr_ftruncate = fs_svr_ftruncate,
        .fs_svr_fstat = fs_svr_fstat,
        .fs_svr_ioctl = fs_svr_ioctl,
        // .fs_svr_fcntl = fs_svr_fcntl,
        .fs_svr_fsync = fs_svr_fsync,
        .fs_svr_unlink = fs_svr_unlink,
        .fs_svr_symlink = fs_svr_symlink,
        .fs_svr_mkdir = fs_svr_mkdir,
        .fs_svr_rmdir = fs_svr_rmdir,
        .fs_svr_rename = fs_svr_rename,
        .fs_svr_stat = fs_svr_stat,
        .fs_svr_readlink = fs_svr_readlink,
};
void fs_svr_init(void)
{
    fs_init(&fs, &ops);
    meta_reg_svr_obj(&fs.svr, FS_PROT);
    file_desc_init();
}
