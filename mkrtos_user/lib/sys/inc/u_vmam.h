#pragma once
#include <u_types.h>
#include <u_prot.h>

enum vpage_prot_attrs
{
    VPAGE_PROT_RO = 1, //!< 只读
    VPAGE_PROT_WO = 0x2,
    VPAGE_PROT_RW = VPAGE_PROT_RO | VPAGE_PROT_WO, //!< 读写
    VPAGE_PROT_X = 0x4,
    VPAGE_PROT_RWX = (VPAGE_PROT_RO | VPAGE_PROT_RW | VPAGE_PROT_X), //!< 读写执行
    VPAGE_PROT_N_ACCESS_FLAG = 0x8,                                 //!< 访问是否缺页，设置代表缺页

    VPAGE_PROT_UNCACHE = 0x10, //!< 不使用缓存
    VPAGE_PROT_IN_KNL = 0x20,  //!< 内核中使用
};

#define VMA_ADDR_RESV 0x1           //!< flags 保留内存
#define VMA_ADDR_PAGE_FAULT_SIM 0x2 //!< page fault模拟，使用mpu模拟，该内存不能被栈使用

typedef union vma_addr
{
    umword_t raw;
    struct
    {
#if IS_ENABLED(CONFIG_MMU)
        umword_t prot : 8;
        umword_t flags : 4;
#else
        umword_t prot : 6;
        umword_t flags : 3;
#endif
        // umword_t resv : 2;
        umword_t addr : (sizeof(void *) * 8 - CONFIG_PAGE_SHIFT);
    };
} vma_addr_t;

static inline vma_addr_t vma_addr_create_raw(umword_t raw)
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
        // .resv = 0,
        .addr = addr >> CONFIG_PAGE_SHIFT,
    };
}
msg_tag_t u_vmam_alloc(obj_handler_t obj, vma_addr_t addr, size_t size, addr_t paddr, addr_t *vaddr);
msg_tag_t u_vmam_grant(obj_handler_t obj, obj_handler_t dst_task_obj,
                       addr_t src_addr, addr_t dst_addr, size_t size);
msg_tag_t u_vmam_free(obj_handler_t obj, addr_t addr, size_t size);
