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
#include "arch.h"
#include "factory.h"
#include "globals.h"
#include "init.h"
#include "kobject.h"
#include "log.h"
#include "mm_wrap.h"
#include "printk.h"
#include "types.h"
#include "util.h"
#if IS_ENABLED(CONFIG_MMU)
#include <vma.h>
#endif
#include <cpio.h>
#include <boot_info.h>
#include <globals.h>
#include <task.h>
#include <thread.h>
#include <thread_knl.h>

typedef struct sys
{
    kobject_t kobj;
} sys_t;

static sys_t sys_obj;

enum sys_op
{
    SYS_INFO_GET,
    SYS_INFO2_GET,
    REBOOT,
    MEM_INFO,
    DIS_IRQ,
};
#define SYS_FLAGS_MAP_CPIO_FS 0x1
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
    int ret = 0;
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);
    if (sys_p.prot != SYS_PROT)
    {
        f->regs[0] = msg_tag_init4(0, 0, 0, -EPROTO).raw;
        return;
    }
    switch (sys_p.op)
    {
    case SYS_INFO_GET:
    {
        umword_t flags = f->regs[0];
        addr_t cpio_addr;

        f->regs[1] = sys_tick_cnt_get();
#if IS_ENABLED(CONFIG_MMU)
        if (flags & SYS_FLAGS_MAP_CPIO_FS)
        {
            ret = task_vma_alloc(&(thread_get_current_task()->mm_space.mem_vma),
                                 vma_addr_create(VPAGE_PROT_RO, VMA_ADDR_RESV, 0),
                                 cpio_get_size(cpio_images), (paddr_t)cpio_images, &cpio_addr);
            f->regs[2] = cpio_addr;
        }
        else
        {
            f->regs[2] = CONFIG_BOOT_FS_VADDR;
        }
#else
        f->regs[2] = arch_get_boot_info()->flash_layer.flash_layer_list[BOOTFS_LAYER_3].st_addr;
#endif
        f->regs[3] = arch_get_sys_clk();

        tag = msg_tag_init4(0, 0, 0, ret);
    }
    break;
    case SYS_INFO2_GET:
    {
#if IS_ENABLED(CONFIG_DTBO_SUPPORT)
        f->regs[1] = arch_get_boot_info()->flash_layer.flash_layer_list[DTBO_LAYER_1].st_addr;
#else
        f->regs[1] = 0;
#endif
        f->regs[2] = cpu_get_current_usage();
        tag = msg_tag_init4(0, 0, 0, ret);
        break;
    }
    case REBOOT:
    {
        printk("sys reboot.\n");
        sys_reset();
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    case MEM_INFO:
    {
        size_t total;
        size_t free;

        mm_info(&total, &free);
        f->regs[1] = total;
        f->regs[2] = free;
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    case DIS_IRQ:
    {
        arch_disable_irq(f->regs[0]);
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    default:
        tag = msg_tag_init4(0, 0, 0, -ENOSYS);
        break;
    }
    f->regs[0] = tag.raw;
    return;
}

void sys_dump(void)
{
}