#ifndef _SPIN_LOCK_H__
#define _SPIN_LOCK_H__

#include <arch/atomic.h>

//自旋锁多线程同步也可以用哦

/**
* @brief
*/
struct spin_lock{
	/*互斥量锁标志*/
	atomic_t lock;
};

//#define SPINLOCK_INIT(a) ((struct spin_lock){.lock.counter=(a)})

/**
* @brief 自旋锁创建
*/
struct spin_lock* spin_lock_create(void);
/**
 * @brief 销毁自旋锁
 * 
 * @param pslh 
 */
void spin_lock_destory(struct spin_lock* pslh);
/**
* @brief 需要初始化的自旋锁
*/
void spin_lock_init(struct spin_lock* pslh);
/**
 * @brief 尝试加锁
 */
int32_t spin_try_lock(struct spin_lock* psh);
/**
* @brief 锁上
* @param psh 上锁的自旋锁
*/
int32_t spin_lock(struct spin_lock* psh);
/**
* @brief 解锁
* @param psh 解锁的自旋锁
*/ 
int32_t spin_unlock(struct spin_lock* psh);

#endif
