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
static log_t log;

enum log_op
{
    WRITE_DATA,
    READ_DATA,
    SET_FLAGS
};
static void log_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f);

static void log_reg(void)
{
    kobject_init(&log.kobj);
    log.kobj.invoke_func = log_syscall;
    global_reg_kobj(&log.kobj, LOG_PROT);
}
INIT_KOBJ(log_reg);
static msg_tag_t log_write_data(log_t *log, const char *data, int len)
{
    for (int i = 0; i < len && data[i]; i++)
    {
        putc(data[i]);
    }
    return msg_tag_init(0);
}
static void
log_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);
    if (sys_p.prot != LOG_PROT)
    {
        f->r[0] = msg_tag_init4(0, 0, 0, -EPROTO).raw;
        return;
    }
    switch (sys_p.op)
    {
    case WRITE_DATA:
        tag = log_write_data(
            (log_t *)kobj, (const char *)(&f->r[1]),
            MIN(in_tag.msg_buf_len * WORD_BYTES, WORD_BYTES * 5));
        break;
    case READ_DATA:
        printk("don't support read data.\n");
        break;
    case SET_FLAGS:
        printk("don't support set flags.\n");
        break;
    default:
        tag = msg_tag_init4(0, 0, 0, -ENOSYS);
        break;
    }
    f->r[0] = tag.raw;
    return;
}

void log_dump(void)
{
}