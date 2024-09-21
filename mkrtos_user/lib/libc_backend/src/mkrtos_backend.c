
#include <u_task.h>
#include <u_hd_man.h>
#include <u_types.h>
#include <u_sleep.h>
#include <syscall_backend.h>
#include <stdarg.h>

long sys_wait_hd_release(va_list ap)
{
    long hd;

    ARG_1_BE(ap, hd, long);

    while (msg_tag_get_val(task_obj_valid(TASK_THIS, hd, NULL)) == 1)
    {
        u_sleep_ms(1);
    }
    return 0;
}
