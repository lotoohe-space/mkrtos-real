#include <arch/atomic.h>
#include <mkrtos/sched.h>
#include "config.h"
#include "knl_service.h"
#include <mm.h>


static int test_fs(void)
{
    // int sys_mknod(const char * filename, int mode, dev_t dev);
    // int sys_mkdir(const char * pathname, int mode);

    // int32_t sys_open(const char* path,int32_t flags,int32_t mode);
    // int sys_write (int fd,uint8_t *buf,uint32_t len);
    // int sys_read (int fd,uint8_t *buf,uint32_t len);
    // void sys_close(int fp);

    // int sys_readdir(unsigned int fd, struct dirent * dirent, uint32_t count);
    return 0;
}

INIT_TEST_REG(test_fs, INIT_TEST_LVL);

