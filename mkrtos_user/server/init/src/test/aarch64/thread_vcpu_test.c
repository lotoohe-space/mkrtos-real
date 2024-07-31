
#include <u_thread.h>
#include <u_hd_man.h>
#include <u_task.h>
#include <errno.h>
#include <u_prot.h>
#include <u_factory.h>
#include <u_vmam.h>
#include <string.h>

#define STACK_SIZE 4096
static __attribute__((aligned(8))) uint8_t stack0[STACK_SIZE];

extern void thread_vcpu_test_fn(void);

int thread_vcpu_test(void)
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
    tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RW, 0, 0),
                       PAGE_SIZE, 0, (addr_t *)(&msg_buf_addr));
    if (msg_tag_get_val(tag) < 0)
    {
        ret = msg_tag_get_val(tag);
        goto end;
    }
    memset((void *)msg_buf_addr, 0, PAGE_SIZE);

    tag = thread_exec_regs(th1_hd, (umword_t)thread_vcpu_test_fn,
                           (umword_t)stack0 + STACK_SIZE - sizeof(void *),
                           TASK_RAM_BASE(), 0);
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
    thread_run(th1_hd, 2); // 优先级默认为2
    ret = 0;
    goto end_ok;
end_free_mm:
    u_vmam_free(VMA_PROT, msg_buf_addr, PAGE_SIZE);
end:
    handler_free_umap(th1_hd);
end_ok:
    return ret;
}
