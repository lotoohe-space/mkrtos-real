#include <type.h>
#include <ipc/spin_lock.h>
#include <knl_service.h>
/**
* @breif ����������
*/
struct spin_lock* spin_lock_create(void){
    struct spin_lock* psh;
	psh=malloc(sizeof(spin_lock));
	if(psh==NULL){
		return psh;
	}
    atomic_set(&(psh->lock),0);
	return psh;
}
/**
* @breif ��������ʼ��
* @breif ��Ҫ��ʼ����������
*/
void spin_lock_init(struct spin_lock* pslh){
	if(pslh==NULL){
		return ;
	}
    atomic_set(&(pslh->lock),0);
}
/**
 * @brief ����������
 * 
 * @param pslh 
 */
void spin_lock_destory(struct spin_lock* pslh){
	if(pslh==NULL){
		return ;
	}
	free(pslh);
}
/**
 * @brief ���Լ���
 *
 */
int32_t spin_try_lock(struct spin_lock* psh){
    if(psh==NULL){
        return -1;
    }
    if(!atomic_test_set(&(psh->lock),1)){
        /*����ʧ�ܣ������������߲��ȴ�������*/
        return -1;
    }else{
        /*ԭ�Ӳ��������ɹ�*/
    }
    return 0;
}
/**
* @breif ����
* @param psh ������������
*/
int32_t spin_lock(struct spin_lock* psh){
	if(psh==NULL){
		return -1;
	}
again:
	if(!atomic_test_set(&(psh->lock),1)){
		/*����ʧ�ܣ������������߲��ȴ�������*/
		goto again;
	}else{
        /*ԭ�Ӳ��������ɹ�*/
	}
	return 0;
}
/**
* @breif ����
* @param psh ������������
*/ 
int32_t spin_unlock(struct spin_lock* psh){
	if(psh==NULL){
		return -1;
	}
	atomic_set(&(psh->lock),0);
	return 0;
}
