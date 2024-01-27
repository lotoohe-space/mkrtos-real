/**
 * @file mm_man.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "types.h"
#include "kobject.h"
#include "init.h"
#include "mm_page.h"
#include "thread.h"
#include "task.h"
#include "globals.h"
#include "mpu.h"
typedef struct mm_man
{
    kobject_t kobj;
} mm_man_t;
static mm_man_t mm_man;

static void mm_man_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f);

static void mm_man_reg(void)
{
    kobject_init(&mm_man.kobj, MM_TYPE);
    mm_man.kobj.invoke_func = mm_man_syscall;
    global_reg_kobj(&mm_man.kobj, MM_PROT);
}
INIT_KOBJ(mm_man_reg);

enum mm_op
{
    MM_ALLOC,
    MM_FREE,
    MM_ALIGN_ALLOC, //!< 直接暂用一个region
    MM_ALIGN_FREE,
    MM_MOD_ATTRS,
};

static void mm_man_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    task_t *cur_task = thread_get_current_task();
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);

    if (sys_p.prot != MM_PROT)
    {
        f->r[0] = msg_tag_init4(0, 0, 0, -EPROTO).raw;
        return;
    }
    switch (sys_p.op)
    {
    case MM_ALLOC:
    {
#if 0
        addr_t ret_addr;
        int ret = mm_pages_alloc_page(&cur_task->mm_space.mm_pages, cur_task->lim, f->r[1], &ret_addr, f->r[2]);
        if (ret < 0)
        {
            tag = msg_tag_init4(0, 0, 0, ret);
        }
        else
        {
            tag = msg_tag_init4(0, 0, 0, 0);
            f->r[1] = ret_addr;
        }
#else
        tag = msg_tag_init4(0, 0, 0, -ENOSYS);
#endif
    }
    break;
    case MM_FREE:
    {
#if 0
        mm_pages_free_page(&cur_task->mm_space.mm_pages, cur_task->lim, f->r[1], f->r[2]);
        tag = msg_tag_init4(0, 0, 0, 0);
#else
        tag = msg_tag_init4(0, 0, 0, -ENOSYS);
#endif
    }
    break;
    case MM_ALIGN_ALLOC:
    {
        region_info_t *regi_info = mm_space_alloc_pt_region(&cur_task->mm_space);

        if (regi_info)
        {
            umword_t size = f->r[2];
            umword_t addr = f->r[1];

#if CONFIG_MPU_VERSION == 1
            if ((!is_power_of_2(size)) && ((addr & (~(size - 1))) != 0))
            {
                tag = msg_tag_init4(0, 0, 0, -EINVAL);
                break;
            }
#elif CONFIG_MPU_VERSION == 2
            if ((size & (MPU_ALIGN_SIZE - 1)) == 0 && (addr & (MPU_ALIGN_SIZE - 1)) == 0)
            {
                tag = msg_tag_init4(0, 0, 0, -EINVAL);
                break;
            }
#endif
            mpu_calc_regs(regi_info, addr, size, REGION_RWX, 0);
            mpu_switch_to_task(cur_task);
            tag = msg_tag_init4(0, 0, 0, 0);
        }
        else
        {
            tag = msg_tag_init4(0, 0, 0, -ENOMEM);
        }
    }
    break;
    case MM_MOD_ATTRS:
    {
        tag = msg_tag_init4(0, 0, 0, -ENOSYS); // TODO:
    }
    break;
    default:
        tag = msg_tag_init4(0, 0, 0, -ENOSYS);
        break;
    }
    f->r[0] = tag.raw;
}
void mm_man_dump(void)
{
}
