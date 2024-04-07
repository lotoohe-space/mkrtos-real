#pragma once
#include <u_types.h>
#include <u_prot.h>

enum vpage_prot_attrs
{
    VPAGE_PROT_RO = 1,                                      //!< 只读
    VPAGE_PROT_RW = VPAGE_PROT_RO | 0x2,                    //!< 读写
    VPAGE_PROT_RWX = (VPAGE_PROT_RO | VPAGE_PROT_RW | 0x4), //!< 读写执行
};

#define VMA_ADDR_RESV 0x1    //!< 保留内存
#define VMA_ADDR_UNCACHE 0x2 //!< uncache内存

typedef union vma_addr
{
    umword_t raw;
    struct
    {
        umword_t prot : 4;
        umword_t flags : 4;
        umword_t resv : 4;
        umword_t addr : (sizeof(void *) * 8 - CONFIG_PAGE_SHIFT);
    };
} vma_addr_t;

static inline vma_addr_t vam_addr_create_raw(umword_t raw)
{
    return (vma_addr_t){
        .raw = raw,
    };
}
static inline vma_addr_t vma_addr_create(uint8_t prot, uint8_t flags, umword_t addr)
{
    return (vma_addr_t){
        .prot = prot,
        .flags = flags,
        .resv = 0,
        .addr = addr >> CONFIG_PAGE_SHIFT,
    };
}
msg_tag_t u_vmam_alloc(obj_handler_t obj, vma_addr_t addr, size_t size, addr_t paddr, addr_t *vaddr);
msg_tag_t u_vmam_free(obj_handler_t obj, addr_t addr, size_t size);
