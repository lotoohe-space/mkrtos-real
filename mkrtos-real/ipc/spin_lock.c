#include <type.h>
#include <ipc/spin_lock.h>
#include <knl_service.h>
/**
* @breif 自旋锁创建
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
* @breif 自旋锁初始化
* @breif 需要初始化的自旋锁
*/
void spin_lock_init(struct spin_lock* pslh){
	if(pslh==NULL){
		return ;
	}
    atomic_set(&(pslh->lock),0);
}
/**
 * @brief 销毁自旋锁
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
 * @brief 尝试加锁
 *
 */
int32_t spin_try_lock(struct spin_lock* psh){
    if(psh==NULL){
        return -1;
    }
    if(!atomic_test_set(&(psh->lock),1)){
        /*加锁失败，挂起任务，休眠并等待被解锁*/
        return -1;
    }else{
        /*原子操作加锁成功*/
    }
    return 0;
}
/**
* @breif 锁上
* @param psh 上锁的自旋锁
*/
int32_t spin_lock(struct spin_lock* psh){
	if(psh==NULL){
		return -1;
	}
again:
	if(!atomic_test_set(&(psh->lock),1)){
		/*加锁失败，挂起任务，休眠并等待被解锁*/
		goto again;
	}else{
        /*原子操作加锁成功*/
	}
	return 0;
}
/**
* @breif 解锁
* @param psh 解锁的自旋锁
*/ 
int32_t spin_unlock(struct spin_lock* psh){
	if(psh==NULL){
		return -1;
	}
	atomic_set(&(psh->lock),0);
	return 0;
}
