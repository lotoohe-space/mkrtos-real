
#include "u_types.h"
#include "u_arch.h"
#include "u_util.h"
#include "u_task.h"
#include "u_hd_man.h"
#include <pthread.h>
#define HANDLER_START_INX 10 //!< fd开始的值，前10个内核保留
#define HANDLER_MAX_NR (HANDLER_START_INX + \
                        (CONFIG_OBJ_MAP_ENTRY_SIZE) * (CONFIG_OBJ_MAP_TAB_SIZE)) //!< 单个task最大支持的hd数量

static umword_t bitmap_handler_alloc[HANDLER_MAX_NR / WORD_BITS];
static pthread_spinlock_t lock;

void hanlder_pre_alloc(obj_handler_t inx)
{
    inx -= HANDLER_START_INX;
    umword_t word_offset = inx / WORD_BITS;
    umword_t bits_offset = inx % WORD_BITS;

    if (word_offset >= (HANDLER_MAX_NR / WORD_BITS))
    {
        return;
    }

    pthread_spin_lock(&lock);
    MK_SET_BIT(bitmap_handler_alloc[word_offset], bits_offset);
    pthread_spin_unlock(&lock);
}

obj_handler_t handler_alloc(void)
{
    pthread_spin_lock(&lock);
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
                    pthread_spin_unlock(&lock);
                    return HANDLER_START_INX + find_inx;
                }
            }
        }
    }
    pthread_spin_unlock(&lock);

    return HANDLER_INVALID;
}
bool_t handler_is_used(obj_handler_t hd_inx)
{
    hd_inx -= HANDLER_START_INX;
    if (hd_inx < 0)
    {
        return 0;
    }

    umword_t word_offset = hd_inx / WORD_BITS;
    umword_t bits_offset = hd_inx % WORD_BITS;

    if (word_offset >= (HANDLER_MAX_NR / WORD_BITS))
    {
        return 0;
    }
    pthread_spin_lock(&lock);
    bool_t is_use = !!MK_GET_BIT(bitmap_handler_alloc[word_offset], bits_offset);
    pthread_spin_unlock(&lock);

    return is_use;
}
void handler_free(obj_handler_t hd_inx)
{
    hd_inx -= HANDLER_START_INX;
    if (hd_inx < 0)
    {
        return;
    }

    umword_t word_offset = hd_inx / WORD_BITS;
    umword_t bits_offset = hd_inx % WORD_BITS;

    if (word_offset >= (HANDLER_MAX_NR / WORD_BITS))
    {
        return;
    }
    pthread_spin_lock(&lock);
    MK_CLR_BIT(bitmap_handler_alloc[word_offset], bits_offset);
    pthread_spin_unlock(&lock);
}
/**
 * @brief 删除用户态的hd inx，并释放内核中的内核对象
 *
 * @param hd_inx
 */
void handler_free_umap(obj_handler_t hd_inx)
{
    handler_free(hd_inx);
    task_unmap(TASK_THIS, vpage_create_raw3(0, 0, hd_inx));
}
/**
 * @brief 删除用户态的hd，并从内核中删除
 *
 * @param hd_inx
 */
void handler_del_umap(obj_handler_t hd_inx)
{
    handler_free(hd_inx);
    task_unmap(TASK_THIS, vpage_create_raw3(KOBJ_DELETE_RIGHT, 0, hd_inx));
}
