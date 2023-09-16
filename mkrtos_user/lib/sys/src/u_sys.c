

#include "u_log.h"
#include "u_prot.h"
#include "u_types.h"
#include "u_util.h"
#include "u_sys.h"
#include <string.h>

enum sys_op
{
    SYS_INFO_GET,
    REBOOT,
};

msg_tag_t sys_read_info(obj_handler_t obj, sys_info_t *info)
{
    register volatile umword_t r0 asm("r0");
    register volatile umword_t r1 asm("r1");
    register volatile umword_t r2 asm("r2");

    syscall(syscall_prot_create(SYS_INFO_GET, SYS_PROT, obj),
            0,
            0,
            0,
            0,
            0,
            0);
    msg_tag_t tag = msg_tag_init(r0);
    if (info)
    {
        info->sys_tick = r1;
        info->bootfs_start_addr = r2;
    }
    return tag;
}
