

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
    MEM_INFO,
    DIS_IRQ,
};

msg_tag_t sys_read_info(obj_handler_t obj, sys_info_t *info)
{
    register volatile umword_t r0 asm("r0");
    register volatile umword_t r1 asm("r1");
    register volatile umword_t r2 asm("r2");
    register volatile umword_t r3 asm("r3");

    mk_syscall(syscall_prot_create(SYS_INFO_GET, SYS_PROT, obj).raw,
               0,
               0,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     : : "r0", "r1", "r2", "r3");
    if (info)
    {
        info->sys_tick = r1;
        info->bootfs_start_addr = r2;
        info->sys_clk = r3;
    }

    return msg_tag_init(r0);
}
msg_tag_t sys_reboot(obj_handler_t obj)
{
    register volatile umword_t r0 asm("r0");
    register volatile umword_t r1 asm("r1");
    register volatile umword_t r2 asm("r2");

    mk_syscall(syscall_prot_create(REBOOT, SYS_PROT, obj).raw,
               0,
               0,
               0,
               0,
               0,
               0);
    msg_tag_t tag = msg_tag_init(r0);

    asm __volatile__(""
                     :
                     : : "r0", "r1", "r2");

    return tag;
}
msg_tag_t sys_mem_info(obj_handler_t obj, umword_t *total, umword_t *free)
{
    register volatile umword_t r0 asm("r0");
    register volatile umword_t r1 asm("r1");
    register volatile umword_t r2 asm("r2");

    mk_syscall(syscall_prot_create(MEM_INFO, SYS_PROT, obj).raw,
               0,
               0,
               0,
               0,
               0,
               0);
    msg_tag_t tag = msg_tag_init(r0);

    asm __volatile__(""
                     :
                     : : "r0", "r1", "r2");

    if (total)
    {
        *total = r1;
    }
    if (free)
    {
        *free = r2;
    }

    return tag;
}
msg_tag_t sys_dis_info(obj_handler_t obj, umword_t irq_inx)
{
    register volatile umword_t r0 asm("r0");

    mk_syscall(syscall_prot_create(DIS_IRQ, SYS_PROT, obj).raw,
               irq_inx,
               0,
               0,
               0,
               0,
               0);
    return msg_tag_init(r0);
}
