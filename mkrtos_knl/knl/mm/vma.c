
#include <arch.h>
#include <assert.h>
#include <buddy.h>
#include <err.h>
#include <init.h>
#include <rbtree_mm.h>
#include <slab.h>
#include <string.h>
#include <task.h>
#include <types.h>
#include <util.h>
#include <vma.h>

#define VMA_DEBUG 0

static slab_t *vma_slab;
static int vma_idl_tree_insert_cmp_handler(const void *key, const void *data);
/**
 * @brief 在系统初始化时调用，初始化vma的内存
 *
 */
static void vma_slab_init(void)
{
    vma_slab = slab_create(sizeof(vma_t), "vma");
    assert(vma_slab);
}
INIT_KOBJ_MEM(vma_slab_init);

static vma_t *vma_alloc(void)
{
    vma_t *pvma = slab_alloc(vma_slab);

    if (!pvma)
    {
        return NULL;
    }
    memset(pvma, 0, sizeof(*pvma));
    return pvma;
}
static void vma_free(vma_t *data)
{
    slab_free(vma_slab, data);
}
static mln_rbtree_node_t *vma_node_create(mln_rbtree_t *r_tree, vma_addr_t addr, size_t size, paddr_t paddr)
{
    mln_rbtree_node_t *node = NULL;
    vma_t *data_vma = vma_alloc();

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
    vma_free(mln_rbtree_node_data_get(node));
    mln_rbtree_node_free(r_tree, node);
}

int task_vma_init(task_vma_t *vma)
{
    rbtree_mm_init(&vma->idle_tree);
    rbtree_mm_init(&vma->alloc_tree);

    mln_rbtree_node_t *node;

    node = vma_node_create(&vma->idle_tree,
                           vma_addr_create(0, 0, 0x1000),
                           0x100000000 - 0x1000,
                           0);
    vma->idle_tree.cmp = vma_idl_tree_insert_cmp_handler;
    mln_rbtree_insert(&vma->idle_tree, node);
}

void task_vma_rbtree_print(mln_rbtree_t *t, mln_rbtree_node_t *root)
{
    if (root == &(t->nil))
    {
        return;
    }
    vma_t *data = mln_rbtree_node_data_get(root);

    task_vma_rbtree_print(t, root->left);
    printk("[0x%lx 0x%lx U:%d 0x%x]\n", vma_addr_get_addr(data->vaddr), vma_addr_get_addr(data->vaddr) + data->size,
           vma_node_get_used(data), vma_addr_get_prot(data->vaddr));
    task_vma_rbtree_print(t, root->right);
}

static mln_rbtree_node_t *task_vma_tree_find(mln_rbtree_t *tree, rbtree_cmp cmp, void *data)
{
    mln_rbtree_node_t *rn;

    tree->cmp = cmp;
    rn = mln_rbtree_search(tree, &data);
    if (mln_rbtree_null(rn, tree))
    {
        return NULL;
    }

    return rn;
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
typedef struct vma_idl_tree_iter_params
{
    // vaddr_t addr;
    size_t size;

    mln_rbtree_node_t *ret_node;
} vma_idl_tree_iter_params_t;
/**
 * @brief 空闲块迭代查找需要的大小
 *
 * @param node
 * @param udata
 * @return int
 */
static int rbtree_iterate_idle_tree_find(mln_rbtree_node_t *node, void *udata)
{
    vma_idl_tree_iter_params_t *param = udata;
    vma_t *node_data = mln_rbtree_node_data_get(node);

    if (!(vma_node_get_used(node_data)))
    {
        if (node_data->size >= param->size)
        {
            param->ret_node = node;
            return -1;
        }
    }
    return 0;
}
/**
 * @brief 在空闲块中找到指定大小的节点
 *
 * @param task_vma
 * @param addr
 * @param size
 * @return mln_rbtree_node_t*
 */
static mln_rbtree_node_t *task_vma_idle_tree_find(task_vma_t *task_vma, size_t size)
{
    vma_idl_tree_iter_params_t param = {
        .size = size,
    };
    int ret;

    ret = mln_rbtree_iterate(&task_vma->idle_tree, rbtree_iterate_idle_tree_find, &param);
    if (ret >= 0)
    {
        return NULL;
    }
    return param.ret_node;
}
/**
 * @brief 分割节点
 * 1.删除原来的节点
 * 2.分割节点 分割时考虑三个情况
 * @param r_tree
 * @param node
 * @return int
 */
static int task_vma_node_split(mln_rbtree_t *r_tree, mln_rbtree_node_t *node, vaddr_t addr, size_t size)
{
    vma_t *data;
    int ret = -EINVAL;

    assert(r_tree);
    assert(node);
    mln_rbtree_delete(r_tree, node);
    data = mln_rbtree_node_data_get(node);

    assert((addr >= vma_addr_get_addr(data->vaddr)) &&
           addr + size <= vma_addr_get_addr(data->vaddr) + data->size);
    //!< 检查地址在合理的范围之内

    if (addr == vma_addr_get_addr(data->vaddr) && size == data->size)
    {
        /*大小正好相等，则什么都不用做*/
    }
    else if (addr == vma_addr_get_addr(data->vaddr))
    {
        //!< 左对齐
        mln_rbtree_node_t *r_node;

        r_node = vma_node_create(r_tree,
                                 vma_addr_create(data->vaddr.prot, data->vaddr.flags,
                                                 vma_addr_get_addr(data->vaddr) + size),
                                 data->size - size, 0);
        if (r_node == NULL)
        {
            ret = -ENOMEM;
            goto err_ret;
        }
        r_tree->cmp = vma_idl_tree_insert_cmp_handler;
        mln_rbtree_insert(r_tree, r_node);
    }
    else if (addr + size == vma_addr_get_addr(data->vaddr) + data->size)
    {
        //!< 右对齐
        mln_rbtree_node_t *r_node;

        r_node = vma_node_create(r_tree,
                                 vma_addr_create(data->vaddr.prot, data->vaddr.flags,
                                                 vma_addr_get_addr(data->vaddr)),
                                 data->size - size, 0);
        if (r_node == NULL)
        {
            ret = -ENOMEM;
            goto err_ret;
        }
        r_tree->cmp = vma_idl_tree_insert_cmp_handler;
        mln_rbtree_insert(r_tree, r_node);
    }
    else
    {
        // 在中间分割成两个
        mln_rbtree_node_t *r_node;

        r_node = vma_node_create(r_tree,
                                 vma_addr_create(data->vaddr.prot, data->vaddr.flags,
                                                 vma_addr_get_addr(data->vaddr)),
                                 addr - vma_addr_get_addr(data->vaddr), 0);
        if (r_node == NULL)
        {
            ret = -ENOMEM;
            goto err_ret;
        }

        mln_rbtree_node_t *l_node;

        l_node = vma_node_create(r_tree,
                                 vma_addr_create(data->vaddr.prot, data->vaddr.flags,
                                                 addr + size),
                                 vma_addr_get_addr(data->vaddr) + data->size - (addr + size), 0);
        if (l_node == NULL)
        {
            vma_node_free(r_tree, r_node);
            ret = -ENOMEM;
            goto err_ret;
        }
        r_tree->cmp = vma_idl_tree_insert_cmp_handler;
        mln_rbtree_insert(r_tree, l_node);
        mln_rbtree_insert(r_tree, r_node);
    }
    vma_addr_set_addr(&data->vaddr, addr); //!< 重设分割后的地址&大小
    data->size = size;

    ret = 0;
    goto ret;
err_ret:
    // 刚刚删除了这里要插入回去
    r_tree->cmp = vma_idl_tree_insert_cmp_handler;
    mln_rbtree_insert(r_tree, node);
ret:
    return ret;
}
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
mword_t task_vma_lock_two(task_vma_t *task_vma_0, task_vma_t *task_vma_1, mword_t *status0, mword_t *status1)
{
    mword_t status;
    assert(status0);
    assert(status1);

    status = task_vma_lock(task_vma_0);
    if (status < 0)
    {
        return status;
    }
    *status0 = status;
    status = task_vma_lock(task_vma_1);
    *status1 = status;
    return status;
}
void task_vma_unlock_two(task_vma_t *task_vma_0, task_vma_t *task_vma_1, mword_t status0, mword_t status1)
{
    task_vma_unlock(task_vma_1, status1);
    task_vma_unlock(task_vma_0, status0);
}
/**
 * 分配步骤
 * 1.找到一个空闲的合适的虚拟内存区域
 * 2.用申请的区域去分割找到的区域
 * 3.将申请的区域插入回去，并设置属性
 */
int task_vma_alloc(task_vma_t *task_vma, vma_addr_t vaddr, size_t size,
                   paddr_t paddr, vaddr_t *ret_vaddr)
{
    int ret;
    mln_rbtree_node_t *node;
    vma_t *node_data;
    mword_t lock_status;
    vaddr_t alloc_addr;

    if ((size & (PAGE_SIZE - 1)) != 0)
    {
        return -EINVAL;
    }
    lock_status = task_vma_lock(task_vma);
    if (lock_status < 0)
    {
        return lock_status;
    }
#if VMA_DEBUG
    printk("alloc pre:\n");
    task_vma_rbtree_print(&task_vma->idle_tree, mln_rbtree_root(&task_vma->idle_tree));
#endif
    if (vma_addr_get_addr(vaddr) == 0)
    {
        node = task_vma_idle_tree_find(task_vma, size);
        if (!node)
        {
            ret = -ENOMEM;
            goto end;
        }
        node_data = mln_rbtree_node_data_get(node);
        alloc_addr = vma_addr_get_addr(node_data->vaddr);
    }
    else
    {
        alloc_addr = vma_addr_get_addr(vaddr);

        task_vma->idle_tree.cmp = vma_idl_tree_alloc_cmp_handler;
        node = mln_rbtree_search(
            &task_vma->idle_tree,
            &(vma_idl_tree_insert_params_t){
                .size = size,
                .addr = alloc_addr,
            }); //!< 查找是否存在
        if (mln_rbtree_null(node, &task_vma->idle_tree))
        {
            ret = -ENOENT;
            goto end;
        }
        if (vma_node_get_used(mln_rbtree_node_data_get(node)))
        {
            ret = -EEXIST;
            goto end;
        }
        node_data = mln_rbtree_node_data_get(node);
    }

    ret = task_vma_node_split(&task_vma->idle_tree, node,
                              alloc_addr, size);
    if (ret < 0)
    {
        goto end;
    }
    //!< 设置当前节点是使用节点，设置属性等，并插入到树中
    vma_node_set_used(node_data);
    vma_addr_set_flags(&node_data->vaddr, vma_addr_get_flags(vaddr));
    vma_addr_set_prot(&node_data->vaddr, vma_addr_get_prot(vaddr));
    vma_node_set_paddr(node_data, paddr);
    task_vma->idle_tree.cmp = vma_idl_tree_insert_cmp_handler;
    mln_rbtree_insert(&task_vma->idle_tree, node);

    if (ret_vaddr)
    {
        //!< 设置分配后的地址
        *ret_vaddr = alloc_addr;
    }
    printk("alloc:[0x%x 0x%x] size:0x%x\n", alloc_addr, alloc_addr + size - 1, size);
#if VMA_DEBUG
    task_vma_rbtree_print(&task_vma->idle_tree, mln_rbtree_root(&task_vma->idle_tree));
#endif
    ret = 0;
end:
    task_vma_unlock(task_vma, lock_status);
    return ret;
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
typedef struct vma_idl_tree_iter_grant_params
{
    vaddr_t addr;
    vaddr_t dst_addr;
    size_t size;
    mln_rbtree_t *r_src_tree;
    mln_rbtree_t *r_dst_tree;
    task_vma_t *src_mm_vma;
    task_vma_t *dst_mm_vma;

    size_t grant_size;
} vma_idl_tree_iter_grant_params_t;
/**
 * @brief 迭代转移
 *
 * @param node
 * @param udata
 * @return int
 */
static int rbtree_iterate_tree_grant(mln_rbtree_node_t *node, void *udata)
{
    vma_idl_tree_iter_grant_params_t *param = udata;
    vma_t *node_data = mln_rbtree_node_data_get(node);

    // printk("[grant][0x%lx 0x%lx] [0x%lx:0x%lx]\n", vma_addr_get_addr(node_data->vaddr), vma_addr_get_addr(node_data->vaddr) + node_data->size,
    //       param->addr, param->size);
    if (vma_addr_get_addr(node_data->vaddr) >= param->addr &&
        vma_addr_get_addr(node_data->vaddr) + node_data->size <= param->addr + param->size)
    {
        mm_space_t *src_mm_space = container_of(param->src_mm_vma, mm_space_t, mem_vma);
        mm_space_t *dst_mm_space = container_of(param->dst_mm_vma, mm_space_t, mem_vma);

        // 从红黑树中删除
        mln_rbtree_delete(param->r_src_tree, node);
        vaddr_t dst_addr;

        dst_addr = vma_addr_get_addr(node_data->vaddr) - param->addr + param->dst_addr;

        // 解除映射
        unmap_mm(mm_space_get_pdir(src_mm_space),
                 vma_addr_get_addr(node_data->vaddr), PAGE_SHIFT, 1);

        int ret = map_mm(mm_space_get_pdir(dst_mm_space), dst_addr,
                         (addr_t)vma_node_get_paddr(node_data), PAGE_SHIFT, 1,
                         vpage_attrs_to_page_attrs(vma_addr_get_prot(node_data->vaddr)));

        if (ret >= 0)
        {
            param->grant_size += PAGE_SIZE;
        }
        // printk("vaddr:0x%x grant vaddr:0x%x 0x%x\n", vma_addr_get_addr(node_data->vaddr), dst_addr, PAGE_SIZE);
        vma_addr_set_addr(&node_data->vaddr, dst_addr);
        param->r_dst_tree->cmp = vma_idl_tree_insert_cmp_handler;
        mln_rbtree_insert(param->r_dst_tree, node);
    }
    return 0;
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
int task_vma_grant(task_vma_t *src_task_vma, task_vma_t *dst_task_vma, vaddr_t src_addr, vaddr_t dst_addr, size_t size)
{
    assert(src_task_vma);
    assert(dst_task_vma);
    if ((size & (PAGE_SIZE - 1)) != 0)
    {
        return -EINVAL;
    }
    mword_t lock_status;
    mword_t lock_status0;
    mword_t lock_status1;
    int ret = -EINVAL;

    lock_status = task_vma_lock_two(src_task_vma, dst_task_vma, &lock_status0, &lock_status1);
    if (lock_status < 0)
    {
        return lock_status;
    }
    mln_rbtree_node_t *src_node;
    mln_rbtree_node_t *dst_node;

    // 查找源
    src_task_vma->idle_tree.cmp = vma_idl_tree_eq_cmp_handler;
    src_node = mln_rbtree_search(
        &src_task_vma->idle_tree,
        &(vma_idl_tree_insert_params_t){
            .size = size,
            .addr = src_addr,
        }); //!< 查找是否存在
    if (mln_rbtree_null(src_node, &src_task_vma->idle_tree))
    {
        ret = -ENOENT;
        goto end;
    }
    if (!vma_node_get_used(mln_rbtree_node_data_get(src_node)))
    {
        ret = -ENOENT;
        goto end;
    }
    // 查找目的
    dst_task_vma->idle_tree.cmp = vma_idl_tree_wrap_cmp_handler; /**TODO:应该检查是否被包裹 */
    dst_node = mln_rbtree_search(
        &dst_task_vma->idle_tree,
        &(vma_idl_tree_insert_params_t){
            .size = size,
            .addr = dst_addr,
        }); //!< 查找是否存在
    if (mln_rbtree_null(dst_node, &dst_task_vma->idle_tree))
    {
        ret = -EEXIST;
        goto end;
    }
    else
    {
        vma_t *dst_node_dat = mln_rbtree_node_data_get(dst_node);

        if (vma_node_get_used(dst_node_dat))
        {
            ret = -EEXIST;
            goto end;
        }
        ret = task_vma_node_split(&dst_task_vma->idle_tree, dst_node, dst_addr, size);
        if (ret < 0)
        {
            goto end;
        }
        vma_node_free(&dst_task_vma->idle_tree, dst_node);
    }

    /*转移映射*/
    mln_rbtree_delete(&src_task_vma->idle_tree, src_node);

    vma_idl_tree_iter_grant_params_t params = {
        .addr = src_addr,
        .dst_addr = dst_addr,
        .size = size,
        .r_src_tree = &src_task_vma->alloc_tree,
        .r_dst_tree = &dst_task_vma->alloc_tree,
        .src_mm_vma = src_task_vma,
        .dst_mm_vma = dst_task_vma,
    };
    // 从源task的分配树中转移到目的task的分配树中。
    mln_rbtree_iterate(&src_task_vma->alloc_tree, rbtree_iterate_tree_grant, &params);

    dst_task_vma->idle_tree.cmp = vma_idl_tree_insert_cmp_handler;
    vma_addr_set_addr(&((vma_t *)mln_rbtree_node_data_get(src_node))->vaddr,
                      dst_addr);
    mln_rbtree_insert(&dst_task_vma->idle_tree, src_node);
    ret = params.grant_size;
#if VMA_DEBUG
    printk("grant:\n");
    task_vma_rbtree_print(&dst_task_vma->idle_tree, mln_rbtree_root(&dst_task_vma->idle_tree));
#endif
end:
    task_vma_unlock_two(src_task_vma, dst_task_vma, lock_status0, lock_status1);
    return ret;
}
static int rbtree_cmp_merge_r(const void *key, const void *data)
{
    vma_idl_tree_insert_params_t *key_p = (vma_idl_tree_insert_params_t *)key;
    vma_t *data_p = (vma_t *)data;

    if (key_p->addr == vma_addr_get_addr(data_p->vaddr) + data_p->size)
    {
        return 0;
    }
    else if (key_p->addr + key_p->size < vma_addr_get_addr(data_p->vaddr))
    {
        return -1;
    }
    else
    {
        return 1;
    }
}
static int rbtree_cmp_merge_l(const void *key, const void *data)
{
    vma_idl_tree_insert_params_t *key_p = (vma_idl_tree_insert_params_t *)key;
    vma_t *data_p = (vma_t *)data;

    if (key_p->addr == vma_addr_get_addr(data_p->vaddr))
    {
        return 0;
    }
    else if (key_p->addr < vma_addr_get_addr(data_p->vaddr))
    {
        return -1;
    }
    else
    {
        return 1;
    }
}
/**
 * @brief 合并节点TODO:合并时属性不一致也不能合并
 *
 * @param r_tree
 * @param node
 * @return int
 */
static int vma_node_merge(mln_rbtree_t *r_tree, mln_rbtree_node_t *node)
{
    assert(r_tree);
    assert(node);

    mln_rbtree_node_t *rnode;
    mln_rbtree_node_t *lnode;
    vma_t *r_datanode;
    vma_t *l_datanode;
    vma_t *data_node;

    // rnode = node->right;
    // lnode = node->left;
    data_node = mln_rbtree_node_data_get(node);

    r_tree->cmp = rbtree_cmp_merge_r;
    lnode = mln_rbtree_search(
        r_tree,
        &(vma_idl_tree_insert_params_t){
            .size = data_node->size,
            .addr = vma_addr_get_addr(data_node->vaddr),
        }); //!< 查找是否存在

    r_tree->cmp = rbtree_cmp_merge_l;
    rnode = mln_rbtree_search(
        r_tree,
        &(vma_idl_tree_insert_params_t){
            .size = 0,
            .addr = vma_addr_get_addr(data_node->vaddr) + data_node->size,
        }); //!< 查找是否存在

    r_tree->cmp = vma_idl_tree_insert_cmp_handler;

    vma_node_set_unused(data_node);

    int l_dmerge = !!mln_rbtree_null(lnode, r_tree) || ((!mln_rbtree_null(lnode, r_tree)) && (vma_node_get_used(((vma_t *)mln_rbtree_node_data_get(lnode))) !=
                                                                                              vma_node_get_used(data_node)) ||
                                                        (vma_node_get_used(((vma_t *)mln_rbtree_node_data_get(lnode))) && ((vma_addr_get_prot(((vma_t *)mln_rbtree_node_data_get(lnode))->vaddr) !=
                                                                                                                            vma_addr_get_prot(data_node->vaddr)) ||
                                                                                                                           (vma_addr_get_flags(((vma_t *)mln_rbtree_node_data_get(lnode))->vaddr) !=
                                                                                                                            vma_addr_get_flags(data_node->vaddr)))));
    int r_dmerge = !!mln_rbtree_null(rnode, r_tree) || ((!mln_rbtree_null(rnode, r_tree)) && (vma_node_get_used(mln_rbtree_node_data_get(rnode)) !=
                                                                                              vma_node_get_used(data_node)) ||
                                                        (vma_node_get_used(mln_rbtree_node_data_get(rnode)) && ((vma_addr_get_prot(((vma_t *)mln_rbtree_node_data_get(rnode))->vaddr) !=
                                                                                                                 vma_addr_get_prot(data_node->vaddr)) ||
                                                                                                                (vma_addr_get_flags(((vma_t *)mln_rbtree_node_data_get(rnode))->vaddr) !=
                                                                                                                 vma_addr_get_flags(data_node->vaddr)))));
    if (l_dmerge && r_dmerge)
    {
        // 左右都是空的
        vma_node_set_unused(data_node);
        return 0;
    }
    else if (l_dmerge && !r_dmerge)
    { // 左边是空的
        mln_rbtree_delete(r_tree, node);

        r_datanode = mln_rbtree_node_data_get(rnode);

        data_node->size += r_datanode->size;
        mln_rbtree_delete(r_tree, rnode);
        vma_node_free(r_tree, rnode);

        mln_rbtree_insert(r_tree, node);
    }
    else if (!l_dmerge && r_dmerge)
    { // 右边是空的
        mln_rbtree_delete(r_tree, node);

        l_datanode = mln_rbtree_node_data_get(lnode);

        mln_rbtree_delete(r_tree, lnode);
        l_datanode->size += data_node->size;
        mln_rbtree_insert(r_tree, lnode);
        vma_node_free(r_tree, node);
    }
    else
    {
        mln_rbtree_delete(r_tree, node);
        r_datanode = mln_rbtree_node_data_get(rnode);
        l_datanode = mln_rbtree_node_data_get(lnode);

        mln_rbtree_delete(r_tree, lnode);
        mln_rbtree_delete(r_tree, rnode);

        data_node->size += l_datanode->size;
        data_node->size += r_datanode->size;

        mln_rbtree_insert(r_tree, node);
        vma_node_free(r_tree, lnode);
        vma_node_free(r_tree, rnode);
    }
    return 0;
}
typedef struct vma_idl_tree_iter_del_params
{
    vaddr_t addr;
    size_t size;
    mln_rbtree_t *r_tree;
    mm_space_t *mm_space;
} vma_idl_tree_iter_del_params_t;
/**
 * @brief 迭代删除
 *
 * @param node
 * @param udata
 * @return int
 */
static int rbtree_iterate_alloc_tree_del(mln_rbtree_node_t *node, void *udata)
{
    vma_idl_tree_iter_del_params_t *param = udata;
    vma_t *node_data = mln_rbtree_node_data_get(node);

    if (vma_addr_get_addr(node_data->vaddr) >= param->addr &&
        vma_addr_get_addr(node_data->vaddr) + node_data->size <= param->addr + param->size)
    {
        // 解除映射
        unmap_mm(mm_space_get_pdir(param->mm_space),
                 vma_addr_get_addr(node_data->vaddr), PAGE_SHIFT, 1);

        // 从红黑树中删除
        mln_rbtree_delete(param->r_tree, node);
        vma_node_free(param->r_tree, node);
    }
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
int task_vma_free(task_vma_t *task_vma, vaddr_t addr, size_t size)
{
    int ret = -EINVAL;
    mln_rbtree_node_t *find_node;
    vma_t *node_data;
    mword_t lock_status;

    assert(task_vma);
    if ((size & (PAGE_SIZE - 1)) != 0)
    {
        return -EINVAL;
    }
    lock_status = task_vma_lock(task_vma);
    if (lock_status < 0)
    {
        return lock_status;
    }
#if VMA_DEBUG
    printk("free pre:\n");
    task_vma_rbtree_print(&task_vma->idle_tree, mln_rbtree_root(&task_vma->idle_tree));
#endif
    task_vma->idle_tree.cmp = vma_idl_tree_eq_cmp_handler;
    find_node = mln_rbtree_search(
        &task_vma->idle_tree,
        &(vma_idl_tree_insert_params_t){
            .size = size,
            .addr = addr,
        }); //!< 查找是否存在
    if (mln_rbtree_null(find_node, &task_vma->idle_tree))
    {
        ret = -ENOENT;
        goto end;
    }
    node_data = mln_rbtree_node_data_get(find_node);
    if (!vma_node_get_used(node_data)) //!< 必须被分配的
    {
        ret = -ENOENT;
        goto end;
    }
    /*释放已经分配的物理内存，并解除mmu的映射*/
    vma_idl_tree_iter_del_params_t param = {
        .r_tree = &task_vma->alloc_tree,
        .mm_space = container_of(task_vma, mm_space_t, mem_vma),
        .addr = addr,
        .size = size,
    };
    mln_rbtree_iterate(&task_vma->alloc_tree, rbtree_iterate_alloc_tree_del, &param);
    vma_node_merge(&task_vma->idle_tree, find_node);
    ret = 0;
#if VMA_DEBUG
    printk("free:\n");
    task_vma_rbtree_print(&task_vma->idle_tree, mln_rbtree_root(&task_vma->idle_tree));
#endif
end:
    task_vma_unlock(task_vma, lock_status);
    return ret;
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
int task_vma_page_fault(task_vma_t *task_vma, vaddr_t addr)
{
    mln_rbtree_node_t *find_node = NULL;
    vma_t *node_data = NULL;
    void *mem = NULL;
    task_t *task;
    mword_t lock_status;
    int ret;

    assert(task_vma);
    lock_status = task_vma_lock(task_vma);
    if (lock_status < 0)
    {
        return lock_status;
    }
    // printk("page fault:0x%x.\n", addr);
    //  1.查找
    task = container_of(container_of(task_vma, mm_space_t, mem_vma), task_t, mm_space);
    task_vma->idle_tree.cmp = vma_idl_tree_wrap_cmp_handler;
    find_node = mln_rbtree_search(
        &task_vma->idle_tree,
        &(vma_idl_tree_insert_params_t){
            .size = PAGE_SIZE,
            .addr = addr,
        }); //!< 查找是否存在
    if (mln_rbtree_null(find_node, &task_vma->idle_tree))
    {
        ret = -ENOENT;
        goto end;
    }
    node_data = mln_rbtree_node_data_get(find_node);
    if (!(node_data->flags & VMA_USED_NODE)) //!< 必须被分配的
    {
        ret = -ENOENT;
        goto end;
    }
    // 2.申请物理内存
    if (vma_addr_get_flags(node_data->vaddr) & VMA_ADDR_RESV)
    {
        if (!vma_node_get_paddr(node_data))
        {
            mem = NULL;
        }
        else
        {
            mem = (void *)(addr - vma_addr_get_addr(node_data->vaddr) +
                           vma_node_get_paddr(node_data));
        }
    }
    else
    {
        mem = buddy_alloc(buddy_get_alloter(), PAGE_SIZE);
        memset(mem, 0, PAGE_SIZE);
    }
    if (!mem)
    {
        ret = -ENOMEM;
        goto end;
    }
    // 3.插入到分配树中
    mln_rbtree_node_t *alloc_node;

    alloc_node = vma_node_create(&task_vma->alloc_tree,
                                 vma_addr_create(vma_addr_get_prot(node_data->vaddr),
                                                 vma_addr_get_flags(node_data->vaddr), addr),
                                 PAGE_SIZE, (paddr_t)mem);
    if (alloc_node == NULL)
    {
        if (!(vma_addr_get_flags(node_data->vaddr) & VMA_ADDR_RESV))
        {
            buddy_free(buddy_get_alloter(), mem);
        }
        ret = -ENOMEM;
        goto end;
    }
    // 4.进行映射
    ret = map_mm(mm_space_get_pdir(&task->mm_space), addr,
                 (addr_t)mem, PAGE_SHIFT, 1,
                 vpage_attrs_to_page_attrs(vma_addr_get_prot(node_data->vaddr)));
    if (ret < 0)
    {
        vma_node_free(&task_vma->alloc_tree, alloc_node);
        ret = -ENOMEM;
        goto end;
    }
    task_vma->alloc_tree.cmp = vma_idl_tree_insert_cmp_handler;
    mln_rbtree_insert(&task_vma->alloc_tree, alloc_node);
    ret = 0;
end:
    task_vma_unlock(task_vma, lock_status);
    return ret;
}
/**
 * @brief 释放task_vma
 *
 * @param task_vma
 * @return int
 */
int task_vma_clean(task_vma_t *task_vma)
{
    /*TODO: */
    return 0;
}
