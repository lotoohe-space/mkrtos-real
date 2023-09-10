#include "u_types.h"
#include "syscall_backend.h"
#include "u_prot.h"
#include "u_ipc.h"
#include "u_mm.h"
#include "u_app.h"
#include <pthread_impl.h>
extern void *app_start_addr;
// extern char __heap_start__[];
extern umword_t __heap_size__;
// extern void *_start_;
static umword_t mm_bitemp[32 /*TODO:自动分配，或者静态划分*/];
// extern char heap[];
static void *mm_page_alloc(void)
{
    // pthread_spin_lock(&lock);
    app_info_t *info = app_info_get(app_start_addr);
    void *heap_addr = RAM_BASE() + info->i.heap_offset - info->i.data_offset;
    size_t max_page_nr = (info->i.heap_size) / 512;
    printf("heap is 0x%x, max page nr is %d.\n", heap_addr, max_page_nr);
    for (umword_t i = 0; i < max_page_nr; i++)
    {
        if (mm_bitemp[i] != (umword_t)(-1))
        {
            for (int j = 0; j < WORD_BITS; j++)
            {
                if (MK_GET_BIT(mm_bitemp[i], j) == 0)
                {
                    // 找到空闲的
                    umword_t find_inx = i * WORD_BITS + j;
                    MK_SET_BIT(mm_bitemp[i], j);
                    // pthread_spin_unlock(&lock);
                    return find_inx * PAGE_SIZE + (char *)heap_addr;
                }
            }
        }
    }
    // pthread_spin_unlock(&lock);

    return NULL;
}

static int sys_mmap2(void *start, size_t len, int prot, int flags, int fd, off_t _offset, umword_t *addr)
{
    if (fd >= 0)
    {
        return -ENOSYS;
    }
    len = ALIGN(len, PAGE_SIZE);
    *addr = (umword_t)mm_page_alloc();
    return 0;
}

umword_t be_mmap2(va_list ap)
{
    void *start;
    size_t len;
    long prot;
    long flags;
    long fd;
    long _offset;

    ARG_6_BE(ap, start, void *, len, size_t, prot, long, flags, long, fd, long, _offset, long);

    umword_t addr;

    int ret = sys_mmap2(start, len, prot, flags, fd, _offset, &addr);

    if (ret < 0)
    {
        return (umword_t)ret;
    }
    return addr;
}
