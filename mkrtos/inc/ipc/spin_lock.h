#ifndef _SPIN_LOCK_H__
#define _SPIN_LOCK_H__

#include <arch/atomic.h>

/**
* @breif 
*/
typedef struct spin_lock{
	/*����������־*/
	Atomic_t lock;
}*pspinlock_handler,spinlock_handler;

/**
* @breif ����������
*/
pspinlock_handler spin_lock_create(void);
/**
 * @brief ����������
 * 
 * @param pslh 
 */
void spin_lock_destory(pspinlock_handler pslh);
/**
* @breif ��������ʼ��
* @breif ��Ҫ��ʼ����������
*/
void spin_lock_init(pspinlock_handler pslh);
int32_t spin_try_lock(pspinlock_handler psh);
/**
* @breif ����
* @param psh ������������
*/
int32_t spin_lock(pspinlock_handler psh);
/**
* @breif ����
* @param psh ������������
*/ 
int32_t spin_unlock(pspinlock_handler psh);

#endif
