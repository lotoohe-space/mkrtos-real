#ifndef _ARM_H__
#define _ARM_H__
#include <type.h>


/**
* @breif ԭ�Ӳ����ı���
*/
typedef struct {
	volatile int32_t counter;
} Atomic_t;

/**
* @breif ԭ�ӱ����Լ�
*/
 uint32_t atomic_inc(volatile Atomic_t* val);
/**
* @breif ԭ�ӱ����Լ�
*/
 uint32_t atomic_dec(volatile Atomic_t* val);
/**
* @breif ����ԭ�ӱ�����ֵ
*/
 void atomic_set(volatile Atomic_t* val,uint32_t i);
/**
* ����ֵ��������֮ǰ��ֵ
*/
 uint32_t atomic_ret_set(volatile Atomic_t* val,uint32_t i);
/**
* @breif ��ȡԭ�ӱ�����ֵ
*/
 uint32_t atomic_read(volatile Atomic_t* val);
/**
* @breif ����һ����
*/ 
 void atomic_add(volatile Atomic_t* a,uint32_t val);
/**
* @breif ��ȥһ����
*/
 void AtomicSub(volatile Atomic_t* a,uint32_t val);
 uint32_t atomic_test(volatile Atomic_t* a,uint32_t val);
/**
* @breif ���Ե���0������и�ֵ�������ز��ԵĽ��
* @return 1����0 ��0������0
*/
 uint32_t atomic_test_set(volatile Atomic_t* a,uint32_t val);
 /**
  * ����val;������֮ǰ�Ľ���������֮ǰ��0���򷵻�1�����򷵻�0
  * @param a
  * @param val
  * @return
  */
uint32_t atomic_set_test(volatile Atomic_t* a,uint32_t val);
 /**
  * �Լ�1�������ؼ�֮ǰ�Ľ���������֮ǰ��0���򷵻�1�����򷵻�0
  * @param a
  * @return
  */
uint32_t atomic_inc_test(volatile Atomic_t* a);
/**
* @breif ���Ե���0�������+1�������ز��ԵĽ�������򣬲�+1
* @return 1����0 ��0������0
 */
uint32_t atomic_test_inc(volatile Atomic_t* a);
/**
* @breif ���Դ���0�����1;��֮����;
* @return 1������0 ��0��������0
*/
 uint32_t atomic_test_dec_nq(volatile Atomic_t* a);
/**
 * @brief �Ƚ�a��b�Ĵ�С,���b<=a a+=1������b<=a����1������0
 *
 * @param a
 * @param b
 * @return uint32_t
 */
uint32_t atomic_cmp_hi_inc(volatile Atomic_t* a,volatile uint32_t b);

/**
 * @brief �Ƚ�a��b�Ĵ�С,���a<=b a+=1������a<=b����1������0
 *
 * @param a
 * @param b
 * @return uint32_t
 */
uint32_t atomic_cmp_hi_inc1(volatile Atomic_t* a,volatile uint32_t b);



#endif
