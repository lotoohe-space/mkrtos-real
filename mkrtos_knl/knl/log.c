/**
 * @file log.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-14
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
#include "irq_sender.h"
#include "uart/uart.h"
typedef struct log
{
    irq_sender_t kobj;
} log_t;
static log_t log;

enum log_op
{
    WRITE_DATA,
    READ_DATA,
    SET_FLAGS
};
static void log_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f);

static void log_trigger(irq_entry_t *irq)
{
    uart_tigger(irq);
}

static void log_reg(void)
{
    // kobject_init(&log.kobj);
    irq_sender_init(&log.kobj);
    log.kobj.kobj.kobj_type = IRQ_SENDER_TYPE;
    log.kobj.kobj.invoke_func = log_syscall;
    log.kobj.kobj.stage_1_func = kobject_release_stage1;
    log.kobj.kobj.stage_2_func = kobject_release_stage2;
    log.kobj.kobj.put_func = kobject_put;
    global_reg_kobj(&log.kobj.kobj, LOG_PROT);
    irq_alloc(38 /*USART1_IRQn*/, &log.kobj, log_trigger);
}
INIT_KOBJ(log_reg);
static msg_tag_t log_write_data(log_t *log, const char *data, int len)
{
    for (int i = 0; i < len && data[i]; i++)
    {
#if 0
        if (data[i] == '\n')
        {
            putc('\r');
        }
#endif
        putc(data[i]);
    }
    return msg_tag_init(0);
}
static int log_read_data(log_t *log, uint8_t *data, int len)
{
    for (int i = 0; i < len; i++)
    {
    again:;
        int c = getc();

        if (c < 0 && i == 0)
        {
            int ret = irq_sender_wait(&log->kobj, thread_get_current(), 0);
            if (ret < 0)
            {
                return ret;
            }
            goto again;
        }
        else if (c < 0)
        {
            return i;
        }
        data[i] = (uint8_t)c;
    }
    return len;
}
static void
log_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);
    if (sys_p.prot == LOG_PROT)
    {

        switch (sys_p.op)
        {
        case WRITE_DATA:
            tag = log_write_data(
                (log_t *)kobj, (const char *)(&f->r[1]),
                MIN(in_tag.msg_buf_len * WORD_BYTES, WORD_BYTES * 5));
            break;
        case READ_DATA:
        {
            int ret = log_read_data((log_t *)kobj, (uint8_t *)(&f->r[1]), MIN(f->r[1], WORD_BYTES * 5));
            tag = msg_tag_init4(0, 0, 0, ret);
        }
        break;
        case SET_FLAGS:
            printk("don't support set flags.\n");
            break;
        default:
            tag = msg_tag_init4(0, 0, 0, -ENOSYS);
            break;
        }
        f->r[0] = tag.raw;
    }
    else if (sys_p.prot == IRQ_PROT)
    {
        irq_sender_syscall(kobj, sys_p, in_tag, f);
    }
    else
    {
        f->r[0] = msg_tag_init4(0, 0, 0, -EPROTO).raw;
        return;
    }
    return;
}

void log_dump(void)
{
}