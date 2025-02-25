#include "u_types.h"
#include "syscall_backend.h"
#include "u_prot.h"
#include "u_ipc.h"
#include "u_app.h"
#include "cons_cli.h"
#include "u_arch.h"
#include "u_mutex.h"
#include "u_hd_man.h"
#include <pthread_impl.h>
#include <assert.h>
#include <u_vmam.h>

static int lock_is_init;
static u_mutex_t lock;
extern void *app_start_addr;
#ifdef CONFIG_EX_RAM_SIZE
static umword_t mm_bitmap[ROUND(CONFIG_EX_RAM_SIZE, MK_PAGE_SIZE) / (sizeof(umword_t) * 8) + 1];
#else
static umword_t mm_bitmap[ROUND(CONFIG_SYS_DATA_SIZE, MK_PAGE_SIZE) / (sizeof(umword_t) * 8) + 1];
#endif
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
    if (max_page_nr > sizeof(mm_bitmap) * WORD_BITS)
    {
        cons_write_str("mm bitmap is to small.\n");
    }
    // printf("heap is 0x%x, max page nr is %d.\n", heap_addr, max_page_nr);
    if (!lock_is_init)
    {
        obj_handler_t mutex_hd;

        mutex_hd = handler_alloc();
        if (mutex_hd == HANDLER_INVALID)
        {
            cons_write_str("mutex_hd alloc failed.\n");
            assert(0);
        }
        u_mutex_init(&lock, handler_alloc());
        lock_is_init = 1;
    }
    u_mutex_lock(&lock, 0, NULL);
    for (umword_t i = 0; i < ROUND_UP(max_page_nr, WORD_BITS); i++)
    {
        if (mm_bitmap[i] != (umword_t)(-1))
        {
            for (int j = 0; j < WORD_BITS; j++)
            {
                if (MK_GET_BIT(mm_bitmap[i], j) == 0)
                {
                    // 找到空闲的
                    if (find_inx == -1)
                    {
                        find_inx = i * WORD_BITS + j;
                    }
                    cnt++;
                    if (find_inx + cnt > max_page_nr)
                    {
                        u_mutex_unlock(&lock);
                        return NULL;
                    }
                    if (cnt >= page_nr)
                    {
                        for (int m = find_inx; m < find_inx + cnt; m++)
                        {

                            MK_SET_BIT(mm_bitmap[m / WORD_BITS], m % WORD_BITS);
                        }
                        u_mutex_unlock(&lock);
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
    u_mutex_unlock(&lock);

    return NULL;
}

static int mm_page_free(int st, int nr)
{
    app_info_t *info = app_info_get(app_start_addr);
    assert(info);
    void *heap_addr = (void *)((umword_t)TASK_RAM_BASE() + info->i.heap_offset - info->i.data_offset);
    size_t max_page_nr = (info->i.heap_size) / PAGE_SIZE;

    if (st + nr >= max_page_nr)
    {
        return -EINVAL;
    }
    u_mutex_lock(&lock, 0, NULL);
    for (int i = st; (i < st + nr) && (i < max_page_nr); i++)
    {
        MK_CLR_BIT(mm_bitmap[i / WORD_BITS], i % WORD_BITS);
    }
    u_mutex_unlock(&lock);
    return 0;
}

static int _sys_mmap2(void *start, size_t len, int prot, int flags, int fd, off_t _offset, umword_t *addr)
{
    assert(addr);
    if (fd >= 0)
    {
        return -ENOSYS;
    }
    len = ALIGN(len, MK_PAGE_SIZE);
    if (prot & PROT_PFS)
    {
        msg_tag_t tag;
        addr_t ret_addr;
        uint8_t mk_prot = 0;

        if (prot & PROT_READ)
        {
            mk_prot |= VPAGE_PROT_RO;
        }
        if (prot & PROT_WRITE)
        {
            mk_prot |= VPAGE_PROT_WO;
        }
        if (prot & PROT_EXEC)
        {
            mk_prot |= VPAGE_PROT_X;
        }
        tag = u_vmam_alloc(VMA_PROT,
                           vma_addr_create(mk_prot,
                                           VMA_ADDR_PAGE_FAULT_SIM | VMA_ADDR_PAGE_FAULT_DSCT,
                                           0),
                           len,
                           (addr_t)start, &ret_addr);
        if (msg_tag_get_val(tag) < 0)
        {
            return msg_tag_get_val(tag);
        }
        *addr = ret_addr;
    }
    else
    {
        // if (len & (PAGE_SIZE - 1))
        // {
        //     return -EINVAL;
        // }
        *addr = (umword_t)mm_page_alloc(len / PAGE_SIZE);
    }
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
long sys_mmap2(va_list ap)
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
long be_munmap(void *start, size_t len)
{
    msg_tag_t tag;
    app_info_t *info = app_info_get(app_start_addr);
    assert(info);
    void *heap_addr = (void *)((umword_t)TASK_RAM_BASE() + info->i.heap_offset - info->i.data_offset);

    if (len & (PAGE_SIZE - 1))
    {
        return -EINVAL;
    }
    // printf("munmap 0x%x, 0x%x.\n", start, len);
    tag = u_vmam_free(VMA_PROT, (addr_t)(start), len);
    if (msg_tag_get_val(tag) < 0)
    {
        return mm_page_free(((umword_t)(start) - (umword_t)heap_addr) / PAGE_SIZE, len / PAGE_SIZE);
    }
    return 0;
}
long sys_munmap(va_list ap)
{
    void *start;
    size_t len;

    ARG_2_BE(ap, len, size_t, start, void *);

    return be_munmap(start, len);
}
