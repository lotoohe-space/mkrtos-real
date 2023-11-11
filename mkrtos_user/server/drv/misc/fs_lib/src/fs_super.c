
#include "fs.h"
#include "fs_inode.h"
#include <malloc.h>
#include <string.h>

struct inode *fs_alloc_inode(struct inode *p_inode)
{
	fs_inode_t *sp_ino;

	p_inode->i_priv_data = malloc(sizeof(fs_inode_t));
	if (p_inode->i_priv_data == NULL)
	{
		return NULL;
	}
	memset(sp_ino, 0, sizeof(fs_inode_t));
	return p_inode;
}

void fs_free_inode(struct inode *p_inode)
{
	free(p_inode->i_priv_data);
}

/**
 * 读取一个inode
 * @param inode
 * @return
 */
int fs_read_inode(struct inode *inode)
{

	return -1;
}
/**
 * @brief 写入inode
 *
 * @param i_node
 */
void fs_write_inode(struct inode *i_node)
{
}
/**
 * @brief
 *
 * @param i_node
 */
void fs_put_inode(struct inode *i_node)
{
}
/**
 * @brief
 *
 * @param sb
 */
void fs_write_super(struct super_block *sb)
{
}
void fs_statfs(struct super_block *sb, struct statfs *s_fs)
{
}
int fs_remount_fs(struct super_block *sb, int *a, char *b)
{

	return -1;
}
static struct super_operations fs_sb = {
	.alloc_inode = fs_alloc_inode,
	.free_inode = fs_free_inode,
	.read_inode = fs_read_inode,
	.notify_change = NULL,
	.write_inode = fs_write_inode,
	.put_inode = fs_put_inode,
	.put_super = NULL,
	.write_super = fs_write_super,
	.statfs = fs_statfs,
	.remount_fs = fs_remount_fs,
};
