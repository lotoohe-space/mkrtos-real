#ifdef NO_LITTLE_MODE
#include "syscall_backend.h"
#include "misc_backend.h"
#include <errno.h>
#include <syscall.h>

typedef long (*sys_call_func)(va_list ap);

static const sys_call_func sys_call_list[] = {
    // TODO: add syscall func pointer.
    [SYS_munmap] = sys_munmap,
    [SYS_mmap2] = sys_mmap2,
    [SYS_read] = sys_be_read,                    //
    [SYS_write] = sys_be_write,                  //
    [SYS_writev] = sys_be_writev,                //
    [SYS_ioctl] = sys_be_ioctl,                  //
    [SYS_set_tid_address] = sys_set_tid_address, //
    [__ARM_NR_set_tls] = sys_set_thread_area,    //
    [SYS_exit] = be_exit,
};

static const sys_call_func_nr = sizeof(sys_call_list) / sizeof(sys_call_func);

long syscall_backend(long sys_inx, ...)
{
    long ret = -ENOSYS;
    if (sys_inx < 0 || sys_inx >= sys_call_func_nr)
    {
        return -ENOSYS;
    }

    if (sys_call_list[sys_inx])
    {

        va_list ap;

        va_start(ap, sys_inx);
        ret = sys_call_list[sys_inx](ap);
        va_end(ap);
    }

    return ret;
}
#endif
