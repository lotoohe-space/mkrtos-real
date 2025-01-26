

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

mword_t task_vma_lock(task_vma_t *task_vma)
{
    task_t *task;
    mword_t ret;

    task = container_of(container_of(task_vma, mm_space_t, mem_vma), task_t, mm_space);

    ret = spinlock_lock(&task->kobj.lock);
    if (ret < 0)
    {
        return ret;
    }
    return ret;
}
void task_vma_unlock(task_vma_t *task_vma, mword_t status)
{
    task_t *task;
    mword_t ret;

    task = container_of(container_of(task_vma, mm_space_t, mem_vma), task_t, mm_space);
    spinlock_set(&task->kobj.lock, status);
}

static region_info_t *vma_alloc_pt_region(task_vma_t *vma)
{
    assert(vma != NULL);
    mword_t status = task_vma_lock(vma);
    for (int i = 0; i < CONFIG_REGION_NUM; i++)
    {
        if (vma->pt_regions[i].region_inx < 0)
        {
            vma->pt_regions[i].region_inx = (int16_t)i;
            task_vma_unlock(vma, status);
            return &vma->pt_regions[i];
        }
    }
    task_vma_unlock(vma, status);
    return NULL;
}
#if IS_ENABLED(CONFIG_MPU)
static region_info_t *vma_find_pt_region(task_vma_t *vma, addr_t addr, size_t size)
{
    assert(vma);
    mword_t status = task_vma_lock(vma);
    for (int i = 0; i < CONFIG_REGION_NUM; i++)
    {
        if (vma->pt_regions[i].region_inx < 0)
        {
            continue;
        }
        if (vma->pt_regions[i].start_addr == addr && vma->pt_regions[i].size == size)
        {
            task_vma_unlock(vma, status);
            return vma->pt_regions + i;
        }
    }
    task_vma_unlock(vma, status);
    return NULL;
}
static void vma_free_pt_region(task_vma_t *vma, region_info_t *ri)
{
    assert(vma);
    assert(ri);
    mword_t status = task_vma_lock(vma);
    memset(ri, 0, sizeof(*ri));
    ri->region_inx = -1;
    task_vma_unlock(vma, status);
}
static void vma_clean_pt_region(task_vma_t *vma, region_info_t *ri)
{
    assert(vma);
    assert(ri);
    mword_t status = task_vma_lock(vma);
    int rg = ri->region_inx;
    memset(ri, 0, sizeof(*ri));
    ri->region_inx = rg;
    task_vma_unlock(vma, status);
}
#endif

static mln_rbtree_node_t *vma_node_create(mln_rbtree_t *r_tree, vma_addr_t addr, size_t size, paddr_t paddr)
{
    mln_rbtree_node_t *node = NULL;
    vma_t *data_vma = mm_alloc(sizeof(vma_t));

    if (data_vma == NULL)
    {
        return NULL;
    }
    data_vma->vaddr = addr;
    data_vma->size = size;
    data_vma->paddr_raw = paddr;

    node = mln_rbtree_node_new(r_tree, data_vma);

    if (mln_rbtree_null(node, r_tree))
    {
        return NULL;
    }

    return node;
}
static void vma_node_free(mln_rbtree_t *r_tree, mln_rbtree_node_t *node)
{
    mm_free(mln_rbtree_node_data_get(node));
    mln_rbtree_node_free(r_tree, node);
}
/**
 * @brief 初始化vma
 *
 * @param vma vma结构体
 * @return int 0表示成功 <0表示失败
 */
int task_vma_init(task_vma_t *vma)
{
#if IS_ENABLED(CONFIG_MPU)
    assert(vma != NULL);

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
#endif
#endif
    rbtree_mm_init(&vma->alloc_tree);
    return 0;
}
typedef struct vma_idl_tree_insert_params
{
    vaddr_t addr;
    size_t size;
} vma_idl_tree_insert_params_t;
/**
 * 返回值：
 * -1 第一个参数小于第二个参数
 * 0 两个参数相同
 * 1 第一个参数大于第二个参数
 */
static int vma_idl_tree_alloc_cmp_handler(const void *key, const void *data)
{
    vma_idl_tree_insert_params_t *key_p = (vma_idl_tree_insert_params_t *)key;
    vma_t *data_p = (vma_t *)data;

    if (key_p->addr + key_p->size <= vma_addr_get_addr(data_p->vaddr))
    {
        return -1;
    }
    else if (key_p->addr >= vma_addr_get_addr(data_p->vaddr) &&
             (key_p->addr + key_p->size) <= vma_addr_get_addr(data_p->vaddr) + data_p->size)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
static int vma_idl_tree_insert_cmp_handler(const void *key, const void *data)
{
    vma_t *key_p = (vma_t *)key;
    vma_t *data_p = (vma_t *)data;

    if (vma_addr_get_addr(key_p->vaddr) + key_p->size <= vma_addr_get_addr(data_p->vaddr))
    {
        return -1;
    }
    else if (vma_addr_get_addr(key_p->vaddr) >= vma_addr_get_addr(data_p->vaddr) &&
             (vma_addr_get_addr(key_p->vaddr) + key_p->size) <= vma_addr_get_addr(data_p->vaddr) + data_p->size)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
static inline bool_t task_vam_addr_size_check(vma_addr_t vaddr, size_t size)
{
#if IS_ENABLED(CONFIG_MPU)
#if CONFIG_MPU_VERSION == 1 || CONFIG_MPU_VERSION == 2
    if (vma_addr_get_flags(vaddr) & VMA_ADDR_PAGE_FAULT_DSCT)
    {
        // 按照页大小映射
        if (size & (PAGE_SIZE - 1))
        {
            return FALSE;
        }
        if (vma_addr_get_addr(vaddr) & (PAGE_SIZE - 1))
        {
            return FALSE;
        }
    }
    else
    {
        // 否则整页映射，效率更高，但是更加费内存
        if (!is_power_of_2(size))
        {
            return FALSE;
        }
#if CONFIG_MPU_VERSION == 1
        if (vma_addr_get_addr(vaddr) & (size - 1))
        {
            return FALSE;
        }
#else
        if (vma_addr_get_addr(vaddr) & (MPU_ALIGN_SIZE - 1))
        {
            return FALSE;
        }
#endif
    }
    return TRUE;
#else
#error "CONFIG_MPU_VERSION is error."
#endif
#else
    if (vma_addr_get_addr(vaddr) & (sizeof(void *) - 1))
    {
        return FALSE;
    }
    return TRUE;
#endif
}
static inline size_t task_vma_get_mem_align_size(vma_addr_t vaddr, size_t size)
{
    size_t mem_align_size;
#if IS_ENABLED(CONFIG_MPU)
#if CONFIG_MPU_VERSION == 1
    if (vma_addr_get_flags(vaddr) & VMA_ADDR_PAGE_FAULT_DSCT)
    {
        mem_align_size = PAGE_SIZE;
    }
    else
    {
        mem_align_size = size;
    }
#elif CONFIG_MPU_VERSION == 2
#if IS_ENABLED(MPU_PAGE_FAULT_SUPPORT) //!< 缺页模拟
    if (vma_addr_get_flags(vaddr) & VMA_ADDR_PAGE_FAULT_DSCT)
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
    mem_align_size = sizeof(void *);
#endif
    return mem_align_size;
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
    mem_align_size = task_vma_get_mem_align_size(vaddr, size);
    if (size & (mem_align_size - 1))
    {
        // 不是MPU_ALIGN_SIZE字节对齐的
        return -EINVAL;
    }
    vma_addr_t temp_vma_addr = vma_addr_create_raw(vaddr.raw);
    vma_addr_set_addr(&temp_vma_addr, paddr);
    if (task_vam_addr_size_check(temp_vma_addr, size) == FALSE)
    {
        return -EINVAL;
    }
    vma_addr = vma_addr_get_addr(vaddr);
    if (vma_addr != 0)
    {
        printk("vam vaddr must is not NULL.\n");
        // 必须让内核自己分配地址
        ret = -EINVAL;
        goto err_end;
    }
    mword_t status = task_vma_lock(task_vma);

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
        // 手动设置了物理内存，必须是保留内存，保留内存自己释放
        vma_addr_set_flags(&vaddr, vma_addr_get_flags(vaddr) | VMA_ADDR_RESV);
    }
    if (vma_addr == 0)
    {
        ret = -ENOMEM;
        goto err_end;
    }
    if (vma_addr_get_flags(vaddr) & VMA_ADDR_PAGE_FAULT_SIM)
    {
        mln_rbtree_node_t *node;
        vma_t *node_data;

        vma_addr_set_addr(&vaddr, vma_addr); // 物理地址和虚拟化地址设置为一样
        node = vma_node_create(&task_vma->alloc_tree, vaddr, size, vma_addr);
        if (!node)
        {
            ret = -ENOMEM;
            goto err_end;
        }
        node_data = mln_rbtree_node_data_get(node);

        vma_node_set_used(node_data);
        task_vma->alloc_tree.cmp = vma_idl_tree_insert_cmp_handler;
        mln_rbtree_insert(&task_vma->alloc_tree, node);
    }
#if IS_ENABLED(CONFIG_MPU)
    else
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
#endif
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
    task_vma_unlock(task_vma, status);
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
 * @brief 完全相等
 * 返回值：
 * -1 第一个参数小于第二个参数
 * 0 两个参数相同
 * 1 第一个参数大于第二个参数
 */
static int vma_idl_tree_eq_cmp_handler(const void *key, const void *data)
{
    vma_idl_tree_insert_params_t *key_p = (vma_idl_tree_insert_params_t *)key;
    vma_t *data_p = (vma_t *)data;

    if (key_p->addr + key_p->size <= vma_addr_get_addr(data_p->vaddr))
    {
        return -1;
    }
    else if (key_p->addr == vma_addr_get_addr(data_p->vaddr) &&
             (key_p->addr + key_p->size) == vma_addr_get_addr(data_p->vaddr) + data_p->size)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
static inline int task_vma_addr_check(vaddr_t addr, size_t size)
{
}
static int task_vma_free_inner(task_vma_t *task_vma, vaddr_t vaddr, size_t size, bool_t is_free_mem)
{
    addr_t vma_addr;
    task_t *pt_task;
    int ret = 0;
    assert(task_vma);

    // if (task_vam_addr_size_check(vaddr, size) == FALSE)
    // {
    //     return -EINVAL;
    // }

    mword_t status = task_vma_lock(task_vma);

    pt_task = container_of(
        container_of(task_vma, mm_space_t, mem_vma),
        task_t,
        mm_space);
    vma_addr = vaddr;
#if IS_ENABLED(MPU_PAGE_FAULT_SUPPORT) //!< 先查看是否在缺页region中，如果是则走模拟流程
    region_info_t *ri = NULL;

    ri = vma_find_pt_region(task_vma, vma_addr, size);
    for (int i = 0; i < MPU_PAGE_FAULT_REGIONS_NUM; i++)
    {
        if (ri == task_vma->mem_pages_pt_regions[i])
        {
            vma_clean_pt_region(task_vma, ri);
            ri = NULL;
            break;
        }
    }
    if (ri)
    {
        vma_free_pt_region(task_vma, ri);
    }
    else
#endif
    {
        mln_rbtree_node_t *find_node;

        // 在红黑树中查看内存地址
        task_vma->alloc_tree.cmp = vma_idl_tree_eq_cmp_handler;
        find_node = mln_rbtree_search(
            &task_vma->alloc_tree,
            &(vma_idl_tree_insert_params_t){
                .size = size,
                .addr = vaddr,
            }); //!< 查找是否存在
        if (mln_rbtree_null(find_node, &task_vma->alloc_tree))
        {
            ret = -ENOENT;
            goto end;
        }
        // mm_trace();
        mln_rbtree_delete(&task_vma->alloc_tree, find_node);
        vma_node_free(&task_vma->alloc_tree, find_node);
        vma_t *vma_data = mln_rbtree_node_data_get(find_node);

        if (vma_addr_get_flags(vma_data->vaddr) & VMA_ADDR_RESV)
        {
            // 保留内存不能释放
            is_free_mem = FALSE;
        }
    }

    if (is_free_mem)
    {
        mm_limit_free_align(pt_task->lim, (void *)vma_addr, size);
    }
end:
    if (pt_task == thread_get_current_task())
    {
        mpu_switch_to_task(pt_task);
    }
    task_vma_unlock(task_vma, status);
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
 * 返回值：
 * -1 第一个参数小于第二个参数
 * 0 两个参数相同
 * 1 第一个参数大于第二个参数
 */
static int vma_idl_tree_wrap_cmp_handler(const void *key, const void *data)
{
    vma_idl_tree_insert_params_t *key_p = (vma_idl_tree_insert_params_t *)key;
    vma_t *data_p = (vma_t *)data;

    if (key_p->addr + key_p->size <= vma_addr_get_addr(data_p->vaddr))
    {
        return -1;
    }
    else if (key_p->addr >= vma_addr_get_addr(data_p->vaddr) &&
             (key_p->addr + key_p->size) <= vma_addr_get_addr(data_p->vaddr) + data_p->size)
    {
        return 0;
    }
    else
    {
        return 1;
    }
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
#if IS_ENABLED(CONFIG_MPU) && IS_ENABLED(MPU_PAGE_FAULT_SUPPORT)
    assert(task_vma);
    task_t *pt_task;
    mln_rbtree_node_t *find_node = NULL;
    int ret = 0;
    vma_t *node_data = NULL;

    pt_task = container_of(
        container_of(task_vma, mm_space_t, mem_vma),
        task_t,
        mm_space);

    mword_t status = task_vma_lock(task_vma);

    task_vma->alloc_tree.cmp = vma_idl_tree_wrap_cmp_handler;
    find_node = mln_rbtree_search(
        &task_vma->alloc_tree,
        &(vma_idl_tree_insert_params_t){
            .size = sizeof(void *), /*最多一次访问4字节，这里按4字节查找即可*/
            .addr = addr,
        }); //!< 查找是否存在
    if (mln_rbtree_null(find_node, &task_vma->alloc_tree))
    {
        ret = -ENOENT;
        goto end;
    }
    node_data = mln_rbtree_node_data_get(find_node);
    int sel_region = task_vma->pt_regions_sel % MPU_PAGE_FAULT_REGIONS_NUM;
    addr_t map_addr;
    size_t map_size;

    if (vma_addr_get_flags(node_data->vaddr) & VMA_ADDR_PAGE_FAULT_DSCT)
    {
        map_addr = ALIGN_DOWN(addr, PAGE_SIZE);
        map_size = PAGE_SIZE;
    }
    else
    {
        map_addr = (umword_t)(vma_addr_get_addr(node_data->vaddr));
        map_size = node_data->size;
    }
    mpu_calc_regs(task_vma->mem_pages_pt_regions[sel_region],
                  map_addr,
                  map_size,
                  vpage_attrs_to_page_attrs(vma_addr_get_prot(node_data->vaddr)),
                  0 /*TODO:支持每一个region bit位*/);
    task_vma->mem_pages_pt_regions[sel_region]->start_addr = map_addr;
    task_vma->mem_pages_pt_regions[sel_region]->size = map_size;
    task_vma->pt_regions_sel++;
    if (pt_task == thread_get_current_task())
    {
        mpu_switch_to_task(pt_task);
    }
end:
    task_vma_unlock(task_vma, status);
    return ret;
#else
    return -1;
#endif
}
typedef struct vma_clean_params
{
    mln_rbtree_t *tree;
    ram_limit_t *lim;
} vma_clean_params_t;
static int rbtree_iterate_handler_delete(mln_rbtree_node_t *node, void *udata)
{
    vma_clean_params_t *params = udata;
    vma_t *vma_data = mln_rbtree_node_data_get(node);

    if (!(vma_addr_get_flags(vma_data->vaddr) & VMA_ADDR_RESV))
    {
        mm_limit_free_align(params->lim, (void *)vma_node_get_paddr(vma_data), vma_data->size);
    }

    mln_rbtree_delete(params->tree, node);
    vma_node_free(params->tree, node);
    return 0;
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
    int ret;

    pt_task = container_of(
        container_of(task_vma, mm_space_t, mem_vma),
        task_t,
        mm_space);
    vma_clean_params_t params = {
        .lim = pt_task->lim,
        .tree = &task_vma->alloc_tree,
    }; // 删除所有的节点
    ret = mln_rbtree_iterate(&task_vma->alloc_tree, rbtree_iterate_handler_delete, &params);
    return 0;
}

#if IS_ENABLED(CONFIG_MPU)
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
#include "mm_wrap.h"
void *mpu_ram_alloc(mm_space_t *ms, ram_limit_t *r_limit, size_t ram_size, int mem_block)
{
    void *ram = mm_limit_alloc_raw(mem_block, r_limit, ram_size);

    return ram;
}
#endif
