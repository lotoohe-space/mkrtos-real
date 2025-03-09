#pragma once
#include <string.h>
// struct allocmem
// {
//     void *ptr;
//     size_t size;
//     struct allocmem *next;
// };

void *u_malloc(size_t n);
static inline void *u_calloc(size_t size, int nmemb)
{
    void *mem = u_malloc(size * nmemb);

    if (mem)
    {
        memset(mem, 0, size * nmemb);
    }
    return mem;
}
void *u_realloc(void *old, size_t size);
void u_free(void *p);
// void spurge();
