/*
 * mpu.c
 *
 *  Created on: 2022年7月26日
 *      Author: Administrator
 */

#include <type.h>
#include <gd32f10x.h>
#include <mkrtos/sched.h>
#include <assert.h>
#include <mpu_armv7.h>

uint32_t *MPUCR = (uint32_t *)0xE000ED94;

void mpu_disable(void){
	ARM_MPU_Disable();
}
void mpu_enable(void){
	ARM_MPU_Enable(4);
}

/**
 * 设置mpu表
 */
void mpu_set(struct region_info* region_i, void* mem_start_addr, int size, int *ret_align_size)
{
	int ffs_t_;
	int ffs_t;

	ffs_t_ = ffs(size);
//	if (!is_power_of_2(size)) {
//		ffs_t_++;
//	}
	ffs_t = 1 << ffs_t_;

	int sub_region_t = ffs_t >> 3;

	int align_sub_size = ALIGN(size, sub_region_t);//获得需要分配的大小，就是子区域对齐的大小。

	uint32_t mem_align_sub_mem_addr = ALIGN((uint32_t)mem_start_addr, sub_region_t);//子区域对齐地址
	uint32_t mem_align_up_mem_addr = ALIGN((uint32_t)mem_start_addr, ffs_t);//区域对齐地址
	uint32_t mem_align_down_mem_addr = ALIGN_DOWN((uint32_t)mem_start_addr, ffs_t);//区域对齐地址

	region_i[0].start_addr = mem_align_sub_mem_addr;
	region_i[0].size = mem_align_up_mem_addr - mem_align_sub_mem_addr;
	region_i[0].block_size = ffs_t;
	region_i[0].block_start_addr=mem_align_down_mem_addr;

	region_i[0].region = 0xff;
	for (uint32_t i = mem_align_down_mem_addr
		; i < mem_align_up_mem_addr
		; i += sub_region_t) {
		if (i < mem_align_sub_mem_addr) {
			//禁止的区域赋值为1
			region_i[0].region |= 1 << ((i - mem_align_down_mem_addr) / sub_region_t);
		}
		else {
			region_i[0].region &= ~(1 << ((i - mem_align_down_mem_addr) / sub_region_t));
		}
	}

	region_i[1].region = 0x00;
	for (uint32_t i = mem_align_up_mem_addr
		; i < mem_align_up_mem_addr + ffs_t
		; i += sub_region_t) {
		if (i < mem_align_sub_mem_addr + align_sub_size) {
			region_i[1].region &= ~(1 << ((i - mem_align_up_mem_addr) / sub_region_t));
		}
		else {
			region_i[1].region |= (1 << ((i - mem_align_up_mem_addr) / sub_region_t));
		}
	}
	region_i[1].start_addr = mem_align_up_mem_addr;
	region_i[1].size = (mem_align_sub_mem_addr + align_sub_size) - mem_align_up_mem_addr;
	region_i[1].block_size = ffs_t;
	region_i[1].block_start_addr=mem_align_up_mem_addr;

	*ret_align_size = sub_region_t;

#if 0
	kprint("st:0x%x re:0x%x sub:0x%x\n region:[", region_i[0].block_start_addr, region_i[0].region , sub_region_t);
	for (int i = 0; i < 8; i++) {
		if (region_i[0].region & (1 << i)) {
			kprint("x");
		}
		else {
			kprint("o");
		}
	}
	kprint("]\n");
	kprint("st:0x%x re:0x%x sub:0x%x\n region:[", region_i[1].block_start_addr, region_i[1].region, sub_region_t);
	for (int i = 0; i < 8; i++) {
		if (region_i[1].region & (1 << i)) {
			kprint("x");
		}
		else {
			kprint("o");
		}
	}
	kprint("]\n");
#endif
	region_i[0].rbar = ARM_MPU_RBAR(0, region_i[0].block_start_addr);
	region_i[0].rasr = ARM_MPU_RASR(0UL, ARM_MPU_AP_FULL, 0UL
				, 0UL, 1UL, 1UL, region_i[0].region, ffs_t_ - 1);

	region_i[1].rbar = ARM_MPU_RBAR(1, region_i[1].block_start_addr);
	region_i[1].rasr = ARM_MPU_RASR(0UL, ARM_MPU_AP_FULL, 0UL
				, 0UL, 1UL, 1UL, region_i[1].region, ffs_t_ - 1);

	region_i[2].rbar = ARM_MPU_RBAR(2, 0x8000000/*TODO:*/);
	region_i[2].rasr = ARM_MPU_RASR(0UL, ARM_MPU_AP_URO, 0UL
					, 0UL, 1UL, 1UL, 0x0/*TODO:*/, ffs(1024*1024) - 1/*TODO:*/);
}


void mpu_update(void){
	struct task *curr_task;

	curr_task = get_current_task();

	mpu_disable();
	if(curr_task->mpu){
		ARM_MPU_SetRegion(curr_task->mpu->mpu_info[0].rbar, curr_task->mpu->mpu_info[0].rasr);
		ARM_MPU_SetRegion(curr_task->mpu->mpu_info[1].rbar, curr_task->mpu->mpu_info[1].rasr);
		ARM_MPU_SetRegion(curr_task->mpu->mpu_info[2].rbar, curr_task->mpu->mpu_info[2].rasr);
	}else{
		ARM_MPU_ClrRegion(0);
		ARM_MPU_ClrRegion(1);
		ARM_MPU_ClrRegion(2);
	}

	mpu_enable();
}


