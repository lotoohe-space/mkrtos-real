#ifndef _SPIN_LOCK_H__
#define _SPIN_LOCK_H__

#include <arch/atomic.h>

//���������߳�ͬ��Ҳ������Ŷ

/**
* @brief
*/
struct spin_lock{
	/*����������־*/
	atomic_t lock;
};

//#define SPINLOCK_INIT(a) ((struct spin_lock){.lock.counter=(a)})

/**
* @brief ����������
*/
struct spin_lock* spin_lock_create(void);
/**
 * @brief ����������
 * 
 * @param pslh 
 */
void spin_lock_destory(struct spin_lock* pslh);
/**
* @brief ��Ҫ��ʼ����������
*/
void spin_lock_init(struct spin_lock* pslh);
/**
 * @brief ���Լ���
 */
int32_t spin_try_lock(struct spin_lock* psh);
/**
* @brief ����
* @param psh ������������
*/
int32_t spin_lock(struct spin_lock* psh);
/**
* @brief ����
* @param psh ������������
*/ 
int32_t spin_unlock(struct spin_lock* psh);

#endif
