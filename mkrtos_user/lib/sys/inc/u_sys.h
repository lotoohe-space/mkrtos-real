#pragma once

#include "u_types.h"
#include "u_prot.h"
typedef struct sys_info
{
    umword_t sys_tick;
    umword_t bootfs_start_addr;
    umword_t sys_clk;
} sys_info_t;

#define SYS_FLAGS_MAP_CPIO_FS 0x01

msg_tag_t sys_read_info(obj_handler_t obj, sys_info_t *info, umword_t flags);
msg_tag_t sys_reboot(obj_handler_t obj);
msg_tag_t sys_mem_info(obj_handler_t obj, umword_t *total, umword_t *free);
msg_tag_t sys_dis_info(obj_handler_t obj, umword_t irq_inx);

static inline umword_t sys_read_tick(void)
{
    sys_info_t info;

    sys_read_info(SYS_PROT, &info, 0);
    return info.sys_tick;
}
static inline umword_t sys_read_clk(void)
{
    sys_info_t info;

    sys_read_info(SYS_PROT, &info, 0);
    return info.sys_clk;
}
