//
// Created by Administrator on 2022/1/17.
//

#include <mkrtos/fs.h>
#include <mkrtos/sp.h>
#include <mkrtos/debug.h>
#include <type.h>

#define SP_FILE_SET_ST_MODE_CMD 1
#define SP_FILE_CLR_ST_MODE_CMD 2
#define SP_FILE_GET_FLASH_ADDR	3

int sp_ioctl(struct inode* ino, struct file* fd, unsigned int cmd, unsigned long arg) {
	struct sp_inode *sp_ino;
	sp_ino=ino->i_fs_priv_info;
	switch (cmd) {
	case SP_FILE_GET_FLASH_ADDR:
	{
		uint32_t mem_addr;

		if (get_bk_start_addr_in_mem(ino->i_sb->s_dev_no, &mem_addr) < 0) {
			return -1;
		}
		*((uint32_t *)(arg)) = ino->i_sb->s_bk_size * sp_ino->_l.start_bk + mem_addr;
	}
 	break;
	case SP_FILE_CLR_ST_MODE_CMD:
	{
		FILE_CLR_ST_MODE(ino->i_type_mode);
		sp_free_ct_bk(ino->i_sb,sp_ino->_l.start_bk,sp_ino->_l.bk_num);
		ino->i_dirt = 1;
	}
	break;
	case SP_FILE_SET_ST_MODE_CMD:
	{
		uint32_t pre_file_size;
		uint32_t res_bk;
		uint32_t bk_num;
		pre_file_size = arg;
		bk_num = ROUND_UP(pre_file_size,ino->i_sb->s_bk_size);

		if(FILE_GET_ST_MODE(ino->i_type_mode) && !sp_ino->_l.bk_num) {
			sp_free_ct_bk(ino->i_sb,sp_ino->_l.start_bk,sp_ino->_l.bk_num);
		}
		int32_t res=sp_alloc_ct_bk(ino->i_sb,bk_num,&res_bk);
		if(res<0){
			return -1;
		}
		sp_ino->_l.start_bk=res_bk;
		sp_ino->_l.bk_num=bk_num;

		FILE_SET_ST_MODE(ino->i_type_mode);
		DEBUG("fs",INFO,"alloc start bk is %d,bk num is %d,res is %d\n",res_bk,ROUND_UP(pre_file_size,ino->i_sb->s_bk_size), res);
		ino->i_dirt = 1;
	}
		break;
	default:
		return -ENOSYS;
	}
	return 0;
}


static struct file_operations sp_file_operations = {
        NULL,			/* lseek - default */
        sp_file_read,		/* read */
        sp_file_write,			/* write */
        NULL,		/* readdir */
        NULL,			/* select - default */
		sp_ioctl,			/* ioctl */
        general_mmap,			/* mmap */
        general_mumap,/*mumap*/
        NULL,			/* no special open code */
        NULL,			/* no special release code */
        sp_sync_file		/* default fsync */
};

/*
 * directories can handle most operations...
 */
struct inode_operations sp_file_inode_operations = {
        &sp_file_operations,	/* default directory file-ops */
        NULL,		/* create */
        NULL,		/* create */
        NULL,		/* lookup */
        NULL,		/* link */
        NULL,		/* unlink */
        NULL,		/* symlink */
        NULL,		/* mkdir */
        NULL,		/* rmdir */
        NULL,		/* mknod */
        NULL,		/* rename */
        NULL,			/* readlink */
        NULL,			/* follow_link */
        NULL,			/* bmap */
        .truncate = sp_truncate,		/* truncate */
        NULL			/* permission */
};
