/*
 * bmap.c
 *
 *  Created on: 2022年9月6日
 *      Author: Administrator
 */
#include <mm.h>
#include <mkrtos/fs.h>
#include <type.h>
#include <mkrtos/bk.h>
#include <string.h>
#include <mkrtos/sp.h>
#include <mkrtos/debug.h>
/**
 * 设置bk块的占用状态
 * @param sb
 * @param usedBkInx
 * @param status
 * @return
 */
int32_t sp_set_bk_no_status(struct super_block *sb, uint32_t usedBkInx,
		uint8_t status) {
	uint32_t bkInx;
	struct sp_super_block *sp_sb;
	sp_sb = sb->s_sb_priv_info;
	//先读Inode used block
	bkInx = ROUND_DOWN(usedBkInx, sb->s_bk_size * 8);
	uint8_t rByte;
	if (rbk(sb->s_dev_no, sp_sb->bk_used_bk_st_inx + bkInx, &rByte,
			ROUND_DOWN(usedBkInx, 8) % sb->s_bk_size, 1) < 0) {
		return -1;
	}
	if (!status) {
		rByte |= ~(1 << (usedBkInx % 8));
	} else {
		rByte &= (1 << (usedBkInx % 8));
	}
	//回写
	if (wbk(sb->s_dev_no, sp_sb->bk_used_bk_st_inx + bkInx, &rByte,
			ROUND_DOWN(usedBkInx, 8) % sb->s_bk_size, 1) < 0) {
		return -1;
	}

	return 0;
}
int32_t sp_get_free_bk(struct super_block *sb) {
	struct sp_super_block *sp_sb = sb->s_sb_priv_info;
	uint32_t free_bk = 0;
	int fn_inx = 0;
	uint32_t j;
	uint8_t r;
	uint8_t m;
	struct bk_cache *bk_tmp;
	for (uint32_t i = 0; i < sp_sb->bk_used_bk_count; i++) {
		bk_tmp = bk_read(sb->s_dev_no, sp_sb->bk_used_bk_st_inx + i, 1, 0);
		DEBUG("fs",INFO,"bk map %x %x\n", bk_tmp->cache[0], bk_tmp->cache[1]);
		for (j = 0; j < sb->s_bk_size; j++) {
			r = bk_tmp->cache[j];
			if (r != 0) {
				//有空的
				for (m = 0; m < 8; m++) {
					fn_inx++;
					if (fn_inx >= sp_sb->block_count) {
						bk_release(bk_tmp);
						return free_bk;
					}
					if (((r) & (1 << m))) {
						free_bk++;
					}
				}
			} else {
				fn_inx += 8;
			}
		}
		bk_release(bk_tmp);
	}
	return free_bk;
}

/**
 * 获取bit中连续1的数量
 */
int32_t get_h_bits_num(uint32_t bit, uint32_t *st_inx) {
	int32_t i;
	int32_t cn = 0;
	int32_t max_cn = 0;
	int32_t max_inx = 0;
	max_inx = 0;
	*st_inx = 0;
	for (i = 0; i < 32; i++) {
		if (bit & (1 << i)) {
			cn++;
		} else {
			if (max_cn < cn) {
				max_cn = cn;
				*st_inx = max_inx;
			}
			max_inx = i;
			cn = 0;
		}
	}
	return max_cn;
}

void sp_free_ct_bk(struct super_block *sb, uint32_t start_bk,uint32_t bk_num) {
	for(int i=0;i<bk_num;i++) {
		sp_free_bk(sb,start_bk+i);
	}
}
/**
 * 分配连续的bk数量
 */
int32_t sp_alloc_ct_bk(struct super_block *sb, uint32_t need_bk_num,
		bk_no_t *res_sart_bk) {
	struct sp_super_block *sp_sb = sb->s_sb_priv_info;
	uint32_t free_bk = 0;
	uint32_t fn_inx = 0;
	uint32_t j;
	uint8_t r;
	uint8_t m;
	struct bk_cache *bk_tmp;
	uint32_t st_bk;
	uint32_t st_byte_inx;

	uint32_t free_cn = 0;
	uint32_t start_inx = 0;

//	if (sp_sb->block_free < need_bk_num) {
//		*res_sart_bk = 0;
//		return -1;
//	}

	//最后分配的位置
	st_bk = sp_sb->last_alloc_bk_inx / (sb->s_bk_size << 3);
	//便宜的bit
	fn_inx += st_bk * (sb->s_bk_size << 3);

	st_byte_inx = (sp_sb->last_alloc_bk_inx % (sb->s_bk_size << 3)) >> 3;

	fn_inx += st_byte_inx << 3;
	for (uint32_t i = st_bk; i < sp_sb->bk_used_bk_count; i++) {
		bk_tmp = bk_read(sb->s_dev_no, sp_sb->bk_used_bk_st_inx + i, 1, 0);
		for (j = st_byte_inx; j < sb->s_bk_size; j++) {
			r = bk_tmp->cache[j];
			if (r) {
				//有空的
				for (m = 0; m < 8; m++) {
					if (((r) & (1 << m))) {
						fn_inx++;
						free_cn++;
						if ((fn_inx + 1) >= sp_sb->block_count) {
							bk_release(bk_tmp);
							return -1;
						}
						if (free_cn >= need_bk_num) {
							//找到需要的数量了
							bk_release(bk_tmp);
							goto next;
						}
					} else {
						free_cn = 0;
						fn_inx++;
						start_inx = fn_inx;
					}
				}
			} else {
				free_cn = 0;
				fn_inx += 8;
				start_inx = fn_inx;
			}
		}

		st_byte_inx = 0;
		bk_release(bk_tmp);
	}
	return -1;
	next:

	//从某块开始
	st_bk = start_inx / (sb->s_bk_size << 3);
	//从某个byte开始
	st_byte_inx = (start_inx % (sb->s_bk_size << 3)) >> 3;
	//从某个bit开始
	int32_t bit_inx = start_inx % 8;
	fn_inx = start_inx;
	free_cn = 0;
	uint8_t *p_t = 0;
	for (uint32_t i = st_bk; i < sp_sb->bk_used_bk_count; i++) {
		bk_tmp = bk_read(sb->s_dev_no, sp_sb->bk_used_bk_st_inx + i, 1, 0);
		for (j = st_byte_inx; j < sb->s_bk_size; j++) {
			p_t = &(bk_tmp->cache[j]);
			if (*p_t) {
				//有空的
				uint8_t tmp_st_inx = bit_inx;
				for (m = tmp_st_inx; m < 8; m++) {
					if (((*p_t) & (1 << m))) {
						*p_t &= ~(1 << ((fn_inx + free_cn) % 8));
						free_cn++;
						if (free_cn >= need_bk_num) {
							bk_release(bk_tmp);
							goto end;
						}
						bit_inx = 0;
					} else {
						fn_inx++;
					}
				}
			} else {
				fn_inx += 8;
			}
		}
		st_byte_inx = 0;
		bk_release(bk_tmp);
	}
	end: if (sp_sb->block_free) {
		sp_sb->block_free -= need_bk_num;
	}
	sp_sb->last_alloc_bk_inx = start_inx + need_bk_num;
	*res_sart_bk = start_inx;
	sb->s_dirt = 1;

	return 0;
}
/**
 * 从磁盘获得一个bk号码
 * @param sb
 * @param res_bk
 * @return
 */
int32_t sp_alloc_bk(struct super_block *sb, bk_no_t *res_bk) {
	struct sp_super_block *sp_sb = sb->s_sb_priv_info;
	uint32_t free_bk = 0;
	uint32_t fn_inx = 0;
	uint32_t j;
	uint8_t r;
	uint8_t m;
	struct bk_cache *bk_tmp;
	uint32_t st_bk;
	uint32_t st_byte_inx;
	st_bk = sp_sb->last_alloc_bk_inx / (sb->s_bk_size << 3);
	fn_inx += st_bk * (sb->s_bk_size << 3);
	st_byte_inx = (sp_sb->last_alloc_bk_inx % (sb->s_bk_size << 3)) >> 3;
	fn_inx += st_byte_inx << 3;
	for (uint32_t i = st_bk; i < sp_sb->bk_used_bk_count; i++) {
		bk_tmp = bk_read(sb->s_dev_no, sp_sb->bk_used_bk_st_inx + i, 1, 0);
		for (j = st_byte_inx; j < sb->s_bk_size; j++) {
			r = bk_tmp->cache[j];
			if (r) {
				//有空的
				for (m = 0; m < 8; m++) {
					if (((r) & (1 << m))) {
						//找到为1的空块
						free_bk = fn_inx;
						if ((free_bk + 1) >= sp_sb->block_count) {
							bk_release(bk_tmp);
							return -1;
						}
						goto next;
					} else {
						fn_inx++;
					}
				}
			} else {
				fn_inx += 8;
			}
		}
		st_byte_inx = 0;
		bk_release(bk_tmp);
	}
	return -1;
	next: sp_sb->last_alloc_bk_inx = free_bk;
	r &= ~(1 << (free_bk % 8));
	bk_tmp->cache[j] = r;

	if (sp_sb->block_free) {
		sp_sb->block_free--;
	}
	*res_bk = free_bk;
	sb->s_dirt = 1;
	bk_release(bk_tmp);
	DEBUG("fs",INFO,"alloc:last bk is %d.\n",sp_sb->last_alloc_bk_inx);
	return 0;
}
/**
 * 从磁盘释放一个bk no
 * @param sb
 * @param bk
 * @return
 */
int32_t sp_free_bk(struct super_block *sb, bk_no_t bk_no) {
	uint32_t bk_inx;
	struct sp_super_block *sp_sb = sb->s_sb_priv_info;
	bk_inx = ROUND_DOWN(bk_no, sb->s_bk_size * 8);

	struct bk_cache* tmp;

	tmp = bk_read(sb->s_dev_no, sp_sb->bk_used_bk_st_inx + bk_inx, 1, 0);
	if (!tmp) {
		return -1;
	}
	tmp->cache[ROUND_DOWN(bk_no, 8) % sb->s_bk_size] |= (1 << (bk_no % 8));
	bk_release(tmp);
	sp_sb->block_free++;
	if (bk_no<sp_sb->last_alloc_bk_inx) {
		sp_sb->last_alloc_bk_inx=bk_no;
		DEBUG("fs",INFO,"free:last bk is %d.\n",sp_sb->last_alloc_bk_inx);
	}
	sb->s_dirt = 1;
	return 0;
}
int32_t sp_get_free_inode(struct super_block *sb) {
	uint32_t i;
	uint32_t free_inode = 0;
	uint32_t fn_inx = 0;
	uint32_t j;
	uint8_t m;
	uint8_t r;
	struct sp_super_block *sp_sb;
	struct bk_cache *bk_tmp;
	sp_sb = sb->s_sb_priv_info;
	//先从bitmap中查找空闲的inode
	for (i = 0; i < sp_sb->inode_used_bk_count; i++) {
		bk_tmp = bk_read(sb->s_dev_no, sp_sb->inode_used_bk_st_inx + i, 1, 0);

		for (j = 0; j < sb->s_bk_size; j++) {
			r = bk_tmp->cache[j];
			if (r != 0) {
				//有空的
				for (m = 0; m < 8; m++) {
					fn_inx++;
					if (fn_inx >= sp_sb->inode_count) {
						bk_release(bk_tmp);
						return free_inode;
					}
					if (((r) & (1 << m))) {
						free_inode++;
					}
				}
			} else {
				fn_inx += 8;
			}
		}
		bk_release(bk_tmp);
		return free_inode;
	}
	return free_inode;
}
/**
 * 从磁盘申请一个inode号
 * @param sb
 * @param res_ino
 * @return
 */
int32_t sp_alloc_inode_no(struct super_block *sb, ino_t *res_ino) {
	uint32_t i;
	uint32_t bk_inx = 0;
	uint32_t free_inode = 0;
	uint32_t f_inx = 0;
	uint32_t j;
	uint8_t m;
	uint8_t r;
	struct sp_super_block *sp_sb;
	struct bk_cache *bk_tmp;
	sp_sb = sb->s_sb_priv_info;
//    if(sp_sb->inode_free==0) {
//        return -1;
//    }
	//先从bitmap中查找空闲的inode
	for (i = 0; i < sp_sb->inode_used_bk_count; i++) {
		bk_tmp = bk_read(sb->s_dev_no, sp_sb->inode_used_bk_st_inx + i, 1, 0);
//        SET_FREEZE(*bk_tmp);
		for (j = 0; j < sb->s_bk_size; j++) {
			r = bk_tmp->cache[j];
			if (r != 0) {
				//有空的
				for (m = 0; m < 8; m++) {
					if (((r) & (1 << m))) {
//                        f_inx += m;
						free_inode = f_inx;
						if ((free_inode + 1) >= sp_sb->inode_count) {
							bk_release(bk_tmp);
							return -1;
						}
						goto next;
					} else {
						f_inx++;
					}
				}
			} else {
				f_inx += 8;
			}
		}
		bk_release(bk_tmp);
	}
	return -1;
	next: r &= ~(1 << (free_inode % 8));
	bk_tmp->cache[j] = r;
	if (sp_sb->inode_free) {
		sp_sb->inode_free--;
	}
	*res_ino = free_inode;
	sb->s_dirt = 1;
	bk_release(bk_tmp);
	//DEBUG("fs",INFO,"分配新inode %d\r\n", free_inode);
	//kprint("alloc inode %d.\n", free_inode);
	return 0;
}
/**
 * 从磁盘释放一个inode号
 * @param sb
 * @param ino
 * @return
 */
int32_t sp_free_inode_no(struct super_block *sb, ino_t ino) {
	int32_t bk_inx;
	struct sp_super_block *sp_sb = sb->s_sb_priv_info;

	//先读Inode used block
	bk_inx = ROUND_DOWN(ino, sb->s_bk_size * 8);
	uint8_t rByte;
	if (rbk(sb->s_dev_no, sp_sb->inode_used_bk_st_inx + bk_inx, &rByte,
			ROUND_DOWN(ino, 8) % sb->s_bk_size, 1) < 0) {
		return -1;
	}
	rByte |= (1 << (ino % 8));
	//回写
	if (wbk(sb->s_dev_no, sp_sb->inode_used_bk_st_inx + bk_inx, &rByte,
			ROUND_DOWN(ino, 8) % sb->s_bk_size, 1) < 0) {
		return -1;
	}
	sp_sb->inode_free++;
	sb->s_dirt = 1;
	//kprint("free inode %d.\n", ino);
	return 0;
}
/**
 * 检查磁盘上的inode是否存在
 * @param sb
 * @param ino
 * @return
 */
int32_t sp_check_inode_no(struct super_block *sb, ino_t ino) {
	uint8_t rByte;
	struct sp_super_block *sp_sb = sb->s_sb_priv_info;
	int32_t bk_inx;

	bk_inx = ROUND_DOWN(ino, sb->s_bk_size * 8);
	if (rbk(sb->s_dev_no, sp_sb->inode_used_bk_st_inx + bk_inx, &rByte,
			ROUND_DOWN(ino, 8) % sb->s_bk_size, 1) < 0) {
		return -1;
	}

	if ((rByte & (1 << (ino % 8)))) {
		//Inode没有被使用
		return 0;
	}
	return 1;
}

