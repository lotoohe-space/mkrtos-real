
#include "u_types.h"
#include "u_arch.h"
#include "u_util.h"
#include "u_task.h"
#include "u_hd_man.h"

#define HANDLER_START_INX 10
#define HANDLER_MAX_NR 64

static umword_t bitmap_handler_alloc[HANDLER_MAX_NR / WORD_BYTES];

// TODO:加锁
obj_handler_t handler_alloc(void)
{
    for (umword_t i = 0; i < ARRAY_SIZE(bitmap_handler_alloc); i++)
    {
        if (bitmap_handler_alloc[i] != (umword_t)(-1))
        {
            for (int j = 0; j < WORD_BITS; j++)
            {
                if (MK_GET_BIT(bitmap_handler_alloc[i], j) == 0)
                {
                    // 找到空闲的
                    umword_t find_inx = i * WORD_BITS + j;
                    MK_SET_BIT(bitmap_handler_alloc[i], j);
                    return HANDLER_START_INX + find_inx;
                }
            }
        }
    }
    return HANDLER_INVALID;
}
// TODO:加锁
void handler_free(obj_handler_t hd_inx)
{
    hd_inx -= HANDLER_START_INX;
    umword_t word_offset = hd_inx / WORD_BITS;
    umword_t bits_offset = hd_inx % WORD_BITS;
    MK_CLR_BIT(bitmap_handler_alloc[word_offset], bits_offset);
}
/**
 * @brief 删除用户态的hd inx，并释放内核中的内核对象
 *
 * @param hd_inx
 */
void handler_free_umap(obj_handler_t hd_inx)
{
    task_unmap(TASK_THIS, vpage_create_raw3(0, 0, hd_inx));
    handler_free(hd_inx);
}
