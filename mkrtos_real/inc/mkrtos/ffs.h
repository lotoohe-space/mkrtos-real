/*
 * ffs.h
 *
 *  Created on: 2022��9��11��
 *      Author: Administrator
 */

#ifndef INC_MKRTOS_FFS_H_
#define INC_MKRTOS_FFS_H_

#include <type.h>
#include <mkrtos.h>
#define FFS_MAGIC_NUM 0x11223344

struct ffs_inode {
	uint32_t start_bk;	//!<�ļ���ʼ��bkλ��
	uint32_t bk_num;	//!<�ļ�ռ�õ�bk��
};

//sp�ļ�ϵͳ˽�е�sb��Ϣ
struct ffs_super_block {
	//������
	uint32_t block_count;
	//�������
	uint32_t block_free;

	//iNode��С
	uint32_t inode_size;
	//iNode����
	uint32_t inode_count;
	//iNode����
	uint32_t inode_free;

	//�ļ�ϵͳ��Ϣ������λ��
	uint32_t fs_info_st_bk_inx;

	//iNodeʹ�ÿ�����λ��
	uint32_t inode_used_bk_st_inx;
	//iNodeʹ�ÿ� ռ�ö��ٿ�
	uint32_t inode_used_bk_count;

	//bkʹ�ÿ�����λ��
	uint32_t bk_used_bk_st_inx;
	//bkʹ�ÿ� ռ���˶��ٿ�
	uint32_t bk_used_bk_count;

	//iNode���ݿ�ʼλ��
	uint32_t inode_data_bk_st_inx;
	uint32_t inode_data_bk_count;
	//���ݿ鿪ʼλ��
	uint32_t data_bk_st_inx;

	uint32_t last_alloc_bk_inx;
};

#endif /* INC_MKRTOS_FFS_H_ */
