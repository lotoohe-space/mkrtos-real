

#include <vma.h>
#include <types.h>
#include <util.h>
#include <assert.h>
#include <err.h>
#include <mpu.h>
#include <mm.h>
#include <mm_wrap.h>
#include <task.h>
#include <thread.h>
#include <string.h>
#include <spinlock.h>
static region_info_t *vma_alloc_pt_region(task_vma_t *vma)
{
    assert(vma != NULL);
    mword_t status = spinlock_lock(&vma->lock);
    for (int i = 0; i < CONFIG_REGION_NUM; i++)
    {
        if (vma->pt_regions[i].region_inx < 0)
        {
            vma->pt_regions[i].region_inx = (int16_t)i;
            spinlock_set(&vma->lock, status);
            return &vma->pt_regions[i];
        }
    }
    spinlock_set(&vma->lock, status);
    return NULL;
}
static region_info_t *vma_find_pt_region(task_vma_t *vma, addr_t addr, size_t size)
{
    assert(vma);
    mword_t status = spinlock_lock(&vma->lock);
    for (int i = 0; i < CONFIG_REGION_NUM; i++)
    {
        if (vma->pt_regions[i].region_inx < 0)
        {
            continue;
        }
        if (vma->pt_regions[i].start_addr == addr && vma->pt_regions[i].size == size)
        {
            spinlock_set(&vma->lock, status);
            return vma->pt_regions + i;
        }
    }
    spinlock_set(&vma->lock, status);
    return NULL;
}
static void vma_free_pt_region(task_vma_t *vma, region_info_t *ri)
{
    mword_t status = spinlock_lock(&vma->lock);
    memset(ri, 0, sizeof(*ri));
    ri->region_inx = -1;
    spinlock_set(&vma->lock, status);
}
/**
 * @brief 初始化vma
 *
 * @param vma vma结构体
 * @return int 0表示成功 <0表示失败
 */
int task_vma_init(task_vma_t *vma)
{
#if IS_ENABLED(CONFIG_MK_MPU_CFG)
    assert(vma != NULL);

    spinlock_init(&vma->lock);
    for (int i = 0; i < CONFIG_REGION_NUM; i++)
    {
        vma->pt_regions[i].region_inx = -1;
    }
#if IS_ENABLED(MPU_PAGE_FAULT_SUPPORT)
    for (int i = 0; i < MPU_PAGE_FAULT_REGIONS_NUM; i++)
    {
        vma->mem_pages_pt_regions[i] = vma_alloc_pt_region(vma);
        if (vma->mem_pages_pt_regions[i] == NULL)
        {
            printk("pt regions alloc is failed.\n");
            assert(vma->mem_pages_pt_regions[i]);
        }
    }
    // vma->mem_pages_bitamp = 0;
    memset(vma->mem_pages, 0, sizeof(vma->mem_pages));
#endif
#endif
    return 0;
}
/**
 * @brief 为task增加内存，需要用到mpu的region
 *
 * @param task_vma
 * @param vaddr
 * @param size
 * @param paddr
 * @param ret_vaddr
 * @return int
 */
int task_vma_alloc(task_vma_t *task_vma, vma_addr_t vaddr, size_t size,
                   paddr_t paddr, vaddr_t *ret_vaddr)
{
    addr_t vma_addr;
    task_t *pt_task;
    int ret = 0;
    size_t mem_align_size;
    bool_t is_alloc_mem = FALSE;

    pt_task = container_of(
        container_of(task_vma, mm_space_t, mem_vma),
        task_t,
        mm_space);

    assert(task_vma != NULL);
#if IS_ENABLED(CONFIG_MK_MPU_CFG)
#if CONFIG_MPU_VERSION == 1
#if IS_ENABLED(MPU_PAGE_FAULT_SUPPORT) //!< 缺页模拟
    if (vma_addr_get_flags(vaddr) & VMA_ADDR_PAGE_FAULT_SIM)
    {
        mem_align_size = PAGE_SIZE;
    }
    else
#endif
    {
        mem_align_size = size;
    }
#elif CONFIG_MPU_VERSION == 2
#if IS_ENABLED(MPU_PAGE_FAULT_SUPPORT) //!< 缺页模拟
    if (vma_addr_get_flags(vaddr) & VMA_ADDR_PAGE_FAULT_SIM)
    {
        mem_align_size = PAGE_SIZE;
    }
    else
#endif
    {
        mem_align_size = MPU_ALIGN_SIZE;
    }
#endif
#else
    mem_align_size = sizeof(void *) * 2;
#endif
    if (size & (mem_align_size - 1))
    {
        // 不是MPU_ALIGN_SIZE字节对齐的
        return -EINVAL;
    }
    vma_addr = vma_addr_get_addr(vaddr);
    if (vma_addr != 0)
    {
        printk("vam vaddr must is NULL.\n");
        // 必须让内核自己分配地址
        ret = -EINVAL;
        goto err_end;
    }
    mword_t status = spinlock_lock(&task_vma->lock);

    if (paddr == 0)
    {
        // 未设置物理内存，则自动申请一个
        vma_addr = (addr_t)mm_limit_alloc_align(pt_task->lim, size, mem_align_size);
        assert(((vma_addr & (mem_align_size - 1))) == 0);
        is_alloc_mem = TRUE;
    }
    else
    {
        // 设置了物理内存，就用设置的物理内存
        vma_addr = paddr;
        if ((vma_addr & (mem_align_size - 1)) != 0)
        {
            // 不是对齐的
            ret = -EINVAL;
            goto err_end;
        }
    }
    if (vma_addr == 0)
    {
        ret = -ENOMEM;
        goto err_end;
    }
#if IS_ENABLED(MPU_PAGE_FAULT_SUPPORT) //!< 缺页模拟
    if (vma_addr_get_flags(vaddr) & VMA_ADDR_PAGE_FAULT_SIM)
    {
        size_t alloc_cn = 0;
        int i;

        for (i = 0; i < MPU_PAGE_NUM; i++)
        {
            if (task_vma->mem_pages[i] == NULL) //!< 放到内存页中保存
            {
                task_vma->mem_pages[i] = (void *)(vma_addr + alloc_cn * PAGE_SIZE);
                task_vma->mem_pages_attrs[i] = vma_addr_get_prot(vaddr);
                if (alloc_cn == 0)
                {
                    task_vma->mem_pages_alloc_size[i] = size;
                }
                else
                {
                    task_vma->mem_pages_alloc_size[i] = 0;
                }
                alloc_cn++;
                if (alloc_cn * PAGE_SIZE >= size)
                {
                    break;
                }
            }
        }
        if (i == MPU_PAGE_NUM)
        {
            ret = -ENOMEM;
            goto err_end;
        }
    }
    else
#endif
    {
        // 找到一个合适的区域
        region_info_t *ri = NULL;

        ri = vma_find_pt_region(task_vma, vma_addr, size);
        if (ri)
        {
            // 已经存在
            ret = -EEXIST;
            goto err_end;
        }

        ri = vma_alloc_pt_region(task_vma);
        if (!ri)
        {
            // 没有找到合适的区域
            ret = -ENOMEM;
            goto err_end;
        }
        mpu_calc_regs(ri, vma_addr, size,
                      vpage_attrs_to_page_attrs(vma_addr_get_prot(vaddr)),
                      0 /*TODO:支持每一个region bit位*/);
        ri->start_addr = vma_addr;
        ri->size = size;
        if (pt_task == thread_get_current_task())
        {
            // 只有是当前线程的时候发生切换
            mpu_switch_to_task(pt_task);
        }
    }
    if (ret_vaddr)
    {
        *ret_vaddr = vma_addr;
    }
    goto end;
err_end:
    if (is_alloc_mem)
    {
        mm_limit_free_align(pt_task->lim, (void *)vma_addr, mem_align_size);
    }
end:
    spinlock_set(&task_vma->lock, status);
    return ret;
}
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
                   vaddr_t src_addr, vaddr_t dst_addr, size_t size)
{
    return -ENOSYS;
}
static int task_vma_free_inner(task_vma_t *task_vma, vaddr_t vaddr, size_t size, bool_t is_free_mem)
{
    region_info_t *ri = NULL;
    addr_t vma_addr;
    task_t *pt_task;
    assert(task_vma);
#if IS_ENABLED(MPU_PAGE_FAULT_SUPPORT) //!< 缺页模拟
    if (vaddr & (PAGE_SIZE - 1))
    {
        return -EINVAL;
    }
    if ((size & (PAGE_SIZE - 1)))
    {
        return -EINVAL;
    }
#endif
    mword_t status = spinlock_lock(&task_vma->lock);

    pt_task = container_of(
        container_of(task_vma, mm_space_t, mem_vma),
        task_t,
        mm_space);
    vma_addr = vaddr;
    ri = vma_find_pt_region(task_vma, vma_addr, size);
#if IS_ENABLED(MPU_PAGE_FAULT_SUPPORT) //!< 缺页模拟
    for (int i = 0; i < MPU_PAGE_FAULT_REGIONS_NUM; i++)
    {
        if (ri == task_vma->mem_pages_pt_regions[i])
        {
            ri = NULL;
            break;
        }
    }
#endif
    if (!ri)
    {
#if IS_ENABLED(MPU_PAGE_FAULT_SUPPORT) //!< 缺页模拟
        size_t mem_cn = 0;
        for (int i = 0; i < MPU_PAGE_NUM; i++)
        {
            if (task_vma->mem_pages[i] == (void *)(vaddr + mem_cn * PAGE_SIZE))
            {
                if (task_vma->mem_pages_alloc_size[i] != 0)
                {
                    // 是头部，则释放内存
                    mm_limit_free_align(pt_task->lim, (void *)vma_addr, PAGE_SIZE);
                    task_vma->mem_pages_alloc_size[i] = 0;
                }
                task_vma->mem_pages[i] = NULL;
                ri = vma_find_pt_region(task_vma, (addr_t)(vaddr + mem_cn * PAGE_SIZE), PAGE_SIZE);
                if (ri)
                {
                    ri->rbar = 0;
                    ri->rasr = 0;
                }
            }
            mem_cn++;
            if (mem_cn * PAGE_SIZE >= size)
            {
                goto end;
            }
        }
#endif
        return -ENOENT;
    }
    vma_free_pt_region(task_vma, ri);
    if (is_free_mem)
    {
#if IS_ENABLED(CONFIG_MK_MPU_CFG)
#if CONFIG_MPU_VERSION == 1
        mm_limit_free_align(pt_task->lim, (void *)vma_addr, size);
#elif CONFIG_MPU_VERSION == 2
        mm_limit_free_align(pt_task->lim, (void *)vma_addr, MPU_ALIGN_SIZE);
#endif
#else
        mm_limit_free_align(pt_task->lim, (void *)vma_addr, sizeof(void *) * 2);
#endif
    }
end:
    if (pt_task == thread_get_current_task())
    {
        mpu_switch_to_task(pt_task);
    }
    spinlock_set(&task_vma->lock, status);
    return 0;
}
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
int task_vma_free(task_vma_t *task_vma, vaddr_t vaddr, size_t size)
{
    return task_vma_free_inner(task_vma, vaddr, size, TRUE);
}

/**
 * @brief 释放掉已经申请的物理内存，但是不释放虚拟内存
 *
 * @param task_vma
 * @param addr
 * @param size 释放的大小
 * @param is_free_mem 是否释放内存
 * @return int
 */
int task_vma_free_pmem(task_vma_t *task_vma, vaddr_t addr, size_t size, bool_t is_free_mem)
{
    return task_vma_free_inner(task_vma, addr, size, is_free_mem);
}
/**
 * @brief 缺页的处理流程
 * 1.查找已经分配的表中是否存在
 * 2.分配物理内存
 * 3.插入到已分配树中去
 * @param task_vma
 * @param addr
 * @return int
 */
int task_vma_page_fault(task_vma_t *task_vma, vaddr_t addr, void *paddr)
{
    assert(task_vma);
    assert((addr & (PAGE_SIZE - 1)) == 0);
    task_t *pt_task;
    assert(task_vma);
    int ret = -ENOENT;

    pt_task = container_of(
        container_of(task_vma, mm_space_t, mem_vma),
        task_t,
        mm_space);

#if IS_ENABLED(MPU_PAGE_FAULT_SUPPORT) //!< 缺页模拟
    mword_t status = spinlock_lock(&task_vma->lock);
    for (int i = 0; i < MPU_PAGE_NUM; i++)
    {
        if (task_vma->mem_pages[i] == NULL)
        {
            continue;
        }
        if ((umword_t)(task_vma->mem_pages[i]) == addr)
        {
            int sel_region = task_vma->pt_regions_sel % MPU_PAGE_FAULT_REGIONS_NUM;

            mpu_calc_regs(task_vma->mem_pages_pt_regions[sel_region], (umword_t)(task_vma->mem_pages[i]), PAGE_SIZE,
                          vpage_attrs_to_page_attrs(task_vma->mem_pages_attrs[i]),
                          0 /*TODO:支持每一个region bit位*/);
            task_vma->mem_pages_pt_regions[sel_region]->start_addr = addr;
            task_vma->mem_pages_pt_regions[sel_region]->size = PAGE_SIZE;
            task_vma->pt_regions_sel++;
            ret = 0;
            break;
        }
    }
    if (pt_task == thread_get_current_task())
    {
        mpu_switch_to_task(pt_task);
    }
    spinlock_set(&task_vma->lock, status);
    return ret;
#else
    return -ENOSYS;
#endif
}

/**
 * @brief 释放task_vma
 *
 * @param task_vma
 * @return int
 */
int task_vma_clean(task_vma_t *task_vma)
{
    task_t *pt_task;

    pt_task = container_of(
        container_of(task_vma, mm_space_t, mem_vma),
        task_t,
        mm_space);
    for (int i = 0; i < CONFIG_REGION_NUM; i++)
    {
        if (task_vma->pt_regions[i].region_inx >= 0)
        {
            task_vma->pt_regions[i].region_inx = -1;
#if CONFIG_MPU_VERSION == 1
            mm_limit_free_align(pt_task->lim, (void *)(task_vma->pt_regions[i].start_addr),
                                task_vma->pt_regions[i].size);
#elif CONFIG_MPU_VERSION == 2
            mm_limit_free_align(pt_task->lim, (void *)(task_vma->pt_regions[i].start_addr),
                                MPU_ALIGN_SIZE);
#endif
        }
    }
    return 0;
}

#if CONFIG_MK_MPU_CFG
#include "mpu.h"
static bool_t mpu_calc(
    mm_space_t *ms,
    umword_t mem_start_addr,
    int size,
    umword_t *ret_align_size,
    umword_t *alloc_addr)
{
#if CONFIG_MPU_VERSION == 1
    int ffs_t_;
    int ffs_t;
    region_info_t *region[2];

    region[0] = vma_alloc_pt_region(&ms->mem_vma);
    region[1] = vma_alloc_pt_region(&ms->mem_vma);

    if (!region[0] || !region[1])
    {
        if (!region[0])
        {
            vma_free_pt_region(&ms->mem_vma, region[0]);
        }
        if (!region[1])
        {
            vma_free_pt_region(&ms->mem_vma, region[1]);
        }
        return FALSE;
    }

    ffs_t_ = ffs(size);
    if (!is_power_of_2(size))
    {
        ffs_t_++;
    }
    ffs_t = 1 << ffs_t_;

    int sub_region_t = ffs_t >> 3;
    int align_sub_size = ALIGN(size, sub_region_t);

    umword_t mem_align_sub_mem_addr = ALIGN((umword_t)mem_start_addr, sub_region_t);
    umword_t mem_align_up_mem_addr = ALIGN((umword_t)mem_start_addr, ffs_t);
    umword_t mem_align_down_mem_addr = ALIGN_DOWN((umword_t)mem_start_addr, ffs_t);

    region[0]->start_addr = mem_align_sub_mem_addr;
    region[0]->size = mem_align_up_mem_addr - mem_align_sub_mem_addr;
    region[0]->block_size = ffs_t;
    region[0]->block_start_addr = mem_align_down_mem_addr;

    if (alloc_addr)
    {
        *alloc_addr = region[0]->start_addr;
    }

    region[0]->region = 0xff;
    for (umword_t i = mem_align_down_mem_addr; i < mem_align_up_mem_addr; i += sub_region_t)
    {
        if (i < mem_align_sub_mem_addr)
        {
            region[0]->region |= 1 << ((i - mem_align_down_mem_addr) / sub_region_t);
        }
        else
        {
            region[0]->region &= ~(1 << ((i - mem_align_down_mem_addr) / sub_region_t));
        }
    }

    region[1]->region = 0x00;
    for (umword_t i = mem_align_up_mem_addr; i < mem_align_up_mem_addr + ffs_t; i += sub_region_t)
    {
        if (i < mem_align_sub_mem_addr + align_sub_size)
        {
            region[1]->region &= ~(1 << ((i - mem_align_up_mem_addr) / sub_region_t));
        }
        else
        {
            region[1]->region |= (1 << ((i - mem_align_up_mem_addr) / sub_region_t));
        }
    }
    region[1]->start_addr = mem_align_up_mem_addr;
    region[1]->size = (mem_align_sub_mem_addr + align_sub_size) - mem_align_up_mem_addr;
    region[1]->block_size = ffs_t;
    region[1]->block_start_addr = mem_align_up_mem_addr;

    *ret_align_size = sub_region_t;

#if 0
    printk("st:0x%x re:0x%x sub:0x%x\n region:[", region[0]->block_start_addr, region[0]->region, sub_region_t);
    for (int i = 0; i < 8; i++)
    {
        if (region[0]->region & (1 << i))
        {
            printk("x");
        }
        else
        {
            printk("o");
        }
    }
    printk("]\n");
    printk("st:0x%x re:0x%x sub:0x%x\n region:[", region[1]->block_start_addr, region[1]->region, sub_region_t);
    for (int i = 0; i < 8; i++)
    {
        if (region[1]->region & (1 << i))
        {
            printk("x");
        }
        else
        {
            printk("o");
        }
    }
    printk("]\n");
#endif
    mpu_calc_regs(region[0], region[0]->block_start_addr, 1 << ffs_t_, REGION_RWX, region[0]->region);
    mpu_calc_regs(region[1], region[1]->block_start_addr, 1 << ffs_t_, REGION_RWX, region[1]->region);
#elif CONFIG_MPU_VERSION == 2

#endif
    return TRUE;
}

void *mpu_ram_alloc(mm_space_t *ms, ram_limit_t *r_limit, size_t ram_size, int mem_block)
{
#if CONFIG_MPU_VERSION == 1
    umword_t pre_alloc_addr;
    struct mem_heap *heap = NULL;
    umword_t status = cpulock_lock();
again_alloc:
    heap = mm_get_free_raw(mem_block, heap, ram_size, &pre_alloc_addr);
    if (!heap)
    {
        cpulock_set(status);
        printk("The system is low on memory.\n");
        // mm_trace();
        return NULL;
    }

    umword_t need_align;
    umword_t alloc_addr;

    if (mpu_calc(ms, pre_alloc_addr, ram_size,
                 &need_align, &alloc_addr) == FALSE)
    {
        cpulock_set(status);
        printk("The MPU area is exhausted.");
        return NULL;
    }

    void *ram = mm_limit_alloc_align_raw(mem_block, r_limit, ram_size, need_align);
    if (!ram)
    {
        cpulock_set(status);
        printk("The system is low on memory.\n");
        return NULL;
    }

    //!< 申请的地址与预分配的地址不同
    if (ram != (void *)alloc_addr)
    {
        cpulock_set(status);
        printk("Again.\n");
        mm_limit_free_align_raw(mem_block, r_limit, ram, need_align);
        heap = heap->next;
        goto again_alloc;
    }
    cpulock_set(status);
    return ram;
#elif CONFIG_MPU_VERSION == 2
    region_info_t *region;

    void *ram = mm_limit_alloc_align_raw(mem_block, r_limit, ram_size + MPU_ALIGN_SIZE, MPU_ALIGN_SIZE);
    if (!ram)
    {
        printk("The system is low on memory.\n");
        return NULL;
    }
    region = mm_space_alloc_pt_region(ms);
    if (!region)
    {
        mm_limit_free_align_raw(mem_block, r_limit, ram, ram_size);
        return NULL;
    }
    region->block_start_addr = (umword_t)ram;
    region->start_addr = (umword_t)ram;
    region->block_size = 0;
    region->size = ram_size + MPU_ALIGN_SIZE;
    mpu_calc_regs(region, region->block_start_addr, ram_size & (~(MPU_ALIGN_SIZE - 1)), REGION_RWX, region->region);
    return ram;
#endif
}
#else
void *mpu_ram_alloc(mm_space_t *ms, ram_limit_t *r_limit, size_t ram_size, int mem_block)
{
    void *ram = mm_limit_alloc_raw(mem_block, r_limit, ram_size);

    return ram;
}
#endif
