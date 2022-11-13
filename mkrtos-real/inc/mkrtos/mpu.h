/*
 * mpu.h
 *
 *  Created on: 2022��10��15��
 *      Author: zhangzheng
 */

#ifndef INC_MKRTOS_MPU_H_
#define INC_MKRTOS_MPU_H_

#include <type.h>
#include <mkrtos/sched.h>
#define REGION_NUM 8

typedef struct mpu {
	struct region_info {
		uint32_t start_addr;//�ڴ�����Ŀ�ʼ��ַ
		
		uint32_t block_start_addr;//������Ŀ�ʼ��ַ
		uint32_t block_size;	//!�����Ŀ��С
		uint32_t size;			//ʵ��������ڴ��С
		uint8_t region;

		uint32_t rbar;
		uint32_t rasr;
	} mpu_info[REGION_NUM];
} mpu_t; 						//!<mpu��ص���Ϣ

struct task;

void mpu_disable(void);
void mpu_enable(void);
void mpu_set(struct region_info* region_i, void* mem_start_addr, int size, int *ret_align_size);
void mpu_update_to(struct task* tk);
void mpu_update(void);

#endif /* INC_MKRTOS_MPU_H_ */
