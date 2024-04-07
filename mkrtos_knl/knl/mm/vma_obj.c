/**
 * @file irq_sender.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <arch.h>
#include <types.h>
#include <kobject.h>
#include <thread.h>
#include <ref.h>
#include <init.h>
#include <mm_wrap.h>
#include <factory.h>
#include <irq.h>
#include <task.h>
#include <fcntl.h>
#include <vma.h>
#include <globals.h>
typedef struct vma_obj
{
    kobject_t kobj;
    vma_t vmam;
} vma_obj_t;

enum
{
    VMA_ALLOC,
    VMA_FREE,
};

static void vma_obj_syscall(kobject_t *kobj, syscall_prot_t sys_p, msg_tag_t in_tag, entry_frame_t *f)
{
    assert(kobj);
    assert(f);
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);
    thread_t *th = thread_get_current();
    task_t *tk = thread_get_current_task();
    vma_obj_t *irq = container_of(kobj, vma_obj_t, kobj);

    if (sys_p.prot != VMA_PROT)
    {
        f->regs[0] = msg_tag_init4(0, 0, 0, -EPROTO).raw;
        return;
    }

    switch (sys_p.op)
    {
    case VMA_ALLOC:
    {
        int ret;
        vaddr_t ret_vaddr = 0;

        ret = task_vma_alloc(&tk->mm_space.mem_vma,
                             vam_addr_create_raw(f->regs[0]), f->regs[1],
                             f->regs[2], &ret_vaddr);
        f->regs[0] = msg_tag_init4(0, 0, 0, ret).raw;
        f->regs[1] = ret_vaddr;
    }
    break;
    case VMA_FREE:
    {
        int ret;

        task_vma_free(&tk->mm_space.mem_vma, f->regs[0], f->regs[1]);
        f->regs[0] = msg_tag_init4(0, 0, 0, ret).raw;
    }
    break;
    }
}
static bool_t vma_obj_put(kobject_t *kobj)
{
    return 0;
}
static void vma_obj_stage1(kobject_t *kobj)
{
}
static void vma_obj_stage2(kobject_t *kobj)
{
}
/**
 * @brief 初始化
 *
 * @param irq
 */
void vma_obj_init(vma_obj_t *obj)
{
    kobject_init(&obj->kobj, VMAM_TYPE);
    obj->kobj.invoke_func = vma_obj_syscall;
    obj->kobj.stage_1_func = vma_obj_stage1;
    obj->kobj.stage_2_func = vma_obj_stage2;
    obj->kobj.put_func = vma_obj_put;
}
static vma_obj_t vma_obj;
/**
 * @brief 初始化一个根工厂对象
 *
 */
static void root_factory_init(void)
{
    vma_obj_init(&vma_obj);
    global_reg_kobj(&vma_obj.kobj, VMA_PROT);
}
INIT_KOBJ(root_factory_init);
