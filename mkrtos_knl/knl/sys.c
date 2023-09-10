/*
 * @Author: zhangzheng 1358745329@qq.com
 * @Date: 2023-08-14 09:47:54
 * @LastEditors: zhangzheng 1358745329@qq.com
 * @LastEditTime: 2023-08-14 13:10:35
 * @FilePath: /mkrtos-real/mkrtos_knl/knl/log.c
 * @Description: 内核的信息输出对象，用户态使用该对象进行数据输出。
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

static sys_t sys_boj;

enum sys_op
{
    SYS_INFO_GET,
    REBOOT,
};
static void sys_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f);

static void sys_reg(void)
{
    kobject_init(&sys_boj.kobj);
    sys_boj.kobj.invoke_func = sys_syscall;
    global_reg_kobj(&sys_boj.kobj, SYS_PROT);
}
INIT_KOBJ(sys_reg);

static void sys_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);
    if (sys_p.prot != LOG_PROT)
    {
        f->r[0] = msg_tag_init4(0, 0, 0, -EPROTO).raw;
        return;
    }
    switch (sys_p.op)
    {
    case SYS_INFO_GET:
    {
        f->r[1] = sys_tick_cnt_get();
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