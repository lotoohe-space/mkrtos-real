/**
 * @file sys.c
 * @author ATShining (1358745329@qq.com)
 * @brief 内核的信息输出对象，用户态使用该对象进行数据输出。
 * @version 0.1
 * @date 2023-09-16
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "log.h"
#include "factory.h"
#include "kobject.h"
#include "globals.h"
#include "init.h"
#include "printk.h"
#include "types.h"
#include "util.h"
#include "arch.h"

typedef struct sys
{
    kobject_t kobj;
} sys_t;

static sys_t sys_obj;

enum sys_op
{
    SYS_INFO_GET,
    REBOOT,
};
static void sys_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f);

static void sys_reg(void)
{
    kobject_init(&sys_obj.kobj, SYS_TYPE);
    sys_obj.kobj.invoke_func = sys_syscall;
    global_reg_kobj(&sys_obj.kobj, SYS_PROT);
}
INIT_KOBJ(sys_reg);

static void sys_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);
    if (sys_p.prot != SYS_PROT)
    {
        f->r[0] = msg_tag_init4(0, 0, 0, -EPROTO).raw;
        return;
    }
    switch (sys_p.op)
    {
    case SYS_INFO_GET:
    {
        f->r[1] = sys_tick_cnt_get();
        f->r[2] = KNL_TEXT + BOOTFS_ADDR_OFFSET;
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    case REBOOT:
    {
        printk("Unsupport.\n");
        tag = msg_tag_init4(0, 0, 0, -ENOSYS);
    }
    break;
    default:
        tag = msg_tag_init4(0, 0, 0, -ENOSYS);
        break;
    }
    f->r[0] = tag.raw;
    return;
}

void sys_dump(void)
{
}