
#include <types.h>
#include <rbtree_mm.h>
#include <arch.h>
#include <slab.h>
#include <init.h>
#include <string.h>
#include <err.h>
#include <assert.h>
#include <util.h>
#include <slab.h>
#include <arch.h>
#include <buddy.h>
#include <vma.h>
#include <task.h>

static slab_t *vma_slab;

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
    mln_rbtree_insert(&vma->idle_tree, node);
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

    if (!(node_data->flags & VMA_USED_NODE))
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
    printk("alloc:[0x%x 0x%x]\n", alloc_addr, alloc_addr + size - 1);
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
static int vma_idl_tree_free_cmp_handler(const void *key, const void *data)
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
/**
 * @brief 合并节点
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

    rnode = node->right;
    lnode = node->left;

    r_tree->cmp = vma_idl_tree_insert_cmp_handler;

    data_node = mln_rbtree_node_data_get(node);
    mln_rbtree_delete(r_tree, node);

    if (mln_rbtree_null(lnode, r_tree) && mln_rbtree_null(rnode, r_tree))
    {
        // 左右都是空的
        return 0;
    }
    else if (mln_rbtree_null(lnode, r_tree))
    {
        // 左边是空的
        r_datanode = mln_rbtree_node_data_get(rnode);

        if (vma_node_get_used(r_datanode) != vma_node_get_used(data_node))
        {
            return 0;
        }
        data_node->size += r_datanode->size;
        mln_rbtree_delete(r_tree, rnode);
        vma_node_free(r_tree, rnode);

        mln_rbtree_insert(r_tree, node);
    }
    else if (mln_rbtree_null(rnode, r_tree))
    {
        // 右边是空的
        r_datanode = mln_rbtree_node_data_get(rnode);

        if (vma_node_get_used(r_datanode) != vma_node_get_used(data_node))
        {
            return 0;
        }
        mln_rbtree_delete(r_tree, rnode);
        r_datanode->size += data_node->size;
        vma_addr_set_addr(&r_datanode->vaddr, vma_addr_get_addr(data_node->vaddr));
        mln_rbtree_insert(r_tree, rnode);
        vma_node_free(r_tree, node);
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
    lock_status = task_vma_lock(task_vma);
    if (lock_status < 0)
    {
        return lock_status;
    }
    task_vma->idle_tree.cmp = vma_idl_tree_free_cmp_handler;
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
    if (!(node_data->flags & VMA_USED_NODE)) //!< 必须被分配的
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
end:
    task_vma_unlock(task_vma, lock_status);
    return ret;
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
    // 1.查找
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
                 vpage_attrs_to_page_attrs(vma_addr_get_prot(node_data->vaddr))); /*TODO:设置权限*/
    if (ret < 0)
    {
        ret = -ENOMEM;
        goto end;
    }
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
