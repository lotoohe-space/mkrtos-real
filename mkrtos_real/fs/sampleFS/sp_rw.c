//
// Created by Administrator on 2022/1/18.
//
#include <type.h>
#include <mkrtos/fs.h>
#include <mkrtos/sp.h>
#include <mkrtos/bk.h>
#include <fcntl.h>
#include <string.h>
#include <mkrtos/stat.h>
#include "mkrtos/debug.h"
#include <knl_service.h>
#include <assert.h>

int sp_file_st_read(struct inode *inode, struct file *fp, char *buf, int count) {
	struct super_block *sb;
	struct sp_super_block *sp_sb;
	struct sp_inode *sp_inode;
	struct bk_cache* tmp_bk;
	int w_ofs;
	int w_ofs_bk;
	int dev_no;
	int rw_size = 0;

	int rw_len = 0;
	int cur_bk_rm_size = 0;
	int cur_w_ofs = 0;

	int max_write_size;
	if (!S_ISREG(inode->i_type_mode)) {
		return -EISDIR;
	}
	if (!FILE_GET_ST_MODE(inode->i_type_mode)) {
		return -1;
	}
	if (count == 0) {
		return 0;
	}
	if (fp->f_flags & O_APPEND) {
		return -ENOSYS;
	}

	sb = inode->i_sb;
	//bk设备的设备号
	sp_inode = inode->i_fs_priv_info;
	dev_no =sb->s_dev_no;
	sp_sb = sb->s_sb_priv_info;
	w_ofs = fp->f_ofs;
	max_write_size=sp_inode->_l.bk_num*sb->s_bk_size; // 最大可以读入的文件大小
	//inode->i_file_size += count;
	while (rw_len < count) {
		//w_ofs += rw_len;        //0
		//获取开始的块偏移
		w_ofs_bk = (w_ofs+rw_len) / sb->s_bk_size + sp_inode->_l.start_bk;        //0
		//当前写入块剩余的大小
		cur_bk_rm_size = sb->s_bk_size - ( (w_ofs+rw_len) % sb->s_bk_size);        //2048
		tmp_bk = bk_read(dev_no, w_ofs_bk, 0, 0);
		if (!tmp_bk) {
			inode->i_file_size -= count-rw_len;
			//运行到这里说明当前的块设备没有存储空间了
			goto end;
		}
		//获得需要写入的大小
		rw_size = MIN((count - rw_len), sb->s_bk_size);
		rw_size = MIN(rw_size, cur_bk_rm_size);
		rw_size = MIN(rw_size, inode->i_file_size-(w_ofs+rw_len));
		if(rw_size==0){
			bk_release(tmp_bk);
			break;
		}
		//计算当前需读入的偏移
		cur_w_ofs =  (w_ofs+rw_len) % sb->s_bk_size;
		mkrtos_memcpy(buf + rw_len, tmp_bk->cache + cur_w_ofs , rw_size);
		bk_release(tmp_bk);

		//写入了数据
		rw_len += rw_size;

//		if (inode->i_file_size + rw_len >= max_write_size) {
//			inode->i_file_size -= count-rw_len;
//			goto end;
//		}
	}
	end:
	fp->f_ofs+=rw_len;
	return rw_len;
}
int sp_file_read(struct inode *inode, struct file *filp, char *buf, int count) {
	uint32_t fileUsedBKNum;
	uint32_t rLen = 0;
	int32_t r_inx = 0;
	struct super_block *sb;
	struct sp_inode *sp_ino;
	//读这一块时最大的大小
	uint32_t bkRemainSize;
	//偏移在块中开始的位置
	uint32_t bInx;
	//偏移开始的位置
	uint32_t ofstBkInx;
	//查看文件是否在连续模式
	if (FILE_GET_ST_MODE(inode->i_type_mode) && S_ISREG(inode->i_type_mode)) {
		int ret;
		ret = sp_file_st_read(inode,filp,buf,count);
		return ret;
	}
	if (count == 0) {
		return 0;
	}
	r_inx = filp->f_ofs;
	if (r_inx >= inode->i_file_size) {
		return -1;
	}
	sb = inode->i_sb;
	sp_ino = (struct sp_inode*) (inode->i_fs_priv_info);

	ofstBkInx = ROUND_DOWN(r_inx, sb->s_bk_size);
	bInx = r_inx % sb->s_bk_size;
	bkRemainSize = sb->s_bk_size - bInx;
	fileUsedBKNum = FILE_USED_BK_NUM(sb, inode);

	for (uint32_t i = ofstBkInx;
			i < fileUsedBKNum && rLen < count
					&& (rLen + r_inx) < inode->i_file_size; i++) {
		uint32_t remainSize = count - rLen;
		uint32_t rSize = 0;
		uint32_t readFileRamainSize;
		readFileRamainSize = inode->i_file_size - (rLen + r_inx);
		rSize = MIN(remainSize, sb->s_bk_size);
		rSize = MIN(rSize, readFileRamainSize);
		if (i == ofstBkInx) {
			rSize = MIN(rSize, bkRemainSize);
		} else {
			bInx = 0;
			bkRemainSize = sb->s_bk_size;
		}
		if (i < A_BK_NUM(sp_ino)) {
			struct bk_cache *tmp;
			tmp = bk_read(sb->s_dev_no, sp_ino->p_ino[i], 0, 0);
			mkrtos_memcpy(buf + rLen, tmp->cache + bInx, rSize);
			rLen += rSize;
			filp->f_ofs += rSize;
			bk_release(tmp);
		} else if (i < A_BK_NUM(sp_ino) + B_BK_NUM(sb, sp_ino)) {

			uint32_t overNum = i - A_BK_NUM(sp_ino);
			uint32_t bkNo = overNum / BK_INC_X_NUM(sb);
			uint32_t bkInx = overNum % BK_INC_X_NUM(sb);
			uint32_t readBkInx;
			struct bk_cache *tmp;
			struct bk_cache *tmp1;

			tmp = bk_read(sb->s_dev_no, sp_ino->pp_ino[bkNo], 0, 0);
			mkrtos_memcpy((uint8_t*) (&readBkInx),
					tmp->cache + bkInx * sizeof(uint32_t), sizeof(uint32_t));
			bk_release(tmp);

			tmp1 = bk_read(sb->s_dev_no, readBkInx, 0, 0);
			mkrtos_memcpy(buf + rLen, tmp1->cache + bInx, rSize);
			rLen += rSize;
			filp->f_ofs += rSize;
			bk_release(tmp1);

		} else if (i
				< A_BK_NUM(sp_ino) + B_BK_NUM(sb, sp_ino) + C_BK_NUM(sb, sp_ino)) {
			struct bk_cache *tmp;
			int a, b;
			//三级大小
			uint32_t overANum = i - A_BK_NUM(sp_ino) - B_BK_NUM(sb, sp_ino);
			//在初级块中的偏移
			uint32_t pFileBksInx = overANum
					/ (BK_INC_X_NUM(sb) * BK_INC_X_NUM(sb));
			//计算二级偏移，等于超出部分大小，除以
			uint32_t ppFileBksInx = (overANum
					% (BK_INC_X_NUM(sb) * BK_INC_X_NUM(sb))) / BK_INC_X_NUM(sb);
			uint32_t pppFileBksInx = overANum % BK_INC_X_NUM(sb);

			tmp = bk_read(sb->s_dev_no, sp_ino->ppp_ino[pFileBksInx], 0, 0);
			mkrtos_memcpy((uint8_t*) (&a), tmp->cache + (ppFileBksInx << 2),
					sizeof(uint32_t));
			bk_release(tmp);

			tmp = bk_read(sb->s_dev_no, a, 0, 0);
			mkrtos_memcpy(&b, tmp->cache + (pppFileBksInx << 2),
					sizeof(uint32_t));
			bk_release(tmp);

			tmp = bk_read(sb->s_dev_no, b, 0, 0);
			mkrtos_memcpy(buf + rLen, tmp->cache + bInx, rSize);
			rLen += rSize;
			filp->f_ofs += rSize;
			bk_release(tmp);
		} else {
			return -1;
		}
	}

	return rLen;
}

/**
 * 获取指定偏移所在的块号
 * @param inode
 * @param offset
 * @param fpBkNum
 * @return
 */
int32_t get_ofs_bk_no(struct inode *inode, uint32_t offset, uint32_t *fpBkNum) {
	struct super_block *sb;
	struct sp_inode *sp_ino;
	if (inode == NULL) {
		return -1;
	}

	sb = inode->i_sb;
	sp_ino = (struct sp_inode*) (inode->i_fs_priv_info);
	if (offset > inode->i_file_size) {
		return -1;
	}
	uint32_t usedBkNum = ROUND_UP(offset, sb->s_bk_size) - 1;
	if (usedBkNum < A_BK_NUM(sp_ino)) {
		*fpBkNum = sp_ino->p_ino[usedBkNum];
		return 0;
	} else if (usedBkNum < A_BK_NUM(sp_ino) + B_BK_NUM(sb, sp_ino)) {
		//超过A部分的大小
		uint32_t overANum = usedBkNum - A_BK_NUM(sp_ino);
		//二级大小
		uint32_t pFileBksInx = overANum / BK_INC_X_NUM(sb);
		uint32_t pFileBksi = overANum % BK_INC_X_NUM(sb);
		uint32_t bkNum;

		struct bk_cache *tmp;
		tmp = bk_read(sb->s_dev_no, sp_ino->pp_ino[pFileBksInx], 0, 0);
		mkrtos_memcpy((uint8_t*) (&bkNum),
				tmp->cache + pFileBksi * sizeof(uint32_t), sizeof(uint32_t));
		bk_release(tmp);
		*fpBkNum = bkNum;
		return 0;
	} else if (usedBkNum
			< A_BK_NUM(sp_ino) + B_BK_NUM(sb, sp_ino) + C_BK_NUM(sb, sp_ino)) {
		struct bk_cache *tmp;
		//三级大小
		uint32_t overANum = usedBkNum - A_BK_NUM(sp_ino) - B_BK_NUM(sb, sp_ino);
		//在初级块中的偏移
		uint32_t pFileBksInx = overANum / (BK_INC_X_NUM(sb) * BK_INC_X_NUM(sb));
		//计算二级偏移，等于超出部分大小，除以
		uint32_t ppFileBksInx = (overANum
				% (BK_INC_X_NUM(sb) * BK_INC_X_NUM(sb))) / BK_INC_X_NUM(sb);
		uint32_t pppFileBksInx = overANum % BK_INC_X_NUM(sb);

		if (!sp_ino->ppp_ino[pFileBksInx]) {
			return -ENOSPC;
		}
		int a;
		tmp = bk_read(sb->s_dev_no, sp_ino->ppp_ino[pFileBksInx], 0, 0);
		mkrtos_memset(&a, tmp->cache + (ppFileBksInx << 2), sizeof(a));
		bk_release(tmp);
		if (!a) {
			return -ENOSPC;
		}
		int b;
		tmp = bk_read(sb->s_dev_no, a, 0, 0);
		mkrtos_memset(&b, tmp->cache + (pppFileBksInx << 2), sizeof(b));
		bk_release(tmp);
		if (!b) {
			return -ENOSPC;
		}
		*fpBkNum = b;
		return 0;
	} else {
		return -1;
	}
}
/**
 * 找到连续可用的block
 */
int32_t inode_alloc_new_bk_ct(struct inode *inode, uint32_t ct_size) {
	struct super_block *sb;
	struct sp_inode *sp_ino;

	uint32_t mode;

	uint32_t need_bk_num;

	sb = inode->i_sb;
	sp_ino = (struct sp_inode*) (inode->i_fs_priv_info);


	mode = FILE_MODE( inode->i_type_mode);
	need_bk_num = ROUND_UP(ct_size,sb->s_bk_size);



	return 0;
}
int32_t inode_alloc_new_bk(struct inode *inode, uint32_t *new_bk_num) {
	struct super_block *sb;
	struct sp_inode *sp_ino;
	sb = inode->i_sb;
	sp_ino = (struct sp_inode*) (inode->i_fs_priv_info);
	uint32_t used_new_bk_num;
	if (inode->i_file_size == 0) {
		used_new_bk_num = 0;
	} else {
		used_new_bk_num = FILE_USED_BK_NUM(sb, inode);
	}

	if (used_new_bk_num < A_BK_NUM(sp_ino)) {

		if (sp_alloc_bk(sb, new_bk_num) < 0) {
			return -1;
		}
		sp_ino->p_ino[used_new_bk_num] = *new_bk_num;
		inode->i_dirt = 1;

	} else if (used_new_bk_num < A_BK_NUM(sp_ino) + B_BK_NUM(sb, sp_ino)) {
		//超过A部分的大小
		uint32_t overANum = used_new_bk_num - A_BK_NUM(sp_ino);
		//二级大小
		uint32_t pFileBksInx = overANum / BK_INC_X_NUM(sb);
		uint32_t pFileBksi = overANum % BK_INC_X_NUM(sb);
		struct bk_cache *tmp;

		uint32_t a;
		if (sp_ino->pp_ino[pFileBksInx] == 0) {
			if (sp_alloc_bk(sb, &a) < 0) {
				return -1;
			}
			//全部设置为0
			tmp = bk_read(sb->s_dev_no, a, 1, 1);
			mkrtos_memset(tmp->cache, 0, sb->s_bk_size);
			bk_release(tmp);
			sp_ino->pp_ino[pFileBksInx] = a;
			inode->i_dirt = 1;
		}
		uint32_t b;
		if (sp_alloc_bk(sb, &b) < 0) {
			return -1;
		}
		tmp = bk_read(sb->s_dev_no, sp_ino->pp_ino[pFileBksInx], 1, 0);
		mkrtos_memcpy(tmp->cache + pFileBksi * 4, (uint8_t*) &b, sizeof(b));
		bk_release(tmp);
		*new_bk_num = b;
	} else if (used_new_bk_num
			< A_BK_NUM(sp_ino) + B_BK_NUM(sb, sp_ino) + C_BK_NUM(sb, sp_ino)) {
		//三级大小
		uint32_t overANum = used_new_bk_num
				- A_BK_NUM(sp_ino)-B_BK_NUM(sb, sp_ino);
		//在初级块中的偏移
		uint32_t pFileBksInx = overANum / (BK_INC_X_NUM(sb) * BK_INC_X_NUM(sb));
		//计算二级偏移，等于超出部分大小，除以
		uint32_t ppFileBksInx = (overANum
				% (BK_INC_X_NUM(sb) * BK_INC_X_NUM(sb))) / BK_INC_X_NUM(sb);
		uint32_t pppFileBksInx = overANum % BK_INC_X_NUM(sb);
		uint32_t a;
		struct bk_cache *tmp;
		if (!sp_ino->ppp_ino[pFileBksInx]) {
			if (sp_alloc_bk(sb, &a) < 0) {
				return -1;
			}
			//全部设置为0
			tmp = bk_read(sb->s_dev_no, a, 1, 1);
			mkrtos_memset(tmp->cache, 0, sb->s_bk_size);
			bk_release(tmp);
			sp_ino->ppp_ino[pFileBksInx] = a;
			inode->i_dirt = 1;
		}
		//得到二级的值
		uint32_t b;
		tmp = bk_read(sb->s_dev_no, sp_ino->ppp_ino[pFileBksInx], 1, 0);
		mkrtos_memcpy(&b, tmp->cache + ppFileBksInx * 4, sizeof(b));
		if (!b) {
			if (sp_alloc_bk(sb, &b) < 0) {
				sp_ino->ppp_ino[pFileBksInx] = 0;
				sp_free_bk(sb, a);
				bk_release(tmp);
				return -1;
			}
			mkrtos_memcpy(tmp->cache + ppFileBksInx * 4, &b, sizeof(b));
			bk_release(tmp);
			//全部设置为0
			tmp = bk_read(sb->s_dev_no, b, 1, 1);
			mkrtos_memset(tmp->cache, 0, sb->s_bk_size);
			inode->i_dirt = 1;
		} else {
			bk_release(tmp);
			tmp = bk_read(sb->s_dev_no, b, 1, 0);
		}
		uint32_t c;
		if (sp_alloc_bk(sb, &c) < 0) {
			sp_ino->ppp_ino[pFileBksInx] = 0;

			sp_free_bk(sb, a);
			sp_free_bk(sb, b);
			return -1;
		}
		mkrtos_memcpy(tmp->cache + pppFileBksInx * 4, (uint8_t*) &c, sizeof(c));
		bk_release(tmp);
		*new_bk_num = c;
		return 0;
	} else {
		return -1;
	}

	return 0;
}
int sp_file_st_write(struct inode *inode, struct file *fp, char * buf, int count) {
	struct super_block *sb;
	struct sp_super_block *sp_sb;
	struct sp_inode *sp_inode;
	struct bk_cache* tmp_bk;
	int w_ofs;
	int w_ofs_bk;
	int dev_no;
	int w_size = 0;

	int w_len = 0;
	int cur_bk_rm_size = 0;
	int cur_w_ofs = 0;

	int max_write_size;
	if (!S_ISREG(inode->i_type_mode)) {
		return -EISDIR;
	}
	if (!FILE_GET_ST_MODE(inode->i_type_mode)) {
		return -1;
	}
	if (count == 0) {
		return 0;
	}
	if (fp->f_flags & O_APPEND) {
		return -ENOSYS;
	}

	sb = inode->i_sb;
	//bk设备的设备号
	sp_inode = inode->i_fs_priv_info;
	dev_no =sb->s_dev_no;
	sp_sb = sb->s_sb_priv_info;
	w_ofs = fp->f_ofs;
	max_write_size=sp_inode->_l.bk_num*sb->s_bk_size; // 最大可以写入的文件大小

	if (max_write_size < inode->i_file_size + count) {
		return -1;
	}

	inode->i_file_size += count;

	while (w_len < count) {
		//获取开始的块偏移
		w_ofs_bk = (w_ofs+w_len) / sb->s_bk_size + sp_inode->_l.start_bk;        //0
		//当前写入块剩余的大小
		cur_bk_rm_size = sb->s_bk_size - ((w_ofs+w_len) % sb->s_bk_size);        //2048
		tmp_bk = bk_read(dev_no, w_ofs_bk, 1, 0);
		if (!tmp_bk) {
			inode->i_file_size -= count;
			//运行到这里说明当前的块设备没有存储空间了
			goto end;
		}
		//获得需要写入的大小
		w_size = MIN((count - w_len), sb->s_bk_size);
		w_size = MIN(w_size, cur_bk_rm_size);
		w_size = MIN(w_size, max_write_size-(w_ofs+w_size));
		if(w_size==0){
			bk_release(tmp_bk);
			break;
		}
		//计算当前需要写入的偏移
		cur_w_ofs = (w_ofs+w_len) % sb->s_bk_size;
		MKRTOS_ASSERT(w_size + cur_w_ofs <= sb->s_bk_size);
		mkrtos_memcpy(tmp_bk->cache + cur_w_ofs, buf + w_len, w_size);
		bk_release(tmp_bk);

		//写入了数据
		w_len += w_size;
//
//		if (inode->i_file_size + w_len >= max_write_size) {
//			inode->i_file_size -= count-w_len;
//			goto end;
//		}
	}
	if(w_len!=count){
		inode->i_file_size -= count - w_len;
	}
	end:
	inode->i_dirt = 1;
	fp->f_ofs += w_len;
	return w_len;
}
int sp_file_write(struct inode *inode, struct file *filp, char *buf, int count) {
	uint32_t usedBkNum;
	uint32_t upSize;
	uint32_t w_size = 0;
	uint8_t flag = 0;
	uint32_t wLen = 0;
	//写入的偏移位置
	uint32_t w_offset = 0;
	struct super_block *sb;
	struct bk_cache *tmp = NULL;

	//查看文件是否在连续模式
	if (FILE_GET_ST_MODE(inode->i_type_mode)) {
		int ret;
		ret = sp_file_st_write(inode,filp,buf,count);
		return ret;
	}

	if (!S_ISREG(inode->i_type_mode)) {
		return -EISDIR;
	}

	if (count == 0) {
		return 0;
	}
	if (filp->f_flags & O_APPEND) {
		filp->f_ofs = inode->i_file_size;
	}
	w_offset = filp->f_ofs;
	//写入的偏移超过了文件大小
	if (w_offset > inode->i_file_size) {
		return -1;
	}

	sb = inode->i_sb;
	//文件占用多少块
	usedBkNum = ROUND_UP(w_offset, sb->s_bk_size);

	//使用块总共的内存
	upSize = usedBkNum * sb->s_bk_size;
	//写入数据
	while (wLen < count) {

		if (flag == 0) { //先写最后一块
			if (upSize > w_offset && upSize != w_offset) { //不能相等，相等说明刚好写到了一块
			//写入最后一块剩余的空间
				uint32_t last_bk_no;
				if (get_ofs_bk_no(inode, w_offset, &last_bk_no) < 0) {
					return -1;
				}
				w_size = count > (sb->s_bk_size - (w_offset % sb->s_bk_size)) ?
						(sb->s_bk_size - (w_offset % sb->s_bk_size)) : count;

				tmp = bk_read(sb->s_dev_no, last_bk_no, 1, 0);
				mkrtos_memcpy(tmp->cache + w_offset % sb->s_bk_size, buf + wLen,
						w_size);

				wLen += w_size;
				w_offset += w_size;

				if (w_offset > inode->i_file_size) {
					//更新文件大小
					inode->i_file_size = w_offset;
					filp->f_ofs = w_offset;
					inode->i_dirt = 1;
				}
				bk_release(tmp);
			}
			flag = 1;
		} else {
			//剩余的大小
			uint32_t remainSize;
			//分配的新的块
			uint32_t need_w_bk;
			if (w_offset >= inode->i_file_size) {
				//如果写入偏移大于或者等于文件大小，申请一块新的
				if (inode_alloc_new_bk(inode, &need_w_bk) < 0) {
					return -ENOSPC;
				}
			} else {
				//否则获取当前偏移的块号
				if (get_ofs_bk_no(inode, w_offset, &need_w_bk) < 0) {
					return -1;
				}
			}
			remainSize = count - wLen;
			//计算还需要写入多少
			w_size = remainSize > sb->s_bk_size ? sb->s_bk_size : remainSize;
			tmp = bk_read(sb->s_dev_no, need_w_bk, 1, w_size == sb->s_bk_size);
			mkrtos_memcpy(tmp->cache, buf + wLen, w_size);
			wLen += w_size;
			w_offset += w_size;
			if (w_offset > inode->i_file_size) {
				//更新文件大小
				inode->i_file_size = w_offset;
				filp->f_ofs = w_offset;
				inode->i_dirt = 1;
			}
			bk_release(tmp);
		}
	}
//    DEBUG("fs",INFO,"剩余块数:%d.",get_free_bk(sb));
//    DEBUG("fs",INFO,"剩余inode:%d.",get_free_inode(sb));
	DEBUG("fs",INFO,"file size %d\r\n", inode->i_file_size);
	return wLen;
}
