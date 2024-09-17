

#include <vma.h>
#include <types.h>
#include <util.h>
#include <assert.h>
#include <err.h>
#include <mpu.h>
static region_info_t *vma_alloc_pt_region(task_vma_t *vma)
{
    assert(vma != NULL);
    /*TODO:加锁*/
    for (int i = 0; i < CONFIG_REGION_NUM; i++)
    {
        if (vma->pt_regions[i].region_inx < 0)
        {
            vma->pt_regions[i].region_inx = (int16_t)i;
            return &vma->pt_regions[i];
        }
    }
    return NULL;
}
static region_info_t *vma_find_pt_region(task_vma_t *vma, addr_t addr, size_t size)
{
    assert(vma);
    for (int i = 0; i < CONFIG_REGION_NUM; i++)
    {
        if (vma->pt_regions[i].region_inx < 0)
        {
            continue;
        }
        if (vma->pt_regions[i].start_addr == addr && vma->pt_regions[i].size == size)
        {
            return vma->pt_regions + i;
        }
    }
    return NULL;
}
static void vma_free_pt_region(task_vma_t *vma, region_info_t *ri)
{
    ri->region_inx = -1;
}

/**
 * @brief 初始化vma
 *
 * @param vma vma结构体
 * @return int 0表示成功 <0表示失败
 */
int task_vma_init(task_vma_t *vma)
{
    assert(vma != NULL);
    for (int i = 0; i < CONFIG_REGION_NUM; i++)
    {
        vma->pt_regions[i].region_inx = -1;
    }
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

    pt_task = container_of(
        container_of(task_vma, mm_space_t, mem_vma),
        task_t,
        mm_space);

    assert(task_vma != NULL);
    if (!ret_vaddr)
    {
        return -EINVAL;
    }
#if CONFIG_MPU_VERSION == 1
    if (!is_power_of_2(size))
    {
        // 不是2的幂
        return -EINVAL;
    }
#elif CONFIG_MPU_VERSION == 2
    if (size & (MPU_ALIGN_SIZE - 1))
    {
        // 不是MPU_ALIGN_SIZE字节对齐的
        return -EINVAL;
    }
#endif
    vma_addr = vma_addr_get_addr(vaddr);
    if (vma_addr != 0)
    {
        // 必须让内核自己分配地址
        return -EINVAL;
    }

#if CONFIG_MPU_VERSION == 1
    vma_addr = (addr_t)mm_limit_alloc_align(pt_task, size, size);
    if (vma_addr == 0)
    {
        return -ENOMEM;
    }
    if ((vma_addr & (size - 1)) != 0)
    {
        // 不是对齐的
        ret = -EINVAL;
        goto err_end;
    }
#elif CONFIG_MPU_VERSION == 2
    vma_addr = (addr_t)mm_limit_alloc_align(pt_task, size, MPU_ALIGN_SIZE);
    if (vma_addr == 0)
    {
        return -ENOMEM;
    }
    if (vma_addr & (MPU_ALIGN_SIZE - 1))
    {
        // 不是对齐的
        ret = -EINVAL;
        goto err_end;
    }
#endif
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
    mpu_switch_to_task(
        pt_task);
err_end:
#if CONFIG_MPU_VERSION == 1
    mm_limit_free_align(pt_task, vma_addr, size);
#elif CONFIG_MPU_VERSION == 2
    mm_limit_free_align(pt_task, vma_addr, MPU_ALIGN_SIZE);
#endif
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
    region_info_t *ri = NULL;
    addr_t vma_addr;
    task_t *pt_task;

    pt_task = container_of(
        container_of(task_vma, mm_space_t, mem_vma),
        task_t,
        mm_space);
    assert(task_vma);
    vma_addr = vma_addr_get_addr(vaddr);
    ri = vma_find_pt_region(task_vma, vma_addr, size);
    if (!ri)
    {
        return -ENOENT;
    }
    vma_free_pt_region(task_vma, ri);
#if CONFIG_MPU_VERSION == 1
    mm_limit_free_align(pt_task, vma_addr, size);
#elif CONFIG_MPU_VERSION == 2
    mm_limit_free_align(pt_task, vma_addr, MPU_ALIGN_SIZE);
#endif
    mpu_switch_to_task(
        container_of(
            container_of(task_vma, mm_space_t, mem_vma),
            task_t,
            mm_space));

    return 0;
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
    return -ENOSYS;
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
    return -ENOSYS;
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
            mm_limit_free_align(pt_task, task_vma->pt_regions[i].start_addr, task_vma->pt_regions[i].size);
#elif CONFIG_MPU_VERSION == 2
            mm_limit_free_align(pt_task, task_vma->pt_regions[i].start_addr, MPU_ALIGN_SIZE);
#endif
        }
    }
    return 0;
}
