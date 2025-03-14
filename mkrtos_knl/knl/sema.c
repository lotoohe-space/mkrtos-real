
#include "sema.h"
#include "atomics.h"
#include "factory.h"
#include "init.h"
#include "kobject.h"
#include "ref.h"
#include "sleep.h"
#include "slist.h"
#include "spinlock.h"
#include "thread.h"
#include "types.h"
#if IS_ENABLED(CONFIG_BUDDY_SLAB)
#include <slab.h>
static slab_t *sema_slab;
#endif

enum SEMA_OP
{
    SEMA_UP,
    SEMA_DOWN,
};

static void sema_mem_init(void)
{
#if IS_ENABLED(CONFIG_BUDDY_SLAB)
    sema_slab = slab_create(sizeof(sema_t), "sema");
    assert(sema_slab);
#endif
}
INIT_KOBJ_MEM(sema_mem_init);
typedef struct sema_wait_item
{
    slist_head_t node;
    thread_t *thread;
} sema_wait_item_t;

static void sema_wait_item_init(sema_wait_item_t *obj, thread_t *th)
{
    slist_init(&obj->node);
    obj->thread = th;
}

void sema_up(sema_t *obj)
{
    assert(obj);
    umword_t status;
    thread_t *th = thread_get_current();

again:
    status = spinlock_lock(&obj->lock);
    if (slist_is_empty(&obj->suspend_head))
    {
        if (obj->cnt < obj->max_cnt)
        {
            obj->cnt++;
        }
        // printk("up0 sema cnt:%d max:%d.\n", obj->cnt, obj->max_cnt);
    }
    else
    {
        slist_head_t *first_wait_node;
        sema_wait_item_t *first_wait;

        first_wait_node = slist_first(&obj->suspend_head);
        first_wait = container_of(first_wait_node, sema_wait_item_t, node);
        if (thread_get_status(first_wait->thread) == THREAD_SUSPEND)
        {
            slist_del(first_wait_node);
            if (ref_counter_dec_and_release(&first_wait->thread->ref, &first_wait->thread->kobj) != 1)
            {
                thread_sleep_del_and_wakeup(first_wait->thread);
            }
            if (obj->cnt < obj->max_cnt)
            {
                obj->cnt++;
            }
            if (obj->max_cnt == 1 && obj->hold_th == &th->kobj)
            {
                //还原优先级
                thread_set_prio(th, obj->hold_th_prio);
                obj->hold_th = NULL;
            }
        }
        else
        {
            // 超时退出，但是切出来的时候没有切到休眠线程，切到了这里。
            spinlock_set(&obj->lock, status);
            goto again;
        }
    }
    spinlock_set(&obj->lock, status);
    if (cpulock_get_status())
    {
        preemption();
    }
}
umword_t sema_down(sema_t *obj, umword_t ticks)
{
    assert(obj);
    thread_t *th = thread_get_current();
    umword_t status;
    umword_t remain_sleep = 0;
    sema_wait_item_t wait_item;

again:
    status = spinlock_lock(&obj->lock);
    if (obj->cnt == 0)
    {
        sema_wait_item_init(&wait_item, th);
        ref_counter_inc(&th->ref);
        slist_add_append(&obj->suspend_head, &wait_item.node);
        if (obj->max_cnt == 1 && obj->hold_th)
        {
            if (thread_get_prio(th) > thread_get_prio((thread_t*)(obj->hold_th)))
            {
                //执行优先级继承
                thread_set_prio(((thread_t*)(obj->hold_th)), thread_get_prio(th));
            }
        }
        remain_sleep = thread_sleep(ticks); //注意：这里可能不是up环保型
        if (remain_sleep == 0 && ticks != 0)
        {
            // 超时退出的，直接从列表中删除
            assert(slist_in_list(&wait_item.node));
            slist_del(&wait_item.node);
            ref_counter_dec(&th->ref);
        }
        else
        {
            spinlock_set(&obj->lock, status);
            if (cpulock_get_status())
            {
                preemption();
            }
            goto again;
        }
    }
    else
    {
        assert(obj->cnt > 0);
        obj->cnt--;
        if (obj->max_cnt == 1)
        {
            obj->hold_th = &th->kobj;
            obj->hold_th_prio = thread_get_prio(th);
        }
    }
    spinlock_set(&obj->lock, status);
    return remain_sleep;
}

static void sema_syscall(kobject_t *kobj, syscall_prot_t sys_p,
                         msg_tag_t in_tag, entry_frame_t *f)
{
    sema_t *sema = container_of(kobj, sema_t, kobj);
    msg_tag_t tag = msg_tag_init4(0, 0, 0, -EINVAL);
    task_t *task = thread_get_current_task();

    if (sys_p.prot != SEMA_PROT)
    {
        f->regs[0] = msg_tag_init4(0, 0, 0, -EPROTO).raw;
        return;
    }
    switch (sys_p.op)
    {
    case SEMA_UP:
    {
        sema_up(sema);
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    break;
    case SEMA_DOWN:
    {
        umword_t ret;

        ret = sema_down(sema, f->regs[0]);
        f->regs[1] = ret;
        tag = msg_tag_init4(0, 0, 0, 0);
    }
    }

    f->regs[0] = tag.raw;
}
static sema_t *sema_create(ram_limit_t *lim, umword_t cnt, umword_t max)
{
    sema_t *kobj = NULL;

#if IS_ENABLED(CONFIG_BUDDY_SLAB)
    kobj = mm_limit_alloc_slab(sema_slab, lim);
#else
    kobj = mm_limit_alloc(lim, sizeof(sema_t));
#endif
    if (!kobj)
    {
        return NULL;
    }
    sema_init(kobj, cnt, max);

    return kobj;
}
static void sema_unmap(obj_space_t *obj_space, kobject_t *kobj)
{
}
static void sema_release_stage1(kobject_t *kobj)
{
    sema_t *obj = container_of(kobj, sema_t, kobj);
    kobject_invalidate(kobj);
    sema_wait_item_t *wait_item;

    slist_foreach(wait_item, &obj->suspend_head, node)
    {
        slist_head_t *first_wait_node;
        sema_wait_item_t *first_wait;

        first_wait_node = slist_first(&obj->suspend_head);
        first_wait = container_of(first_wait_node, sema_wait_item_t, node);
        slist_del(first_wait_node);
        if (ref_counter_dec_and_release(&first_wait->thread->ref, &first_wait->thread->kobj) != 1)
        {
            thread_ready_remote(first_wait->thread, FALSE);
        }
        if (obj->cnt < obj->max_cnt)
        {
            obj->cnt++;
        }
    }
}
static void sema_release_stage2(kobject_t *kobj)
{
    sema_t *obj = container_of(kobj, sema_t, kobj);

#if IS_ENABLED(CONFIG_BUDDY_SLAB)
    mm_limit_free_slab(sema_slab, thread_get_current_task()->lim, obj);
#else
    mm_limit_free(thread_get_current_task()->lim, obj);
#endif
    printk("sema 0x%x free.\n", kobj);
}

void sema_init(sema_t *obj, int cnt, int max)
{
    if (max <= 0)
    {
        max = 1;
    }
    if (cnt < 0)
    {
        cnt = 0;
    }
    if (cnt > max)
    {
        cnt = max;
    }
    obj->cnt = cnt;
    kobject_init(&obj->kobj, SEMA_TYPE);
    spinlock_init(&obj->lock);
    slist_init(&obj->suspend_head);
    obj->max_cnt = max <= 0 ? 1 : max;
    obj->kobj.invoke_func = sema_syscall;
    // obj->kobj.put_func = thread_put;
    obj->kobj.stage_1_func = sema_release_stage1;
    obj->kobj.stage_2_func = sema_release_stage2;

    printk("sema init cnt:%d max:%d.\n", cnt, max);
}
static kobject_t *sema_func(ram_limit_t *lim, umword_t arg0, umword_t arg1,
                            umword_t arg2, umword_t arg3)
{
    sema_t *sema = sema_create(lim, arg0, arg1);
    if (!sema)
    {
        return NULL;
    }
    return &sema->kobj;
}
/**
 * @brief 工厂注册函数
 *
 */
static void sema_factory_register(void)
{
    factory_register(sema_func, SEMA_PROT);
}
INIT_KOBJ(sema_factory_register);
