/*
 * config.h
 *
 *  Created on: 2022��7��23��
 *      Author: Administrator
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#include <arch/arch.h>
#include <mkrtos.h>

#define SYS_NAME "mkrtos"
#define SYS_VERSION "0.01"
#define SYS_RELEASE "0.01"
#define SYS_DOMAINNAME SYS_NAME
#define SYS_MACHINE "cortex-m3 open"

#define MKRTOS_USED_NET 0 //!<ʹ��ʹ������ӿ�
#define MKRTOS_USED_M_PIPE 1 //!<ʹ������pipe

#define SECTION(x)	__attribute__((section(x)))

#define INIT_LV0(name)  const init_func mkrtos_init_func_##name SECTION(".mkrtos.init.0") = name
#define INIT_LV1(name)  const init_func mkrtos_init_func_##name SECTION(".mkrtos.init.1") = name
#define INIT_LV2(name)  const init_func mkrtos_init_func_##name SECTION(".mkrtos.init.2") = name
#define INIT_LV3(name)  const init_func mkrtos_init_func_##name SECTION(".mkrtos.init.3") = name

#define ARCH_INIT() arch_init()

#endif /* INC_CONFIG_H_ */
