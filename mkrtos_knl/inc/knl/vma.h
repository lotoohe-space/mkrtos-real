#pragma once
#include <types.h>
#include <util.h>
#include <arch.h>
#include <rbtree_mm.h>

enum vpage_prot_attrs
{
    VPAGE_PROT_RO = 1, //!< 只读
    VPAGE_PROT_WO = 0x2,
    VPAGE_PROT_RW = VPAGE_PROT_RO | VPAGE_PROT_WO, //!< 读写
    VPAGE_PROT_X = 0x4,
    VPAGE_PROT_RWX = (VPAGE_PROT_RO | VPAGE_PROT_RW | VPAGE_PROT_X), //!< 读写执行
    VPAGE_PROT_N_ACCESS_FLAG = 0x08,                                 //!< 访问是否缺页，设置代表缺页

    VPAGE_PROT_UNCACHE = 0x10, //!< 不使用缓存
    VPAGE_PROT_IN_KNL = 0x20,  //!< 内核中使用
};

#define VMA_ADDR_RESV 0x1    //!< 保留内存
// #define VMA_ADDR_UNCACHE 0x2 //!< uncache内存

#define VMA_USED_NODE 0x1 //!< 该vma节点被使用，非空闲

typedef union vma_addr
{
    umword_t raw;
    struct
    {
        umword_t prot : 8;
        umword_t flags : 4;
        // umword_t resv : 2;
        umword_t addr : (sizeof(void *) * 8 - PAGE_SHIFT);
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
        // .resv = 0,
        .addr = addr >> PAGE_SHIFT,
    };
}
static inline addr_t vma_addr_get_addr(vma_addr_t vaddr)
{
    return MASK_LSB(vaddr.raw, PAGE_SHIFT);
}
static inline void vma_addr_set_addr(vma_addr_t *vaddr, addr_t addr)
{
    vaddr->addr = addr >> PAGE_SHIFT;
}
static inline uint8_t vma_addr_get_prot(vma_addr_t vaddr)
{
    return vaddr.prot;
}
static inline void vma_addr_set_prot(vma_addr_t *vaddr, uint8_t prot)
{
    vaddr->prot = prot;
}
static inline uint8_t vma_addr_get_flags(vma_addr_t vaddr)
{
    return vaddr.flags;
}
static inline void vma_addr_set_flags(vma_addr_t *vaddr, uint8_t flags)
{
    vaddr->flags = flags;
}

typedef struct vma
{
    vma_addr_t vaddr;
    size_t size; //!< 节点的大小
    union
    {
        umword_t paddr_raw;
        struct
        {
            umword_t flags : 12;
            umword_t paddr : (sizeof(void *) * 8 - 12); //!< 被分配后的节点才使用改数据，表示分配的物理内存。
        };
    };
} vma_t;

static inline paddr_t vma_node_get_paddr(vma_t *data)
{
    return data->paddr << PAGE_SHIFT;
}
static inline void vma_node_set_paddr(vma_t *data, paddr_t addr)
{
    data->paddr = addr >> PAGE_SHIFT;
}
static inline bool_t vma_node_get_used(vma_t *data)
{
    return !!(data->flags & VMA_USED_NODE);
}
static inline void vma_node_set_used(vma_t *data)
{
    data->flags |= VMA_USED_NODE;
}
static inline void vma_node_set_unused(vma_t *data)
{
    data->flags &= ~(VMA_USED_NODE);
}

typedef struct task_vma
{
    mln_rbtree_t idle_tree;
    mln_rbtree_t alloc_tree;
} task_vma_t;

int task_vma_init(task_vma_t *vma);

/**
 * 分配步骤
 * 1.找到一个空闲的合适的虚拟内存区域
 * 2.用申请的区域去分割找到的区域
 * 3.将申请的区域插入回去，并设置属性
 */
int task_vma_alloc(task_vma_t *task_vma, vma_addr_t vaddr, size_t size,
                   paddr_t paddr, vaddr_t *ret_vaddr);
/**
 * @brief 将一个task的内存转移给另一个task
 * 1.查找源中是否存在
 * 2.查找目的中是否存在
 * 3.虚拟地址节点从源中删除，插入到目的
 * 4.物理地址从源中解除映射，并从树中删除，插入到目的，并映射到目的端。
 * @param src_task_vma
 * @param dst_task_vma
 * @param src_addr
 * @param size
 * @return int
 */
int task_vma_grant(task_vma_t *src_task_vma, task_vma_t *dst_task_vma,
                   vaddr_t src_addr, vaddr_t dst_addr, size_t size);
/**
 * @brief 释放申请的虚拟内存，并释放已经申请的物理内存
 * 1.从分配树中找到需要的节点
 * 2.迭代释放物理内存
 * 3.设置节点为空闲节点，并与周围的合并
 * @param task_vma
 * @param addr
 * @param size
 * @return int
 */
int task_vma_free(task_vma_t *task_vma, vaddr_t addr, size_t size);

/**
 * @brief 缺页的处理流程
 * 1.查找已经分配的表中是否存在
 * 2.分配物理内存
 * 3.插入到已分配树中去
 * @param task_vma
 * @param addr
 * @return int
 */
int task_vma_page_fault(task_vma_t *task_vma, vaddr_t addr);

/**
 * @brief 释放task_vma
 *
 * @param task_vma
 * @return int
 */
int task_vma_clean(task_vma_t *task_vma);
