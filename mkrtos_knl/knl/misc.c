
#include "types.h"
#include "mm_wrap.h"
#include "mm.h"
#include "mm_space.h"
#include "util.h"
#include "mpu.h"
#include "printk.h"
static bool_t mpu_calc(
    mm_space_t *ms,
    umword_t mem_start_addr,
    int size,
    umword_t *ret_align_size,
    umword_t *alloc_addr)
{
    int ffs_t_;
    int ffs_t;
    region_info_t *region[2];

    region[0] = mm_space_alloc_pt_region(ms);
    region[1] = mm_space_alloc_pt_region(ms);

    if (!region[0] || !region[1])
    {
        if (!region[0])
        {
            mm_space_free_pt_region(ms, region[0]);
        }
        if (!region[1])
        {
            mm_space_free_pt_region(ms, region[1]);
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

#if 1
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
    mpu_calc_regs(region[0], region[0]->block_start_addr, ffs_t_, REGION_RWX, region[0]->region);
    mpu_calc_regs(region[1], region[1]->block_start_addr, ffs_t_, REGION_RWX, region[1]->region);
    // region_i[2].rbar = ARM_MPU_RBAR(2, 0x20010000/*TODO:*/);
    // region_i[2].rasr = ARM_MPU_RASR(0UL, ARM_MPU_AP_URO, 0UL
    // 				, 0UL, 1UL, 1UL, 0x0/*TODO:*/, 16-1/*TODO:*/);

    // region_i[2].rbar = ARM_MPU_RBAR(2, 0x8000000 /*TODO:*/);
    // region_i[2].rasr = ARM_MPU_RASR(0UL, ARM_MPU_AP_URO, 0UL, 0UL, 1UL, 1UL, 0x0 /*TODO:*/, ffs(1024 * 1024) - 1 /*TODO:*/);
    return TRUE;
}

void *mpu_ram_alloc(mm_space_t *ms, ram_limit_t *r_limit, size_t ram_size)
{
    umword_t pre_alloc_addr;
    struct mem_heap *heap = NULL;
    /*TODO:临界区保护*/
again_alloc:
    heap = mm_get_free(heap, ram_size, &pre_alloc_addr);
    if (!heap)
    {
        printk("The system is low on memory.\n");
        mm_trace();
        return NULL;
    }

    umword_t need_align;
    umword_t alloc_addr;

    if (mpu_calc(ms, pre_alloc_addr, ram_size,
                 &need_align, &alloc_addr) == FALSE)
    {
        printk("The MPU area is exhausted.");
        return NULL;
    }

    void *ram = mm_limit_alloc_align(r_limit, ram_size, need_align);
    if (!ram)
    {
        printk("The system is low on memory.\n");
        return NULL;
    }

    //!< 申请的地址与预分配的地址不同
    if (ram != (void *)alloc_addr)
    {
        printk("Again.\n");
        mm_limit_free_align(r_limit, ram, need_align);
        heap = heap->next;
        goto again_alloc;
    }
    return ram;
}
