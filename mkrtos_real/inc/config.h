/*
 * config.h
 *
 *  Created on: 2022年7月23日
 *      Author: Administrator
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#include <arch/arch.h>
#include <mkrtos.h>
typedef int (*init_func)(void); 		//!< 初始化函数
#define SYS_NAME "mkrtos"
#define SYS_VERSION "0.01"
#define SYS_RELEASE "0.01"
#define SYS_DOMAINNAME SYS_NAME
#define SYS_MACHINE "cortex-m3 open"

#define MKRTOS_USED_NET 0       //!<使用使用网络接口
#define MKRTOS_USED_M_PIPE 1    //!<使用匿名pipe

#define SECTION(x)	__attribute__((section(x)))

//自动初始化接口
#define INIT_LV0(name)  const init_func mkrtos_init_func_##name SECTION(".mkrtos.init.0") = name
#define INIT_LV1(name)  const init_func mkrtos_init_func_##name SECTION(".mkrtos.init.1") = name
#define INIT_LV2(name)  const init_func mkrtos_init_func_##name SECTION(".mkrtos.init.2") = name
#define INIT_LV3(name)  const init_func mkrtos_init_func_##name SECTION(".mkrtos.init.3") = name

#define INIT_REG(name, LVL)\
    const init_func mkrtos_init_func_##name SECTION(".mkrtos.init."LVL) = name

#define INIT_FUNC_REG "3" 
#define INIT_PRE_BOOT "4"

/**
 * @brief 初始化调用
 * 
 */
static inline void mkrtos_init(void){
    extern ptr_t _mkrtos_init_start,_mkrtos_init_end;
	ptr_t* func_start=(ptr_t*)(&_mkrtos_init_start);
	ptr_t* func_stop=(ptr_t*)(&_mkrtos_init_end);

	for(;func_start<func_stop;func_start++){
		((init_func)(*func_start))();
	}
}

//自动测试接口
#define INIT_TEST_REG(name, LVL)\
    const init_func mkrtos_init_test_func##name SECTION(".mkrtos.test."LVL) = name
#define INIT_TEST_LVL "5"
/**
 * @brief 测试自动调用
 * 
 */
static inline void mkrtos_test(void){
    extern ptr_t _mkrtos_init_test_start,_mkrtos_init_test_end;
	ptr_t* func_start=(ptr_t*)(&_mkrtos_init_test_start);
	ptr_t* func_stop=(ptr_t*)(&_mkrtos_init_test_end);

	for(;func_start<func_stop;func_start++){
		((init_func)(*func_start))();
	}
}

#define ARCH_INIT() arch_init()

#endif /* INC_CONFIG_H_ */
