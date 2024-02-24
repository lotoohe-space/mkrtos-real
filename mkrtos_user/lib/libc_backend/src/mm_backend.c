#include "u_types.h"
#include "syscall_backend.h"
#include "u_prot.h"
#include "u_ipc.h"
#include "u_mm.h"
#include "u_app.h"
#include "cons_cli.h"
#include "u_arch.h"
#include <pthread_impl.h>
#include <assert.h>
static pthread_spinlock_t lock;
extern void *app_start_addr;
static umword_t mm_bitemp[ROUND(CONFIG_EX_RAM_SIZE, MK_PAGE_SIZE) / (sizeof(umword_t) * 8) + 1];
static void *mm_page_alloc(int page_nr)
{
    int cnt = 0;
    mword_t find_inx = -1;
    app_info_t *info = app_info_get(app_start_addr);
    assert(info);
    void *heap_addr = (void *)((umword_t)TASK_RAM_BASE() + info->i.heap_offset - info->i.data_offset);
    size_t max_page_nr = (info->i.heap_size) / MK_PAGE_SIZE;

    if (max_page_nr == 0)
    {
        return NULL;
    }
    if (max_page_nr > sizeof(mm_bitemp) * WORD_BITS)
    {
        cons_write_str("mm bitmap is to small.\n");
    }
    // printf("heap is 0x%x, max page nr is %d.\n", heap_addr, max_page_nr);
    pthread_spin_lock(&lock);
    for (umword_t i = 0; i < ROUND_UP(max_page_nr, WORD_BITS); i++)
    {
        if (mm_bitemp[i] != (umword_t)(-1))
        {
            for (int j = 0; j < WORD_BITS; j++)
            {
                if (MK_GET_BIT(mm_bitemp[i], j) == 0)
                {
                    // 找到空闲的
                    if (find_inx == -1)
                    {
                        find_inx = i * WORD_BITS + j;
                    }
                    cnt++;
                    if (find_inx + cnt > max_page_nr)
                    {
                        pthread_spin_unlock(&lock);
                        return NULL;
                    }
                    if (cnt >= page_nr)
                    {
                        for (int m = find_inx; m < find_inx + cnt; m++)
                        {

                            MK_SET_BIT(mm_bitemp[m / WORD_BITS], m % WORD_BITS);
                        }
                        pthread_spin_unlock(&lock);
                        // printf("st_inx:%d, cnt:%d\n", find_inx, cnt);
                        return find_inx * PAGE_SIZE + (char *)heap_addr;
                    }
                }
                else
                {
                    cnt = 0;
                    find_inx = -1;
                }
            }
        }
    }
    pthread_spin_unlock(&lock);

    return NULL;
}

static void mm_page_free(int st, int nr)
{
    app_info_t *info = app_info_get(app_start_addr);
    assert(info);
    void *heap_addr = (void *)((umword_t)TASK_RAM_BASE() + info->i.heap_offset - info->i.data_offset);
    size_t max_page_nr = (info->i.heap_size) / PAGE_SIZE;

    pthread_spin_lock(&lock);
    for (int i = st; (i < st + nr) && (i < max_page_nr); i++)
    {
        MK_CLR_BIT(mm_bitemp[i / WORD_BITS], i % WORD_BITS);
    }
    pthread_spin_unlock(&lock);
}

static int _sys_mmap2(void *start, size_t len, int prot, int flags, int fd, off_t _offset, umword_t *addr)
{
    assert(addr);
    if (fd >= 0)
    {
        return -ENOSYS;
    }
    len = ALIGN(len, PAGE_SIZE);
    *addr = (umword_t)mm_page_alloc(len / PAGE_SIZE);
    if (*addr == 0)
    {
        return -ENOMEM;
    }
    return 0;
}
umword_t be_mmap2(void *start,
                  size_t len,
                  long prot,
                  long flags,
                  long fd,
                  long _offset)
{
    umword_t addr;

    if (len == 0)
    {
        return 0;
    }
    int ret = _sys_mmap2(start, len, prot, flags, fd, _offset, &addr);

    if (ret < 0)
    {
        return (umword_t)ret;
    }
    return addr;
}
umword_t sys_mmap2(va_list ap)
{
    void *start;
    size_t len;
    long prot;
    long flags;
    long fd;
    long _offset;

    ARG_6_BE(ap, start, void *, len, size_t, prot, long, flags, long, fd, long, _offset, long);

    return be_mmap2(start, len, prot, flags, fd, _offset);
}
umword_t be_munmap(void *start, size_t len)
{
    app_info_t *info = app_info_get(app_start_addr);
    assert(info);
    void *heap_addr = (void *)((umword_t)TASK_RAM_BASE() + info->i.heap_offset - info->i.data_offset);

    len = ALIGN(len, PAGE_SIZE);
    // printf("munmap 0x%x, 0x%x.\n", start, len);
    mm_page_free(((umword_t)(start) - (umword_t)heap_addr) / PAGE_SIZE, len / PAGE_SIZE);
}
umword_t sys_munmap(va_list ap)
{
    void *start;
    size_t len;

    ARG_2_BE(ap, len, size_t, start, void *);

    return be_munmap(start, len);
}
