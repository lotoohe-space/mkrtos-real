#include <mkrtos/fs.h>
#include <mkrtos/cpio.h>
#include <mkrtos.h>
#define SP_FILE_SET_ST_MODE_CMD 1
#define SP_FILE_CLR_ST_MODE_CMD 2
#define SP_FILE_GET_FLASH_ADDR	3

int cpio_ioctl(struct inode* ino, struct file* fd, unsigned int cmd, unsigned long arg) {
	cpio_fs_ino_priv_t *p_ino;
	p_ino=ino->i_fs_priv_info;

	switch (cmd) {
	case SP_FILE_GET_FLASH_ADDR:
	{
        uint32_t mem_addr;

		if (get_bk_start_addr_in_mem(ino->i_sb->s_dev_no, &mem_addr) < 0) {
			return -1;
		}
		*((uint32_t *)(arg)) = //ino->i_sb->s_bk_size * p_ino->st_bk +
         p_ino->in_bk_offs + mem_addr;
	}
 	break;
	case SP_FILE_CLR_ST_MODE_CMD:
	{
	
	}
	break;
	case SP_FILE_SET_ST_MODE_CMD:
	{
		
	}
		break;
	default:
		return -ENOSYS;
	}
	return 0;
}
int cpio_file_read(struct inode *inode, struct file *filp, char *buf, int count) {
    int r_size = 0;
    cpio_fs_ino_priv_t *ino_priv = inode->i_fs_priv_info;
    struct super_block *sb = inode->i_sb;
    
    r_size = cpio_read_data(inode->i_sb, buf, count,
        (sb->s_bk_size * ino_priv->st_bk) + ino_priv->in_bk_offs + sizeof(cpio_fs_t) +
        ALIGN(htoi(ino_priv->fs_info.c_namesize, 8),4) +  ALIGN(htoi(ino_priv->fs_info.c_filesize, 8),4));

    if (r_size < 0) {
        return r_size;
    }
    filp->f_ofs += r_size;
}
int cpio_lookup(struct inode *p_inode, const char *file_name, int len,
		struct inode **res_inode) {
	ino_t res_ino;
	int res;
	struct inode *r_inode;
    cpio_fs_ino_priv_t cfs;

    
	res = cpio_find_file(p_inode->i_sb, file_name, &cfs);//sp_dir_find(p_inode, file_name, len, &res_ino);
	if (res < 0) {
		puti(p_inode);
		return res;
	}
    res_ino = htoi(cfs.fs_info.c_ino, 8);

	r_inode = geti(p_inode->i_sb, res_ino);
	if (r_inode == NULL) {
		puti(p_inode);
		return -ENOENT;
	}
	puti(p_inode);

	if (res_inode) {
		*res_inode = r_inode;
	}
	return 0;
}
static struct file_operations cpio_file_operations = {
        NULL,			/* lseek - default */
        cpio_file_read,		/* read */
        NULL,			/* write */
        NULL,		/* readdir */
        NULL,			/* select - default */
		cpio_ioctl,			/* ioctl */
        NULL,			/* mmap */
        NULL,/*mumap*/
        NULL,			/* no special open code */
        NULL,			/* no special release code */
        NULL		/* default fsync */
};

struct inode_operations cpio_file_inode_operations = {
        .default_file_ops = &cpio_file_operations,	/* default directory file-ops */
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
        .truncate = NULL,		/* truncate */
        NULL			/* permission */
};

static struct file_operations cpio_dir_operations = {
        NULL,			/* lseek - default */
        NULL,		/* read */
        NULL,			/* write - bad */
        NULL,		/* readdir */
        NULL,			/* select - default */
        NULL,			/* ioctl - default */
        NULL,			/* mmap */
        NULL,
        NULL,			/* no special open code */
        NULL,			/* no special release code */
        NULL		/* default fsync */
};

/*
 * directories can handle most operations...
 */
struct inode_operations cpio_dir_inode_operations = {
        &cpio_dir_operations,	/* default directory file-ops */
        NULL,		/* create */
        cpio_lookup,		/* lookup */
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
        NULL,		/* truncate */
        NULL			/* permission */
};


