#include "u_types.h"
#include "syscall_backend.h"
#include "u_prot.h"
#include "u_ipc.h"
#include "u_mm.h"
#include "u_app.h"
#include "cons_cli.h"
#include "u_arch.h"
#include "u_vmam.h"
#include <assert.h>
#undef hidden
#undef weak
#define hidden
#include <pthread_impl.h>
umword_t be_mmap(void *start,
                 size_t len,
                 long prot,
                 long flags,
                 long fd,
                 long _offset)
{
    addr_t addr;

    if (len == 0)
    {
        return 0;
    }
    len = ALIGN(len, PAGE_SIZE);
    // if ((len & ((1UL << CONFIG_PAGE_SHIFT) - 1)) != 0)
    // {
    //     return -EINVAL;
    // }
    if (fd != -1)
    {
        return -ENOSYS;
    }
    msg_tag_t tag;
    uint8_t addr_prot = 0;

    if (prot & PROT_READ)
    {
        addr_prot |= VPAGE_PROT_RO;
    }
    if (prot & PROT_WRITE)
    {
        addr_prot |= VPAGE_PROT_WO;
    }
    if (prot & PROT_EXEC)
    {
        addr_prot |= VPAGE_PROT_X;
    }
    if (prot & PROT_NACC)
    {
        addr_prot |= VPAGE_PROT_N_ACCESS_FLAG;
    }
    if (prot & PROT_UNCACHE)
    {
        addr_prot |= VPAGE_PROT_UNCACHE;
    }
    tag = u_vmam_alloc(VMA_PROT, vma_addr_create(VPAGE_PROT_RWX/*TODO:暂时不支持属性修改接口*/, 0, 0),
                       len, 0, &addr);
    if (msg_tag_get_val(tag) < 0)
    {
        return (umword_t)msg_tag_get_val(tag);
    }
    return addr;
}
long sys_mmap(va_list ap)
{
    void *start;
    size_t len;
    long prot;
    long flags;
    long fd;
    long _offset;

    ARG_6_BE(ap, start, void *, len, size_t, prot, long, flags, long, fd, long, _offset, long);

    return be_mmap(start, len, prot, flags, fd, _offset);
}
umword_t be_munmap(void *start, size_t len)
{
    if ((len & (CONFIG_PAGE_SHIFT - 1)) != 0)
    {
        return -EINVAL;
    }
    msg_tag_t tag;

    tag = u_vmam_free(VMA_PROT, (addr_t)start, len);

    return msg_tag_get_val(tag);
}
long sys_munmap(va_list ap)
{
    void *start;
    size_t len;

    ARG_2_BE(ap, start, void *, len, size_t);
    return be_munmap(start, len);
}
