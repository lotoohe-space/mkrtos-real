
#include <u_thread.h>
#include <u_hd_man.h>
#include <u_task.h>
#include <errno.h>
#include <u_prot.h>
#include <u_factory.h>
#include <u_vmam.h>
#include <string.h>
#include <stdio.h>
#include "guest_os.h"
#include "ipa_exeception.h"

#define VCPU_DEFAULT_PRIO 2

int guest_os_add_dev(guest_os_t *guest, vmm_dev_t *dev)
{
    for (int i = 0; i < VMM_DEV_LIST; i++)
    {
        if (guest->devs[i] == NULL)
        {
            // dev->ref_cnt++;
            guest->devs[i] = dev;
            return 0;
        }
    }
    return -1;
}
vmm_dev_t* guest_os_find_region(guest_os_t *guest, paddr_t addr)
{
    for (int i = 0; i < VMM_DEV_LIST; i++)
    {
        if (guest->devs[i] == NULL)
        {
            continue;
        }
        if (addr >= guest->devs[i]->devtree_node->addr &&
            addr < guest->devs[i]->devtree_node->addr + guest->devs[i]->devtree_node->size)
        {
            return guest->devs[i];
        }
    }
    return NULL;
}

int guest_os_create(guest_os_t *gos, char *cfg, addr_t st_addr, void *entry, void *bin_end, size_t mem_size)
{
    msg_tag_t tag;

    obj_handler_t th1_hd;
    umword_t msg_buf_addr;
    int ret;

    th1_hd = handler_alloc();
    if (th1_hd == HANDLER_INVALID)
    {
        return -ENOENT;
    }
    tag = factory_create_thread_vcpu(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, th1_hd));
    if (msg_tag_get_prot(tag) < 0)
    {
        handler_free(th1_hd);
        return msg_tag_get_prot(tag);
    }
    gos->os_mem = NULL;
    task_set_obj_name(TASK_THIS, th1_hd, "vcpu_th");
    tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RWX, 0, st_addr),
                       mem_size, 0, (addr_t *)(&gos->os_mem));
    if (msg_tag_get_val(tag) < 0)
    {
        ret = msg_tag_get_val(tag);
        goto end;
    }

    ret = uvmm_devtree_scan(gos, cfg); //扫描设备树，并进行初始化
    if (ret < 0)
    {
        goto end;
    }

    printf("os bin size is %d.\n", (char *)bin_end - (char *)entry);
    memcpy(gos->os_mem, entry, (char *)bin_end - (char *)entry);
    tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RW, 0, 0),
                       PAGE_SIZE, 0, (addr_t *)(&msg_buf_addr));
    if (msg_tag_get_val(tag) < 0)
    {
        ret = msg_tag_get_val(tag);
        goto end;
    }
    memset((void *)msg_buf_addr, 0, PAGE_SIZE);

    tag = thread_exec_regs(th1_hd, (umword_t)gos->os_mem,
                           0, TASK_RAM_BASE(), 0);
    if (msg_tag_get_prot(tag) < 0)
    {
        ret = msg_tag_get_prot(tag);
        goto end_free_mm;
    }
    tag = thread_bind_task(th1_hd, TASK_THIS);
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_free_mm;
    }
    tag = thread_msg_buf_set(th1_hd, (void *)msg_buf_addr);
    if (msg_tag_get_prot(tag) < 0)
    {
        goto end_free_mm;
    }
    ret = ipc_execetion_create(gos);
    if (ret < 0)
    {
        goto end_free_mm;
    }
    thread_set_exec(th1_hd, gos->ipa_execption_ipc);
    gos->msg_buf = (void *)msg_buf_addr;
    gos->prio = VCPU_DEFAULT_PRIO;
    gos->vcpu.vcpu_id = 0; // TODO:需要设置内核的寄存器
    gos->vcpu.vcpu_th = th1_hd;
    gos->vcpu.guest = gos;
    gos->vcpu.name = "vcpu0";
    ipc_msg_t *msg = (ipc_msg_t *)msg_buf_addr;
    msg->user[3] = (umword_t)(&gos->vcpu);

    thread_run(th1_hd, VCPU_DEFAULT_PRIO); // 优先级默认为2
    ret = 0;
    goto end_ok;
end_free_mm:
    u_vmam_free(VMA_PROT, msg_buf_addr, PAGE_SIZE);
end:
    if (gos->os_mem)
    {
        u_vmam_free(VMA_PROT, (addr_t)(gos->os_mem), mem_size);
    }
    handler_free_umap(th1_hd);
end_ok:
    return ret;
}
