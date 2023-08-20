#include "mm.h"

void mem_init(mem_t *_this)
{
    spinlock_init(&_this->lock);
    _this->heap_start = NULL;
    _this->heap_end = NULL;
    _this->l_heap = NULL;
    // _this->blong_user = is_user;
}
/**
 * @brief 合并空闲的内存
 * @param[in] mem从某一块内存开始内存
 */
static void mem_merge(mem_t *_this, struct mem_heap *mem)
{
    struct mem_heap *prev_mem;
    struct mem_heap *t_mem;

    // knl_check_user_ram_access_exit(mem, sizeof(struct mem_heap), 1, _this->blong_user);
    _this->l_heap = mem;
    prev_mem = mem->prev;
    for (t_mem = mem; t_mem != _this->heap_end; t_mem = t_mem->next)
    {
        // knl_check_user_ram_access_exit(t_mem, sizeof(struct mem_heap), 1, _this->blong_user);
        if (prev_mem && prev_mem->used == 0)
        {
            // 如果当前没有使用，并且上一个的下一个位置等于当前，则上一个和当前合并
            if (t_mem->used == 0 && ((ptr_t)prev_mem + prev_mem->size + MEM_HEAP_STRUCT_SIZE) == (ptr_t)t_mem)
            {
                // 上一个和当前合并
                prev_mem->size += t_mem->size + MEM_HEAP_STRUCT_SIZE;
                prev_mem->next = t_mem->next;
                t_mem->next->prev = prev_mem;

                _this->l_heap = prev_mem;
            }
        }
        else
        {
            prev_mem = t_mem;
        }
    }
}
/**
 * @brief 释放内存
 * @param[in] mem 释放的内存
 */
void mem_free(mem_t *_this, void *mem)
{
    struct mem_heap *m_mem;

    if (!mem)
    {
        return;
    }
    // #if MEM_TRACE
    //		mem_trace(_this);
    // #endif
    umword_t status = spinlock_lock(&_this->lock);
    m_mem = (struct mem_heap *)((ptr_t)mem - MEM_HEAP_STRUCT_SIZE);
    // knl_check_user_ram_access_exit(m_mem, sizeof(struct mem_heap), 1, _this->blong_user);
    assert(m_mem->magic == MAGIC_NUM);
    if (!m_mem->used)
    {
        spinlock_set(&_this->lock, status);
        return;
    }
    m_mem->used = 0;
    mem_merge(_this, m_mem);
    spinlock_set(&_this->lock, status);
    // #if MEM_TRACE
    //		mem_trace(_this);
    // #endif
}
/**
 * @brief 划分内存
 */
void *mem_split(mem_t *_this, void *mem, uint32_t size)
{
    struct mem_heap *t_mem;
    struct mem_heap *r_mem;
    if (!mem)
    {
        return NULL;
    }
    umword_t status = spinlock_lock(&_this->lock);
    t_mem = (struct mem_heap *)((ptr_t)mem - MEM_HEAP_STRUCT_SIZE);
    // knl_check_user_ram_access_exit(t_mem, sizeof(struct mem_heap), 1, _this->blong_user);
    if (t_mem->used == 0 || t_mem->size < MEM_HEAP_STRUCT_SIZE || t_mem->size < size || size < MEM_HEAP_STRUCT_SIZE)
    {
        spinlock_set(&_this->lock, status);
        return NULL;
    }

    r_mem = (struct mem_heap *)((ptr_t)t_mem + size);
    // knl_check_user_ram_access_exit(r_mem, sizeof(struct mem_heap), 1, _this->blong_user);
    r_mem->used = 1;
    r_mem->size = t_mem->size - size;
    r_mem->next = t_mem->next;
    r_mem->prev = t_mem;
    r_mem->magic = MAGIC_NUM;
    t_mem->next = r_mem;
    t_mem->used = 1;
    t_mem->size = size - MEM_HEAP_STRUCT_SIZE;
    spinlock_set(&_this->lock, status);
    return (void *)((ptr_t)r_mem + MEM_HEAP_STRUCT_SIZE);
}
/**
 * @brief申请一块对齐内存
 */
void *mem_alloc_align(mem_t *_this, uint32_t size, uint32_t align)
{
    void *mem;
    int alloc_size;
    void *align_ptr;

    alloc_size = ALIGN(ALIGN(size + 4, 4), align);
again_alloc:
    mem = mem_alloc(_this, alloc_size);
    if (!mem)
    {
        return NULL;
    }

    align_ptr = (void *)ALIGN((ptr_t)mem, align);
    if (align_ptr == mem)
    {
        if (alloc_size - size >= align && align > MEM_HEAP_STRUCT_SIZE)
        {
            void *split_addr = mem_split(_this, mem, alloc_size - (size - MEM_HEAP_STRUCT_SIZE));
            if (!split_addr)
            {
                mem_free(_this, split_addr);
            }
        }
        assert(((struct mem_heap *)(mem - MEM_HEAP_STRUCT_SIZE))->size >= size);
        return mem;
    }
    else
    {
        void *split_addr = mem_split(_this, mem, (ptr_t)align_ptr - (ptr_t)mem);
        if (split_addr == NULL)
        {
            *(((uint32_t *)(align_ptr)) - 1) = (uint32_t)mem;
            return align_ptr;
        }
        mem_free(_this, mem);
        if (((struct mem_heap *)(split_addr - MEM_HEAP_STRUCT_SIZE))->size < size)
        {
            alloc_size += size - ((struct mem_heap *)(split_addr - MEM_HEAP_STRUCT_SIZE))->size;
            mem_free(_this, split_addr);
            goto again_alloc;
        }
        // assert(((struct mem_heap *)(split_addr - MEM_HEAP_STRUCT_SIZE))->size >= size);
        return split_addr;
    }

    return NULL;
}
/**
 * @brief 释放申请的对齐内存
 */
void mem_free_align(mem_t *_this, void *f_mem)
{
    struct mem_heap *mem;
    void *real_mem;

    umword_t status = spinlock_lock(&_this->lock);
    for (mem = _this->heap_start; mem != _this->heap_end; mem = mem->next)
    {
        // knl_check_user_ram_access_exit(mem, sizeof(struct mem_heap), 1, _this->blong_user);
        assert(mem->magic == MAGIC_NUM);
        if ((ptr_t)mem == (ptr_t)f_mem - MEM_HEAP_STRUCT_SIZE)
        {
            break;
        }
    }
    spinlock_set(&_this->lock, status);

    if (mem == _this->heap_end)
    {
        real_mem = (void *)(*(((uint32_t *)(f_mem)) - 1));
        mem_free(_this, real_mem);
        // #if MEM_TRACE
        //		mem_trace(_this);
        // #endif
        return;
    }
    mem_free(_this, f_mem);
    // #if MEM_TRACE
    //	mem_trace(_this);
    // #endif
}
/**
 * @brief 申请内存
 * @param[in] size 申请的大小
 */
void *mem_alloc(mem_t *_this, uint32_t size)
{

    struct mem_heap *mem;
    size = ALIGN(size, 4);
    umword_t status = spinlock_lock(&_this->lock);
    for (mem = _this->l_heap; mem != _this->heap_end; mem = mem->next)
    {
        // knl_check_user_ram_access_exit(mem, sizeof(struct mem_heap), 1, _this->blong_user);
        assert(mem->magic == MAGIC_NUM);
        if (mem->used == 0 && mem->size > size)
        {
            if (mem->size - size > MEM_HEAP_STRUCT_SIZE)
            {
                struct mem_heap *mem_temp = NULL;
                mem_temp = (struct mem_heap *)((ptr_t)mem + MEM_HEAP_STRUCT_SIZE + size);
                // knl_check_user_ram_access_exit(mem_temp, sizeof(struct mem_heap), 1, _this->blong_user);
                mem_temp->next = mem->next;
                mem_temp->prev = mem;
                mem_temp->used = 0;
                mem_temp->magic = MAGIC_NUM;
                mem_temp->size = mem->size - size - MEM_HEAP_STRUCT_SIZE;
                _this->l_heap = mem_temp;

                mem->size = size;
                mem->used = 1;
                mem->next = mem_temp;
                spinlock_set(&_this->lock, status);

                return (void *)((ptr_t)mem + MEM_HEAP_STRUCT_SIZE);
            }
            else
            {
                mem->used = 1;
                spinlock_set(&_this->lock, status);

                return (void *)((ptr_t)mem + MEM_HEAP_STRUCT_SIZE);
            }
        }
    }
    spinlock_set(&_this->lock, status);

    return NULL;
}

int mem_heap_add(mem_t *_this, void *mem, uint32_t size)
{
    if (size < (MEM_HEAP_STRUCT_SIZE << 1))
    {
        return -1;
    }
    mem = (void *)(ALIGN((ptr_t)mem, 4));
    size -= 4;

    ((struct mem_heap *)mem)->name[0] = ' ';
    ((struct mem_heap *)mem)->name[1] = ' ';
    ((struct mem_heap *)mem)->name[2] = ' ';
    ((struct mem_heap *)mem)->name[3] = ' ';
    ((struct mem_heap *)mem)->used = 0;
    umword_t status = spinlock_lock(&_this->lock);
    if (!_this->heap_start)
    {
        _this->heap_start = mem;
        _this->heap_end = (struct mem_heap *)((ptr_t)mem + size - MEM_HEAP_STRUCT_SIZE);
        _this->heap_start->next = _this->heap_end;
        _this->heap_start->prev = NULL;
        _this->heap_start->used = 0;
        _this->heap_start->magic = MAGIC_NUM;
        _this->heap_start->size = size - (MEM_HEAP_STRUCT_SIZE << 1);
        _this->l_heap = _this->heap_start;

        _this->heap_end->prev = _this->heap_start;
        _this->heap_end->next = NULL;
        _this->heap_end->used = 1;
        _this->heap_end->magic = MAGIC_NUM;
    }
    else
    {
        ((struct mem_heap *)mem)->next = _this->heap_start;
        _this->heap_start->prev = ((struct mem_heap *)mem);

        ((struct mem_heap *)mem)->prev = NULL;
        ((struct mem_heap *)mem)->size = size - MEM_HEAP_STRUCT_SIZE;
        ((struct mem_heap *)mem)->magic = MAGIC_NUM;

        _this->heap_start = mem;
        _this->l_heap = _this->heap_start;
    }
    spinlock_set(&_this->lock, status);

    return 0;
}
size_t mem_get_free_size(mem_t *_this)
{
    size_t size = 0;
    struct mem_heap *mem;

    umword_t status = spinlock_lock(&_this->lock);
    for (mem = _this->heap_start; mem != _this->heap_end; mem = mem->next)
    {
        // knl_check_user_ram_access_exit(mem, sizeof(struct mem_heap), 1, _this->blong_user);
        assert(mem->magic == MAGIC_NUM);
        if (!mem->used)
        {
            size += mem->size;
        }
    }
    spinlock_set(&_this->lock, status);

    return size;
}
/**
 * 获取内存中空余的块
 */
struct mem_heap *mem_get_free(mem_t *_this, struct mem_heap *next,
                              int hope_size, uint32_t *ret_addr)
{
    struct mem_heap *mem;

    if (!next)
    {
        mem = _this->heap_start;
    }
    else
    {
        mem = next;
    }

    umword_t status = spinlock_lock(&_this->lock);
    // knl_check_user_ram_access_exit(mem, sizeof(struct mem_heap), 1, _this->blong_user);
    for (; mem != _this->heap_end; mem = mem->next)
    {
        // knl_check_user_ram_access_exit(mem, sizeof(struct mem_heap), 1, _this->blong_user);
        assert(mem->magic == MAGIC_NUM);
        if (hope_size > 0 && !mem->used && mem->size >= (umword_t)hope_size)
        {
            *ret_addr = (ptr_t)mem + MEM_HEAP_STRUCT_SIZE;
            spinlock_set(&_this->lock, status);

            return mem;
        }
        else if (hope_size < 0 && !mem->used)
        {
            *ret_addr = (ptr_t)mem + MEM_HEAP_STRUCT_SIZE;
            spinlock_set(&_this->lock, status);

            return mem;
        }
    }
    spinlock_set(&_this->lock, status);

    return NULL;
}

#if MEM_TRACE
void mem_trace(mem_t *_this)
{
    struct mem_heap *mem;

    kprint("start heap:0x%x.\n", _this->heap_start);
    kprint("l heap:0x%x.\n", _this->l_heap);
    kprint("end heap:0x%x.\n", _this->heap_end);

    for (mem = _this->heap_start; mem != _this->heap_end; mem = mem->next)
    {
        knl_check_user_ram_access_exit(mem, sizeof(struct mem_heap), 1, _this->blong_user);
        kprint("%d [0x%x-] %dB\n", mem->used, mem, mem->size);
    }
}
#endif
