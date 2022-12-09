#include <mkrtos/cpio.h>
#include <mkrtos/fs.h>
#include <mkrtos/stat.h>

extern struct super_operations cpio_s_indoe;

struct inode* cpio_alloc_inode(struct inode *p_inode) {
	cpio_fs_ino_priv_t *sp_ino;
	p_inode->i_fs_priv_info = malloc(sizeof(cpio_fs_ino_priv_t));
	if (p_inode->i_fs_priv_info == NULL) {
		return NULL;
	}
	sp_ino = p_inode->i_fs_priv_info;
	p_inode->i_ops = &cpio_dir_inode_operations;
	mkrtos_memset(sp_ino, 0, sizeof(cpio_fs_ino_priv_t));
	return p_inode;
}

int32_t cpio_free_inode(struct inode *p_inode) {
//    if(p_inode->i_hlink){
//        return -1;
//    }
	if (atomic_read(&p_inode->i_used_count)) {
		return -1;
	}
	free(p_inode->i_fs_priv_info);
	return 0;
}

static int32_t mode_tran(int32_t mode)
{
	switch(LX_S_IFMT & mode) {
		case LX_S_IFLNK:
			return MK_MODE(S_IFLNK, mode & 0777);
		case LX_S_IFREG:
			return MK_MODE(S_IFREG, mode & 0777);
		case LX_S_IFBLK:
			return MK_MODE(S_IFBLK, mode & 0777);
		case LX_S_IFDIR:
			return MK_MODE(S_IFDIR, mode & 0777);
		case LX_S_IFCHR:
			return MK_MODE(S_IFCHR, mode & 0777);
		case LX_S_IFIFO:
			return MK_MODE(S_IFIFO, mode & 0777);
		case LX_S_ISUID:
			return MK_MODE(S_ISUID, mode & 0777);
		case LX_S_ISGID:
			return MK_MODE(S_ISGID, mode & 0777);
		case LX_S_ISVTX:
			return MK_MODE(S_ISVTX, mode & 0777);
		default:
			kprint("unknow file type\n");
			break;
	}
	return MK_MODE(S_IFREG, mode & 0777);
}

int32_t cpio_read_inode(struct inode *inode) {
	uint32_t ino;
	struct super_block *sb = inode->i_sb;
	cpio_fs_ino_priv_t *cpio_ino = inode->i_fs_priv_info;
	cpio_sb_t *sp_sb = sb->s_sb_priv_info;
	// cpio_fs_t cpio_fs;

	ino = inode->i_no;
	if (ino == 0) {
		//文件类型与权限
		inode->i_type_mode =  MK_MODE(S_IFDIR,0777);
		//文件大小
		cpio_root_file_count(sb, cpio_ino, &inode->i_file_size);
		//自己的Inode号码
		inode->i_no = 0;
		//硬连接数
		inode->i_hlink = inode->i_file_size;
		inode->i_rdev_no = 0;
		inode->i_buser_no = 0;
		switch (FILE_TYPE(inode->i_type_mode)) {
		case 0:
			//普通文件
			inode->i_ops = &cpio_file_inode_operations;
			break;
		case 1:
			//目录文件
			inode->i_ops = &cpio_dir_inode_operations;
			break;
		case 2: //字符设备
			inode->i_ops = &chrdev_inode_operations;
			break;
		case 3: //块设备
			inode->i_ops = &blkdev_inode_operations;
			break;
		}
		//是否被修改过
		inode->i_dirt = 0;
		return 0;
	}
	//检查Inode是否被使用
	if (cpio_check_inode_no(sb, ino, cpio_ino) != 1) {
		return -1;
	}

	//文件类型与权限
	inode->i_type_mode = mode_tran(htoi(cpio_ino->fs_info.c_mode, 8));
	//文件大小
	inode->i_file_size = htoi(cpio_ino->fs_info.c_filesize, 8);
	//自己的Inode号码
	inode->i_no = ino;
	//硬连接数
	inode->i_hlink = htoi(cpio_ino->fs_info.c_nlink, 8);
	inode->i_rdev_no = 0;
	inode->i_buser_no = 0;
	switch (FILE_TYPE(inode->i_type_mode)) {
	case 0:
		//普通文件
		inode->i_ops = &cpio_file_inode_operations;
		break;
	case 1:
		//目录文件
		inode->i_ops = &cpio_dir_inode_operations;
		break;
	case 2: //字符设备
		inode->i_ops = &chrdev_inode_operations;
		break;
	case 3: //块设备
		inode->i_ops = &blkdev_inode_operations;
		break;
	}
	//是否被修改过
	inode->i_dirt = 0;
	return 0;
}

/**
 * 读取指定设备的sb
 * @param dev_no
 * @return
 */
struct super_block* cpio_read_sb(struct super_block *sb)
{
	//魔数
	sb->s_magic_num = 0x11223344;
	//块大小
	sb->s_bk_size = 512;
	//inode写入到磁盘的大小
	sb->s_inode_size = 0;
	//super操作函数
	sb->s_ops = &cpio_s_indoe;
	//是否修改过
	sb->s_dirt = 0;

	sb->root_inode = geti(sb, 0);
	sb->root_inode->i_ops = &cpio_dir_inode_operations;
	return sb;
}
void cpio_statfs(struct super_block *sb, struct statfs *s_fs) {

}
int cpio_remount_fs(struct super_block *sb, int *a, char *b) {

	return -ENOSYS;
}
int cpio_notify_change(int flags, struct inode *i_node) {

	return -ENOSYS;
}
struct super_operations cpio_s_indoe = {
		//申请指定文件系统的inode占用的内存
		.alloc_inode = cpio_alloc_inode,
		//释放指定文件系统的inode占用的内存
		.free_inode = cpio_free_inode,
		.read_inode = cpio_read_inode,
		.notify_change = cpio_notify_change,
		.write_inode = NULL,
		.put_inode = NULL,
		.put_super = NULL,
		.write_super = NULL,
		.statfs = cpio_statfs,
		.remount_fs = cpio_remount_fs
};
