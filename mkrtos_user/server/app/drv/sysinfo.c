#include "sysinfo.h"

#include "fs_cli.h"
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <stddef.h>
#include <pthread.h>
uapp_sys_info_t sys_info = {0};

static pthread_spinlock_t lock;

void sys_info_lock(void)
{
    pthread_spin_lock(&lock);
}
void sys_info_unlock(void)
{
    pthread_spin_unlock(&lock);
}

int sys_info_save(void)
{
    int fd = fs_open("/sys_info.bin", O_CREAT | O_WRONLY, 0777);
    if (fd < 0)
    {
        return fd;
    }
    sys_info_lock();
    int wlen = fs_write(fd, &sys_info, sizeof(sys_info));
    if (wlen != sizeof(sys_info))
    {
        sys_info_unlock();
        fs_close(fd);
        return -1;
    }
    sys_info_unlock();
    fs_close(fd);
    return 0;
}
int sys_info_read(void)
{
    int fd = fs_open("/sys_info.bin", O_CREAT | O_RDONLY, 0777);
    if (fd < 0)
    {
        return fd;
    }
    sys_info_lock();
    int wlen = fs_read(fd, &sys_info, sizeof(sys_info));
    if (wlen != sizeof(sys_info))
    {
        sys_info_unlock();
        fs_close(fd);
        return -1;
    }
    sys_info_unlock();
    fs_close(fd);
    return 0;
}
