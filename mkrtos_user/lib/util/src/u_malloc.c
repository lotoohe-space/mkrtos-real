/**
 * u_malloc.c: a simple memory allocator for use in MKRTOS.
 * 使用缺页模拟申请内存，申请的内存不能用于栈内存。
 */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <string.h>

#include <u_malloc.h>
#include <u_mutex.h>
#include <u_hd_man.h>
#include <assert.h>
#include <errno.h>

static struct allocmem *memlocs = NULL;
static u_mutex_t lock_mutex = U_MUTEX_INIT;

static void alloc_lock(void)
{
    obj_handler_t lock_hd;

    if (lock_mutex.obj == HANDLER_INVALID)
    {
        lock_hd = handler_alloc();
        assert(lock_hd != HANDLER_INVALID);
        u_mutex_init(&lock_mutex, lock_hd);
    }
    u_mutex_lock(&lock_mutex, 0, NULL);
}
static void alloc_unlock(void)
{
    assert(lock_mutex.obj != HANDLER_INVALID);
    u_mutex_unlock(&lock_mutex);
}

static struct allocmem *new_alloc_mem()
{
    struct allocmem *ptr;
    if ((ptr = mmap(NULL, sizeof(struct allocmem), PROT_READ | PROT_WRITE | PROT_PFS, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)) == MAP_FAILED)
    {
        return NULL;
    }
    else
    {
        ptr->ptr = NULL;
        ptr->size = 0;
        ptr->next = NULL;
        return ptr;
    }
}

static void free_alloc_mem(struct allocmem *ptr)
{
    if (ptr != NULL)
    {
        munmap(ptr, sizeof(struct allocmem));
    }
}

/// Pass a valid pointer (or NULL) to store the resultant into
struct allocmem *insert_end(struct allocmem *ptr, struct allocmem **outVar)
{
    if (ptr == NULL)
    {
        struct allocmem *outAble = new_alloc_mem();
        if (outVar != NULL)
            *outVar = outAble;
        return (outAble);
    }
    else
    {
        ptr->next = insert_end(ptr->next, outVar);
        return ptr;
    }
}

struct allocmem *find(struct allocmem *ptr, void *searchPtr)
{
    if (ptr == NULL)
    {
        return NULL;
    }
    if (ptr->ptr == searchPtr)
        return ptr;
    return find(ptr->next, searchPtr);
}

struct allocmem *find_and_remove(struct allocmem *ptr, void *searchPtr)
{
    if (ptr == NULL)
    {
        return NULL;
    }
    if (ptr->ptr == searchPtr)
    {
        struct allocmem *nextUp = ptr->next;
        free_alloc_mem(ptr);
        return nextUp;
    }
    ptr->next = find_and_remove(ptr->next, searchPtr);
    return ptr;
}

void *u_malloc(size_t size)
{
    struct allocmem *info = NULL;

    alloc_lock();
    memlocs = insert_end(memlocs, &info);
    if (info == NULL)
    {
        /// Failed to initialize metadata info
        alloc_unlock();
        return NULL;
    }
    void *ptr;
    if ((ptr = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_PFS, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)) == MAP_FAILED)
    {
        /// Failed to make data, revert changes
        memlocs = find_and_remove(memlocs, NULL);
    }
    else
    {
        /// Got it together
        info->ptr = ptr;
        info->size = size;
        /// Return this precious pointer
        alloc_unlock();
        return ptr;
    }
    alloc_unlock();
    return NULL;
}

void *u_calloc(size_t size, int nmemb)
{
    size_t isOverFlow = size * nmemb;
    if (size != isOverFlow / nmemb)
    {
        /// Overflow, dont try
        return NULL;
    }
    void *newPtr = u_malloc(isOverFlow);
    if (newPtr == NULL)
    {
        /// Failed to init
        return NULL;
    }
    /// A character is a byte of data that can be managed
    for (size_t loc = 0; loc < isOverFlow; loc++)
    {
        *(((char *)newPtr) + loc) = 0;
    }
    return newPtr;
}
void *u_realloc(void *old, size_t size)
{
    struct allocmem *info;
    size_t old_size;
    void *new_mem = u_malloc(size);

    if (!new_mem)
    {
        return NULL;
    }
    alloc_lock();
    info = find(memlocs, old);
    if (info == NULL)
    {
        alloc_unlock();
        u_free(new_mem);
        return NULL;
    }
    old_size = info->size;
    alloc_unlock();
    memcpy(new_mem, old, old_size);
    u_free(old);
    return new_mem;
}
void u_free(void *ptr)
{
    if (ptr == NULL)
        return;
    alloc_lock();
    struct allocmem *info = find(memlocs, ptr);
    if (info == NULL)
    {
        alloc_unlock();
        return;
    }
    munmap(info->ptr, info->size);
    memlocs = find_and_remove(memlocs, ptr);
    alloc_unlock();
}

/// Violently Remove Everything
// void spurge()
// {
//     while (memlocs)
//     {
//         sfree(memlocs->ptr);
//     }
// }