/**
 * u_malloc.c: a simple memory allocator for use in MKRTOS.
 * 使用缺页模拟申请内存，申请的内存不能用于栈内存。
 */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <u_malloc.h>

static struct allocmem *memlocs = NULL;

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
    memlocs = insert_end(memlocs, &info);
    if (info == NULL)
    {
        /// Failed to initialize metadata info
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
        return ptr;
    }
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

void u_free(void *ptr)
{
    if (ptr == NULL)
        return;
    struct allocmem *info = find(memlocs, ptr);
    if (info == NULL)
        return;
    munmap(info->ptr, info->size);
    memlocs = find_and_remove(memlocs, ptr);
}

/// Violently Remove Everything
// void spurge()
// {
//     while (memlocs)
//     {
//         sfree(memlocs->ptr);
//     }
// }