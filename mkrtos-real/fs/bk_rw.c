//
// Created by Administrator on 2022/4/7.
//
#include <mkrtos/fs.h>
#include <mkrtos/bk.h>
#include <mkrtos.h>
#include <fcntl.h>
#include <string.h>

/* ���豸˳���д */

/**
 * �򿪿��豸
 * @param inode
 * @param filp
 * @return
 */
int bk_file_open(struct inode *ino, struct file *filp) {
	int ret;
	struct bk_operations *bk_ops;
	bk_ops = get_bk_ops(ino->i_rdev_no);
	if (!bk_ops) {
		return -1;
	}
	if (bk_ops->open) {
		ret = bk_ops->open(ino->i_rdev_no);
		if (ret < 0) {
			return ret;
		}
	}
	return 0;
}
//�����豸
int bk_file_read(struct inode *ino, struct file *fp, char *buf, int count) {
	struct bk_operations *bk_ops;
	struct super_block *sb;
	struct bk_cache *tmp_bk;

	int w_ofs;
	int w_ofs_bk;
	int dev_no;
	int w_size = 0;

	int w_len = 0;
	int cur_bk_rm_size = 0;
	int cur_w_ofs = 0;
	dev_no = ino->i_rdev_no;
	bk_ops = get_bk_ops(dev_no);
	if (!bk_ops) {
		return -1;
	}
	sb = ino->i_sb;
	if (fp->f_flags & O_APPEND) {
		w_ofs = ino->i_file_size;
	} else {
		w_ofs = fp->f_ofs;
	}
	ino->i_file_size += count;
	while (w_len < count) {
		w_ofs += w_len; //0
		//��ȡ��ʼ�Ŀ�ƫ��
		w_ofs_bk = w_ofs / sb->s_bk_size;        //0
		//��ǰ��ȡ��ʣ��Ĵ�С
		cur_bk_rm_size = sb->s_bk_size - (w_ofs % sb->s_bk_size);        //2048
		tmp_bk = bk_read(dev_no, w_ofs_bk, 0, 0);
		if (!tmp_bk) {
			ino->i_file_size -= count;
			//���е�����˵����ǰ�Ŀ��豸û�д洢�ռ���
			return -ENOSPC;
		}
		//�����Ҫ��ȡ�Ĵ�С
		w_size = MIN((count - w_len), sb->s_bk_size);
		w_size = MIN(w_size, cur_bk_rm_size);
		//���㵱ǰ��Ҫ��ȡ��ƫ��
		cur_w_ofs = w_ofs % sb->s_bk_size;
		mkrtos_memcpy(buf + w_len, tmp_bk->cache + cur_w_ofs, w_size);
		bk_release(tmp_bk);

		//���˶�������
		w_len += w_size;
	}
	return w_len;
}

//д���豸
int bk_file_write(struct inode *ino, struct file *fp, char *buf, int count) {
	struct bk_operations *bk_ops;
	struct super_block *sb;
	struct bk_cache *tmp_bk;

	int w_ofs;
	int w_ofs_bk;
	int dev_no;
	int w_size = 0;

	int w_len = 0;
	int cur_bk_rm_size = 0;
	int cur_w_ofs = 0;
	//bk�豸���豸��
	dev_no = ino->i_rdev_no;
	bk_ops = get_bk_ops(dev_no);
	if (!bk_ops) {
		return -1;
	}
	sb = ino->i_sb;
	if (fp->f_flags & O_APPEND) {
		w_ofs = ino->i_file_size;
	} else {
		w_ofs = fp->f_ofs;
	}
	ino->i_file_size += count;
	while (w_len < count) {
		w_ofs += w_len;        //0
		//��ȡ��ʼ�Ŀ�ƫ��
		w_ofs_bk = w_ofs / sb->s_bk_size;        //0
		//��ǰд���ʣ��Ĵ�С
		cur_bk_rm_size = sb->s_bk_size - (w_ofs % sb->s_bk_size);        //2048
		tmp_bk = bk_read(dev_no, w_ofs_bk, 1, 0);
		if (!tmp_bk) {
			ino->i_file_size -= count;
			//���е�����˵����ǰ�Ŀ��豸û�д洢�ռ���
			return -ENOSPC;
		}
		//�����Ҫд��Ĵ�С
		w_size = MIN((count - w_len), sb->s_bk_size);
		w_size = MIN(w_size, cur_bk_rm_size);
		//���㵱ǰ��Ҫд���ƫ��
		cur_w_ofs = w_ofs % sb->s_bk_size;
		mkrtos_memcpy(tmp_bk->cache + cur_w_ofs, buf + w_len, w_size);
		bk_release(tmp_bk);

		//д��������
		w_len += w_size;
	}
	return w_len;
}
int bK_file_fsync(struct inode *inode, struct file *filp) {
	sync_all_bk(inode->i_rdev_no);
	return 0;
}
int bk_file_release(struct inode *ino, struct file *fp) {
	struct bk_operations *bk_ops;
	bk_ops = get_bk_ops(ino->i_rdev_no);
	if (!bk_ops) {
		return -1;
	}
	if (bk_ops->release) {
		bk_ops->release(ino->i_rdev_no);
	}
	return 0;
}
