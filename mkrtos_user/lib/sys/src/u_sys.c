

#include "u_log.h"
#include "u_prot.h"
#include "u_types.h"
#include "u_util.h"
#include "u_sys.h"
#include <string.h>

enum sys_op
{
    SYS_INFO_GET,
    SYS_INFO2_GET,
    REBOOT,
    MEM_INFO,
    DIS_IRQ,
};
MK_SYSCALL
msg_tag_t sys_read_info(obj_handler_t obj, sys_info_t *info, umword_t flags)
{
    register volatile umword_t r0 asm(ARCH_REG_0);
    register volatile umword_t r1 asm(ARCH_REG_1);
    register volatile umword_t r2 asm(ARCH_REG_2);
    register volatile umword_t r3 asm(ARCH_REG_3);
    register volatile umword_t r4 asm(ARCH_REG_4);

    mk_syscall(syscall_prot_create(SYS_INFO_GET, SYS_PROT, obj).raw,
               flags,
               0,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     : : ARCH_REG_0, ARCH_REG_1, ARCH_REG_2, ARCH_REG_3, ARCH_REG_4);
    if (info)
    {
        info->sys_tick = r1;
        info->bootfs_start_addr = r2;
        info->sys_clk = r3;
    }

    return msg_tag_init(r0);
}
MK_SYSCALL
msg_tag_t sys_read_info2(obj_handler_t obj, sys_info2_t *info, umword_t flags)
{
    register volatile umword_t r0 asm(ARCH_REG_0);
    register volatile umword_t r1 asm(ARCH_REG_1);
    register volatile umword_t r2 asm(ARCH_REG_2);
    register volatile umword_t r3 asm(ARCH_REG_3);
    register volatile umword_t r4 asm(ARCH_REG_4);

    mk_syscall(syscall_prot_create(SYS_INFO2_GET, SYS_PROT, obj).raw,
               flags,
               0,
               0,
               0,
               0,
               0);
    asm __volatile__(""
                     :
                     : : ARCH_REG_0, ARCH_REG_1, ARCH_REG_2, ARCH_REG_3, ARCH_REG_4);
    if (info)
    {
        info->resv_dtbo = r1;
        info->cpu_usage_cur = r2;
    }

    return msg_tag_init(r0);
}
MK_SYSCALL
msg_tag_t sys_reboot(obj_handler_t obj)
{
    register volatile umword_t r0 asm(ARCH_REG_0);
    register volatile umword_t r1 asm(ARCH_REG_1);
    register volatile umword_t r2 asm(ARCH_REG_2);

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
                     : : ARCH_REG_0, ARCH_REG_1, ARCH_REG_2);

    return tag;
}
MK_SYSCALL
msg_tag_t sys_mem_info(obj_handler_t obj, umword_t *total, umword_t *free)
{
    register volatile umword_t r0 asm(ARCH_REG_0);
    register volatile umword_t r1 asm(ARCH_REG_1);
    register volatile umword_t r2 asm(ARCH_REG_2);

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
                     : : ARCH_REG_0, ARCH_REG_1, ARCH_REG_2);

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
MK_SYSCALL
msg_tag_t sys_dis_info(obj_handler_t obj, umword_t irq_inx)
{
    register volatile umword_t r0 asm(ARCH_REG_0);

    mk_syscall(syscall_prot_create(DIS_IRQ, SYS_PROT, obj).raw,
               irq_inx,
               0,
               0,
               0,
               0,
               0);
    return msg_tag_init(r0);
}
