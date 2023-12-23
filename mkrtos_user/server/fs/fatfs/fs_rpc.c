#include "u_rpc.h"
#include "u_rpc_svr.h"
#include "fs_svr.h"
#include "ff.h"
#include "cons_cli.h"
#include "u_log.h"
#include "u_env.h"
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include "rpc_prot.h"
#include "u_sig.h"
static fs_t fs;
static int fs_sig_call_back(pid_t pid, umword_t sig_val);
void fs_svr_init(void)
{
    fs_init(&fs);
    meta_reg_svr_obj(&fs.svr, FS_PROT);
#ifdef CONFIG_USING_SIG
    pm_sig_func_set(fs_sig_call_back);
#endif
}
typedef struct file_desc
{
    union
    {
        FIL fp;
        FATFS_DIR dir;
    };
    pid_t pid;
    uint8_t type; //!< 0:file 1:dir
} file_desc_t;

#define FILE_DESC_NR 8                  //!< 最多同时可以打开多少个文件
static file_desc_t files[FILE_DESC_NR]; //!< 预先设置的文件描述符

static void free_fd(pid_t pid)
{
    for (int i = 0; i < FILE_DESC_NR; i++)
    {
        if (files[i].fp.obj.fs)
        {
            fs_svr_close(i);
            files[i].fp.obj.fs = NULL;
            files[i].pid = 0;
        }
    }
}

static int fs_sig_call_back(pid_t pid, umword_t sig_val)
{
    switch (sig_val)
    {
    case KILL_SIG:
        free_fd(pid);
        break;
    }
    return 0;
}

static file_desc_t *alloc_file(int *fd)
{
    for (int i = 0; i < FILE_DESC_NR; i++)
    {
        if (files[i].fp.obj.fs == NULL)
        {
            *fd = i;
            files[i].pid = thread_get_src_pid();
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
    if (files[fd].fp.obj.fs == NULL)
    {
        return NULL;
    }
    return files + fd;
}
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
    // printf("open %s.\n", path);
    int fd;
    pid_t pid = thread_get_src_pid();
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
        if (ret == FR_NO_FILE || ret == FR_INVALID_NAME)
        {
            // 打开的是一个目录，则作为一个目录打开
            ret = f_opendir(&file->dir, path);
            if (ret != FR_OK)
            {
                cons_write_str("open fail..\n");
                free_file(fd);
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
    file_desc_t *file = file_get(fd);

    if (!file)
    {
        return -ENOENT;
    }
    if (file->type != 0)
    {
        return -EACCES;
    }
    FRESULT ret = f_read(&file->fp, buf, len, &br);

    if (ret != FR_OK)
    {
        return fatfs_err_conv(ret);
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
    if (file->type != 0)
    {
        return -EACCES;
    }
    FRESULT ret = f_write(&file->fp, buf, len, &bw);

    if (ret != FR_OK)
    {
        return fatfs_err_conv(ret);
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
    switch (file->type)
    {
    case 0:
        f_close(&file->fp);
        break;
    case 1:
        f_closedir(&file->dir);
        break;
    }
    file->fp.obj.fs = NULL;
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
    if (file->type != 0)
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

    return fatfs_err_conv(ret);
}
int fs_svr_ftruncate(int fd, off_t off)
{
    file_desc_t *file = file_get(fd);

    if (!file)
    {
        return -ENOENT;
    }
    if (file->type != 0)
    {
        return -EACCES;
    }
    FRESULT ret = f_truncate(&file->fp);

    return fatfs_err_conv(ret);
}
void fs_svr_sync(int fd)
{
    file_desc_t *file = file_get(fd);

    if (!file)
    {
        return;
    }
    if (file->type != 0)
    {
        return;
    }
    f_sync(&file->fp);
}
int fs_svr_readdir(int fd, dirent_t *dir)
{
    file_desc_t *file = file_get(fd);

    if (!file)
    {
        return -ENOENT;
    }
    FILINFO info;
    FRESULT ret = f_readdir(&file->dir, &info);

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
    return -ENOSYS;
}
int fs_svr_symlink(const char *src, const char *dst)
{
    return -ENOSYS;
}
void fs_svr_loop(void)
{
    rpc_loop();
}