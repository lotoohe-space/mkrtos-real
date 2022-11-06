#include <mkrtos/fs.h>
#include <mkrtos/bk.h>
#include <mkrtos/sp.h>
#include <knl_service.h>
#include <mkrtos/sp.h>
#include <string.h>
#include "mkrtos/debug.h"

struct inode* sp_alloc_inode(struct inode *p_inode) {
	struct sp_inode *sp_ino;
	p_inode->i_fs_priv_info = malloc(sizeof(struct sp_inode));
	if (p_inode->i_fs_priv_info == NULL) {
		return NULL;
	}
	sp_ino = p_inode->i_fs_priv_info;
	p_inode->i_ops = &sp_dir_inode_operations;
	mkrtos_memset(sp_ino, 0, sizeof(struct sp_inode));

	return p_inode;
}

int32_t sp_free_inode(struct inode *p_inode) {
//    if(p_inode->i_hlink){
//        return -1;
//    }
	if (atomic_read(&p_inode->i_used_count)) {
		return -1;
	}
	free(p_inode->i_fs_priv_info);
	return 0;
}

/**
 * 申请一个心得inode
 * @param p_inode
 * @return
 */
struct inode* sp_new_inode(struct inode *p_inode) {
	ino_t res_ino;
	struct inode *new_inode;

	if (sp_alloc_inode_no(p_inode->i_sb, &res_ino) < 0) {
		return NULL;
	}
	new_inode = get_empty_inode();
	new_inode->i_no = res_ino;
	new_inode->i_hlink = 1;
	new_inode->i_file_size = 0;
	new_inode->i_sb = p_inode->i_sb;
	new_inode->i_dirt = 1;
	new_inode->i_ops = NULL;
	atomic_set(&(new_inode->i_used_count), 1);
	atomic_set(&(new_inode->i_lock), 0);
	if (sp_alloc_inode((new_inode)) == NULL) {
		sp_free_inode_no(p_inode->i_sb, res_ino);
		puti(new_inode);
		return NULL;
	}
	return new_inode;
}

/**
 * 读取一个inode
 * @param inode
 * @return
 */
int32_t sp_read_inode(struct inode *inode) {
	uint32_t ino;
	struct super_block *sb = inode->i_sb;
	struct sp_super_block *sp_sb = sb->s_sb_priv_info;
	//需要读取的块号
	uint32_t ndReadBkInx;
	//inode在块内偏移
	uint32_t ndReadBkInInx;
	ino = inode->i_no;
	//检查Inode是否被使用
	if (sp_check_inode_no(sb, ino) != 1) {
		return -1;
	}
	uint8_t *w_ch;
	struct bk_cache *tmp;

	ndReadBkInx = (ino * sb->s_inode_size) / sb->s_bk_size
			+ sp_sb->inode_data_bk_st_inx;
	ndReadBkInInx = (ino) % (sb->s_bk_size / sb->s_inode_size);
	tmp = bk_read(sb->s_dev_no, ndReadBkInx, 0, 0);
	if (!tmp) {
		return -1;
	}
	mkrtos_memcpy(inode->i_fs_priv_info,
			(uint32_t)tmp->cache + ndReadBkInInx * sb->s_inode_size
			, sizeof(struct sp_inode));


	struct inode *temp = (struct inode*) (tmp->cache + ndReadBkInInx * sb->s_inode_size
			+ sizeof(struct sp_inode));

	//文件类型与权限
	inode->i_type_mode = temp->i_type_mode;
	//文件大小
	inode->i_file_size = temp->i_file_size;
	//自己的Inode号码
	inode->i_no = temp->i_no;
	//硬连接数
	inode->i_hlink = temp->i_hlink;
	inode->i_rdev_no = temp->i_rdev_no;
	inode->i_buser_no = temp->i_buser_no;
	switch (FILE_TYPE(inode->i_type_mode)) {
	case 0:
		//普通文件
		inode->i_ops = &sp_file_inode_operations;
		break;
	case 1:
		//目录文件
		inode->i_ops = &sp_dir_inode_operations;
		break;
	case 2: //字符设备
		inode->i_ops = &chrdev_inode_operations;
		break;
	case 3: //块设备
		inode->i_ops = &blkdev_inode_operations;
		break;
	}
	bk_release(tmp);
	//是否被修改过
	inode->i_dirt = 0;
	//kprint("read inode %d, size %d.\n",inode->i_no,inode->i_file_size);
	return 0;
}


/**
 * 吧inode写回磁盘
 * @param i_node
 * @return none
 */
void sp_write_inode(struct inode *i_node) {
	struct super_block *sb = i_node->i_sb;
	struct sp_super_block *sp_sb = sb->s_sb_priv_info;
	uint8_t *w_ch;
	//写Inode
	uint32_t bk_inx;

	if (!i_node->i_dirt) {
		return;
	}

	bk_inx = ROUND_DOWN(i_node->i_no, INODE_NUM_IN_BK(sb));

	struct bk_cache* tmp;
	tmp = bk_read(sb->s_dev_no, sp_sb->inode_data_bk_st_inx + bk_inx
			,1 ,0);
	if (!tmp) {
		return ;
	}
	w_ch = (uint8_t *)tmp->cache + ((i_node->i_no % INODE_NUM_IN_BK(sb)) *sb->s_inode_size);
	mkrtos_memcpy(w_ch, i_node->i_fs_priv_info, sizeof(struct sp_inode));

	struct inode *temp = (struct inode*) (w_ch + sizeof(struct sp_inode));
	//文件类型与权限
	temp->i_type_mode = i_node->i_type_mode;
	//文件大小
	temp->i_file_size = i_node->i_file_size;
	//自己的Inode号码
	temp->i_no = i_node->i_no;
	//硬连接数
	temp->i_hlink = i_node->i_hlink;
	//设备号
	temp->i_rdev_no = i_node->i_rdev_no;
	temp->i_buser_no = i_node->i_buser_no;
	i_node->i_dirt = 0;
	bk_release(tmp);

	//kprint("write inode %d, size %d.\n",i_node->i_no,i_node->i_file_size);
}
//删除这个inode，释放其占用的空间，
void sp_put_inode(struct inode *i_node) {
	if (i_node->i_hlink > 0) {
		return;
	}
	if(FILE_GET_ST_MODE(i_node->i_type_mode)) {
		struct sp_inode *sp_ino;
		sp_ino = i_node->i_fs_priv_info;
		sp_free_ct_bk(i_node->i_sb,sp_ino->_l.start_bk,sp_ino->_l.bk_num);
	}else{
		//刪除文件占用的bk
		sp_truncate(i_node, 0);
	}
	//释放占用inode
	sp_free_inode_no(i_node->i_sb, i_node->i_no);
	DEBUG("fs", INFO, "remain blocks:%d.", sp_get_free_bk(i_node->i_sb));
	DEBUG("fs", INFO, "remain inode:%d.", sp_get_free_inode(i_node->i_sb));
}
//释放超级块
void sp_put_super(struct super_block *sb) {
	//
}
/**
 * 读取指定设备的sb
 * @param dev_no
 * @return
 */
struct super_block* sp_read_sb(struct super_block *sb)
{
	struct super_block *sb_temp;
	struct bk_cache* tmp;

	tmp = bk_read(sb->s_dev_no, 1, 0, 0);
	if (!tmp) {
		return NULL;
	}
	sb_temp = (struct super_block *)tmp->cache;
	//检
	if (sb_temp->s_magic_num != SP_MAGIC_NUM) {
		bk_release(tmp);
		tmp = bk_read(sb->s_dev_no, 0, 0, 0);
		if (sb_temp->s_magic_num != SP_MAGIC_NUM) {
			bk_release(tmp);
			return NULL;
		}
	}

	//魔数
	sb->s_magic_num = sb_temp->s_magic_num;
	//块大小
	sb->s_bk_size = sb_temp->s_bk_size;
	//块设备的设备号
	sb->s_dev_no = sb_temp->s_dev_no;
	//inode写入到磁盘的大小
	sb->s_inode_size = sb_temp->s_inode_size;
	//super操作函数，这里暂时留空，应该设置
	sb->s_ops = &sp_s_ops;
	//是否修改过
	sb->s_dirt = 0;
	//文件系统拥有的私有信息
	mkrtos_memcpy(sb->s_sb_priv_info,
			(uint8_t*) (tmp->cache) + sizeof(struct super_block),
			sizeof(struct sp_super_block));

	sb->root_inode = geti(sb, 0);
	sb->root_inode->i_ops = &sp_dir_inode_operations;
	bk_release(tmp);
	return sb;
}
void sp_write_super(struct super_block *sb) {
	struct bk_cache *bk_tmp;

	if (!sb->s_dirt) {
		return;
	}
	//在这里要吧前两个块都写了，有一个是备份
	bk_tmp = bk_read(sb->s_dev_no, 1, 1, 1);
	mkrtos_memcpy(bk_tmp->cache, sb, sizeof(struct super_block));
	mkrtos_memcpy(bk_tmp->cache + sizeof(struct super_block),
			sb->s_sb_priv_info, sizeof(struct sp_super_block));
	bk_release(bk_tmp);

	bk_tmp = bk_read(sb->s_dev_no, 0, 1, 1);
	mkrtos_memcpy(bk_tmp->cache, sb, sizeof(struct super_block));
	mkrtos_memcpy(bk_tmp->cache + sizeof(struct super_block),
			sb->s_sb_priv_info, sizeof(struct sp_super_block));
	bk_release(bk_tmp);
	sb->s_dirt = 0;
}
int sp_sync_inode(struct inode *inode) {
	lock_inode(inode);
	sp_write_inode(inode);
	unlock_inode(inode);
	return 0;
}
int sp_notify_change(int flags, struct inode *i_node) {

	return -ENOSYS;
}
void sp_statfs(struct super_block *sb, struct statfs *s_fs) {

}
int sp_remount_fs(struct super_block *sb, int *a, char *b) {

	return -ENOSYS;
}
/**
 * 获取inode文件最后位置所在的bk号码
 * @param p_inode
 * @param offset
 * @param bk_no
 * @return
 */
int32_t get_bk_no_ofs(struct inode *p_inode, uint32_t offset, uint32_t *bk_no) {
	struct super_block *sb;
	struct sp_inode *sp_inode;
	uint32_t used_bk_num;
	if (offset > p_inode->i_file_size) {
		return -1;
	}
	sb = p_inode->i_sb;
	sp_inode = SP_INODE(p_inode);
	used_bk_num = ROUND_UP(offset + 1, sb->s_bk_size);
	if (used_bk_num <= A_BK_NUM(sp_inode)) {
		*bk_no = sp_inode->p_ino[used_bk_num - 1];
		return 0;
	} else if (used_bk_num <= A_BK_NUM(sp_inode) + B_BK_NUM(sb, sp_inode)) {
		//超过A部分的大小
		uint32_t over_size = used_bk_num - A_BK_NUM(sp_inode) - 1;
		//二级大小
		uint32_t pp_bk_no = over_size / BK_INC_X_NUM(sb);
		uint32_t pp_bk_ofs = over_size % BK_INC_X_NUM(sb);
		uint32_t bk_num;
		//二级大小
		if (rbk(sb->s_dev_no, sp_inode->pp_ino[pp_bk_no], (uint8_t*) (&bk_num),
				pp_bk_ofs * sizeof(bk_no_t), sizeof(bk_no_t))) {
			return -1;
		}
		*bk_no = bk_num;
		return 0;
	}
//    else if (usedBkNum <= A_BK_NUM(pINode) + B_BK_NUM(pFsInfo, pINode) + C_BK_NUM(pFsInfo, pINode)) {
//        //三级部分的大小
//        uint32_t overANum = usedBkNum - A_BK_NUM(pINode)-B_BK_NUM(pFsInfo, pINode)-1;
//        //得到一级偏移
//        uint32_t pFileBksInx = overANum/BK_INC_X_NUM(pFsInfo)*BK_INC_X_NUM(pFsInfo);
//        //计算二级偏移，等于超出部分大小，除以
//        uint32_t ppFileBksInx = (overANum%(BK_INC_X_NUM(pFsInfo)*BK_INC_X_NUM(pFsInfo)))/BK_INC_X_NUM(pFsInfo);
//        uint32_t pppFileBksInx= (overANum%(BK_INC_X_NUM(pFsInfo)*BK_INC_X_NUM(pFsInfo)))%BK_INC_X_NUM(pFsInfo);
//        uint32_t bkNum;
//        //获得二级块偏移
//        if (BkCacheAddOp(pFsInfo->bkDev, 3, pINode->ppFileBks[pFileBksInx],
//                         (uint8_t*)(&bkNum), ppFileBksInx*sizeof(uint32_t), sizeof(uint32_t))) {
//            return FsNotFindErr;
//        }
//        //获得最终偏移
//        if (BkCacheAddOp(pFsInfo->bkDev, 3, bkNum,  (uint8_t*)(&bkNum), pppFileBksInx*sizeof(uint32_t), sizeof(uint32_t))) {
//            return FsNotFindErr;
//        }
//        *fpBkNum = bkNum;
//        return FsNoneErr;
//    }
	else {
		//超过了单个文件的大小
//        printk("超过了单个文件大小\r\n");
		return -1;
	}
	return -1;
}
struct super_operations sp_s_ops = {
		//申请指定文件系统的inode占用的内存
		.alloc_inode = sp_alloc_inode,
		//释放指定文件系统的inode占用的内存
		.free_inode = sp_free_inode,
		.read_inode = sp_read_inode,
		.notify_change = sp_notify_change,
		.write_inode = sp_write_inode,
		.put_inode = sp_put_inode,
		.put_super = NULL,
		.write_super = sp_write_super,
		.statfs = sp_statfs,
		.remount_fs = sp_remount_fs
};
