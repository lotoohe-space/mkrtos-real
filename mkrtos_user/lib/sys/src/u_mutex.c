#include "u_sema.h"
#include "u_mutex.h"
#include "u_types.h"
#include "u_prot.h"
#include "u_thread.h"
#include "u_task.h"
#include "u_factory.h"
/**
 * @brief 创建一个mutex
 *
 * @param lock
 * @return int
 */
int u_mutex_init(u_mutex_t *lock, obj_handler_t sema_hd)
{
    msg_tag_t tag;

    if (sema_hd == HANDLER_INVALID)
    {
        return -1;
    }
    tag = facotry_create_sema(FACTORY_PROT, vpage_create_raw3(KOBJ_ALL_RIGHTS, 0, sema_hd), 1, 1);
    if (msg_tag_get_val(tag) < 0)
    {
        return msg_tag_get_val(tag);
    }
    lock->obj = sema_hd;
    return 0;
}
void u_mutex_lock(u_mutex_t *lock, umword_t timeout, umword_t *remain_times)
{
    u_sema_down(lock->obj, timeout, remain_times);
}
void u_mutex_unlock(u_mutex_t *lock)
{
    u_sema_up(lock->obj);
}
