
#include "u_types.h"
#include "u_hd_man.h"
#include <u_mutex.h>
#include <errno.h>
#include <stdio.h>
#include <u_sig.h>
#include "fd.h"

#define FILE_DESC_NR_OPEN 8
static file_desc_t file_desc[FILE_DESC_NR_OPEN];
static u_mutex_t fds_lock;

static int fs_sig_call_back(pid_t pid, umword_t sig_val)
{
    switch (sig_val)
    {
    case KILL_SIG:
        file_desc_recycle(pid);
        break;
    }
    return 0;
}

int file_desc_init(void)
{
    int ret;
    obj_handler_t sema_hd;

    sema_hd = handler_alloc();
    if (sema_hd == HANDLER_INVALID)
    {
        return -ENOMEM;
    }
    ret = u_mutex_init(&fds_lock, sema_hd);
    if (ret < 0)
    {
        return ret;
    }
    pm_sig_func_set(fs_sig_call_back);
    return ret;
}
int file_desc_alloc(pid_t pid, void *priv, void (*close_fn)(int fd))
{
    u_mutex_lock(&fds_lock, 0, NULL);
    for (int i = 0; i < FILE_DESC_NR_OPEN; i++)
    {
        if (file_desc[i].close == NULL)
        {
            file_desc[i].pid = pid;
            file_desc[i].priv = priv;
            file_desc[i].close = close_fn;
            u_mutex_unlock(&fds_lock);
            return i;
        }
    }
    u_mutex_unlock(&fds_lock);
    return -ENOMEM;
}
int file_desc_set_priv(int fd, void *priv)
{
    if (fd < 0 || fd >= FILE_DESC_NR_OPEN)
    {
        return -EBADF;
    }
    file_desc[fd].priv = priv;
    return 0;
}
file_desc_t *file_desc_get(int fd)
{
    if (fd < 0 || fd >= FILE_DESC_NR_OPEN)
    {
        return NULL;
    }
    return &file_desc[fd];
}
void *file_desc_free_unlock(int fd)
{
    void *ret;

    if (fd < 0 || fd >= FILE_DESC_NR_OPEN)
    {
        return NULL;
    }
    file_desc[fd].pid = 0;
    ret = file_desc[fd].priv;
    file_desc[fd].priv = NULL;
    file_desc[fd].close = NULL;
    return ret;
}
void *file_desc_free(int fd)
{
    void *ret;

    u_mutex_lock(&fds_lock, 0, NULL);
    ret = file_desc_free_unlock(fd);
    u_mutex_unlock(&fds_lock);
    return ret;
}
void file_desc_recycle(pid_t pid)
{
    int free_cn = 0;

    u_mutex_lock(&fds_lock, 0, NULL);
    for (int i = 0; i < FILE_DESC_NR_OPEN; i++)
    {
        if (file_desc[i].close != NULL && file_desc[i].pid == pid)
        {
            file_desc[i].close(i);
            printf("free fd:%d.\n", i);
        }
    }
    u_mutex_unlock(&fds_lock);
}