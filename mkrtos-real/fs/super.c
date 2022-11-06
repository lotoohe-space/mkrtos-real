#include <mkrtos/fs.h>
#include <mkrtos/sched.h>
#include <mkrtos/stat.h>
#include "mkrtos/dev.h"
#include <mkrtos/debug.h>
#define SB_MAX_NUM 4

//ϵͳ���֧�ֵ�sb
static struct super_block sb_list[SB_MAX_NUM] = { 0 };
//ʹ�ñ�־λ
static uint8_t sb_used_flag[SB_MAX_NUM] = { 0 };

/**
 * ��ȡһ���յ�sb��û��ֱ�ӷ���NULL
 */
struct super_block* get_empty_sb(void) {
	int32_t i;
	for (i = 0; i < SB_MAX_NUM; i++) {
		if (sb_used_flag[i] == 0) {
			sb_used_flag[i] = 1;
			return sb_list + i;
		}
	}
	return NULL;
}
/**
 * �ͷ�һ��sb
 */
void free_sb(struct super_block *sb) {
	int32_t i;
	for (i = 0; i < SB_MAX_NUM; i++) {
		if (sb_used_flag[i] == 1) {
			if (&(sb_list[i]) == sb) {
				sb_used_flag[i] = 0;
				return;
			}
		}
	}
}
/**
 * �ҵ�һ��ʹ���е�sb
 */
struct super_block* find_sb(const dev_t d_no) {
	int32_t i;
	for (i = 0; i < SB_MAX_NUM; i++) {
		if (sb_used_flag[i] == 1) {
			if (sb_list[i].s_dev_no == d_no) {
				return &(sb_list[i]);
			}
		}
	}
	return NULL;
}
/**
 * ͬ�����б�ʹ�õ�sb
 */
int sync_all_sb(void) {
	for (int i = 0; i < SB_MAX_NUM; i++) {
		if (sb_used_flag[i]) {
			if (sb_list[i].s_ops && sb_list[i].s_ops->write_super) {
				sb_list[i].s_ops->write_super(&sb_list[i]);
			}
		}
	}
}
//��ȡָ���豸��sb
struct super_block* read_sb(dev_t dev_no, const char *fs_name,
		int32_t req_dev_no) {
	struct super_block *sb;
	struct fs_type *ft;

	sb = find_sb(dev_no);
	if (sb) {
		return sb;
	}
	sb = get_empty_sb();
	if (sb == NULL) {
		return NULL;
	}
	ft = find_fs_type(fs_name);
	if (ft == NULL) {
		return NULL;
	}
	sb->s_dev_no = dev_no;
	if (ft->alloc_sb(sb) == NULL) {
		free_sb(sb);
		ft->free_sb(sb);
		return NULL;
	}
	if (ft->read_sb(sb) == NULL) {
		free_sb(sb);
		ft->free_sb(sb);
		kprint("read sb fail.\n");
		return NULL;
	}

	DEBUG("fs", INFO, "%s remain blocks:%d.", fs_name, sp_get_free_bk(sb));
	DEBUG("fs", INFO, "%s remain inode:%d.", fs_name, sp_get_free_inode(sb));
	sb->s_dev_no = dev_no;
	return sb;
}
int do_mount(dev_t dev, const char *dir, char *type, int flags, void *data) {
	struct inode *dir_i;
	struct super_block *sb;
	int error;

	error = namei(dir, &dir_i);
	if (error) {
		return error;
	}
	if (atomic_read(&dir_i->i_used_count) != 1 || dir_i->i_mount) {
		puti(dir_i);
		return -EBUSY;
	}
	if (!S_ISDIR(dir_i->i_type_mode)) {
		puti(dir_i);
		return -EPERM;
	}
	sb = read_sb(dev, type, 0);
	if (!sb || sb->s_covered) {
		puti(dir_i);
		return -EBUSY;
	}
	/* ���ñ����ǵĹ��ص� */
	sb->s_covered = dir_i;
	/* ����dir���ص�λ�� */
	dir_i->i_mount = sb->root_inode;
	return 0;
}
int32_t sys_mount(char *dev_name, char *dir_name, char *type,
		unsigned long new_flags, void *data) {
	struct inode *inode;
	int32_t res;

	res = namei(dev_name, &inode);
	if (res < 0) {
		return res;
	}
	if (!S_ISBLK(inode->i_type_mode)) {
		puti(inode);
		return -1;
	}
	res = do_mount(MAJOR(inode->i_rdev_no), dir_name, type, new_flags, data);
	puti(inode);
	return res;
}
/**
 * ���ص�ǰ���̵ĸ��ļ�ϵͳ��ÿ������Ӧ�ö���Ҫ����һ��
 *
 */
int root_mount(struct task *task) {
	uint32_t i;
	struct super_block *sb;
	for (i = 0; i < fs_type_len; i++) {
		//һ��һ�����Թ���
		sb = read_sb(root_dev_no, fs_type_list[i].f_name,
				fs_type_list[i].req_dev_no);
		if (!sb) {
			continue;
		}

		//�߼���ʹ����4�Σ�. ..Ŀ¼���Σ�super_blockһ�Σ���ǰ����һ��
		atomic_inc(&(sb->root_inode->i_used_count));
		atomic_inc(&(sb->root_inode->i_used_count));
		atomic_inc(&(sb->root_inode->i_used_count));

		task->root_inode = sb->root_inode;
		task->pwd_inode = sb->root_inode;

		return 0;
	}
	kprint("mount root fail.\n");
	return -1;
}

