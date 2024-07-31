#pragma once

#include <types.h>
#define THREAD_BLOCK_SIZE CONFIG_THREAD_BLOCK_SIZE //!< 线程块大小，栈在块的顶部

static inline void thread_arch_init(thread_t *th, umword_t flags)
{
}
static inline umword_t task_get_currnt_paddr(vaddr_t vaddr)
{
    return vaddr;
}
static inline umword_t task_get_paddr(task_t *tk, vaddr_t vaddr)
{
    return vaddr;
}
