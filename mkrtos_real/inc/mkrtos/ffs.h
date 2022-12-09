/*
 * ffs.h
 *
 *  Created on: 2022年9月11日
 *      Author: Administrator
 */

#ifndef INC_MKRTOS_FFS_H_
#define INC_MKRTOS_FFS_H_

#include <type.h>
#include <mkrtos.h>
#define FFS_MAGIC_NUM 0x11223344

struct ffs_inode {
	uint32_t start_bk;	//!<文件开始的bk位置
	uint32_t bk_num;	//!<文件占用的bk数
};

//sp文件系统私有的sb信息
struct ffs_super_block {
	//块总数
	uint32_t block_count;
	//空余块数
	uint32_t block_free;

	//iNode大小
	uint32_t inode_size;
	//iNode总数
	uint32_t inode_count;
	//iNode空余
	uint32_t inode_free;

	//文件系统信息块启动位置
	uint32_t fs_info_st_bk_inx;

	//iNode使用块启动位置
	uint32_t inode_used_bk_st_inx;
	//iNode使用块 占用多少块
	uint32_t inode_used_bk_count;

	//bk使用块启动位置
	uint32_t bk_used_bk_st_inx;
	//bk使用块 占用了多少块
	uint32_t bk_used_bk_count;

	//iNode数据开始位置
	uint32_t inode_data_bk_st_inx;
	uint32_t inode_data_bk_count;
	//数据块开始位置
	uint32_t data_bk_st_inx;

	uint32_t last_alloc_bk_inx;
};

#endif /* INC_MKRTOS_FFS_H_ */
