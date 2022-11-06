//
// Created by Administrator on 2022/1/10.
//
#include <type.h>
#include <errno.h>
#include <mkrtos/stat.h>
#include <mkrtos/fs.h>
#include <mkrtos/sched.h>
#include <fcntl.h>
#include <mkrtos/debug.h>

int sys_ustat(int dev, struct ustat *ubuf) {
	return -ENOSYS;
}
/**
 * ��ȡ�ļ�ϵͳ״̬
 * @param path
 * @param buf
 * @return
 */
int sys_statfs(const char *path, struct statfs *buf) {
	struct inode *inode;
	int32_t res;
	res = namei(path, &inode);
	if (res < 0) {
		return -1;
	}
	if (!inode->i_sb->s_ops->statfs) {
		//�ͷ����inode
		puti(inode);
		return -ENOSYS;
	}

	inode->i_sb->s_ops->statfs(inode->i_sb, buf);
	puti(inode);
	return 0;
}
int sys_fstatfs(unsigned int fd, struct statfs *buf) {
	struct file *files;
	files = get_current_task()->files;
	if (fd >= NR_FILE || files[fd].used == 0) {
		//�ļ��Ѿ��ر���
		return -EBADF;
	}
	if (!(files[fd].f_inode)) {
		return -ENOENT;
	}
	if ((files[fd].f_inode->i_sb->s_ops->statfs)) {
		files[fd].f_inode->i_sb->s_ops->statfs(files[fd].f_inode->i_sb, buf);
	} else {
		return -ENOSYS;
	}
	return 0;
}
int sys_truncate(const char *path, unsigned int length) {
	struct inode *inode;
	int error;

	error = namei(path, &inode);
	if (error) {
		return error;
	}
	if (!S_ISREG(inode->i_type_mode)) {
		puti(inode);
		return -EACCES;
	}
	if (inode->i_ops && inode->i_ops->truncate) {
		inode->i_ops->truncate(inode, length);
	}
	inode->i_dirt = 1;
	puti(inode);

	DEBUG("fs", INFO, "�ض��ļ���%s", path);

	return error;
}
int sys_ftruncate(unsigned int fd, unsigned int length) {
	struct inode *inode;
	struct file *files;
	files = get_current_task()->files;
	if (fd >= NR_FILE || files[fd].used == 0) {
		//�ļ��Ѿ��ر���
		return -EBADF;
	}
	if (!(files[fd].f_inode)) {
		return -ENOENT;
	}
	inode = &files[fd].f_inode;
	if (inode->i_ops && inode->i_ops->truncate) {
		inode->i_ops->truncate(inode, length);
	}
	inode->i_dirt = 1;
	puti(inode);
	return -ENOSYS;
}
int sys_utime(char *filename, struct utimbuf *times) {
	return -ENOSYS;
}
int sys_access(const char *filename, int mode) {
	return -ENOSYS;
}
/**
 * ����ĳ��Ŀ¼
 * @param filename
 * @return
 */
int sys_chdir(const char *filename) {
	struct inode *o_inode;
	const char *file_name;
	int32_t f_len;
	int32_t res;
	res = dir_namei(filename, &f_len, &file_name, NULL, &o_inode);
	if (res < 0) {
		return -ENOENT;
	}
	if (!S_ISDIR(o_inode->i_type_mode)) {
		puti(o_inode);
		return -ENOENT;
	}
	//�ͷŵ�֮ǰ��
	puti(get_current_task()->pwd_inode);
	//�����µ�
	get_current_task()->pwd_inode = o_inode;
	puti(o_inode);
	//strncpy(get_current_task()->pwd_path,filename,sizeof(get_current_task()->pwd_path));

	return 0;
}
/**
 * ��ȡ��ǰ����Ŀ¼
 * @param source
 * @param cn
 * @return
 */
int sys_getcwd(char *source, int cn) {
	//strncpy(source,CUR_TASK->pwd_path,cn);
	// return strlen(CUR_TASK->pwd_path);
	return -1;
}
/**
 * ����ĳ��Ŀ¼
 * @param fd
 * @return
 */
int sys_fchdir(unsigned int fd) {
	struct inode *o_inode;
	struct file *files;
	files = get_current_task()->files;
	if (fd >= NR_FILE || files[fd].used == 0) {
		//�ļ��Ѿ��ر���
		return -EBADF;
	}
	if (!(files[fd].f_inode)) {
		return -ENOENT;
	}
	o_inode = files[fd].f_inode;
	if (o_inode == NULL) {
		return -ENOENT;
	}
	if (!S_ISDIR(o_inode->i_type_mode)) {
		//���ﲻҪ�ͷ�f_inode����Ϊ���ܻ���ʹ��
		return -ENOENT;
	}
	//�ͷŵ�֮ǰ��
	puti(get_current_task()->pwd_inode);
	//�����µ�
	get_current_task()->pwd_inode = o_inode;
	return 0;
}
//���ĸ�Ŀ¼
int sys_chroot(const char *filename) {
	struct inode *o_inode;
	char *file_name;
	int32_t f_len;
	int32_t res;
	res = dir_namei(filename, &f_len, &file_name, NULL, &o_inode);
	if (res < 0) {
		return -ENOENT;
	}
	if (!S_ISDIR(o_inode->i_type_mode)) {
		puti(o_inode);
		return -ENOENT;
	}

	//�ͷŵ�֮ǰ��
	puti(get_current_task()->root_inode);
	//�����µ�
	get_current_task()->root_inode = o_inode;
	puti(o_inode);

	return 0;
}
//�����ļ���Ȩ��
int sys_fchmod(unsigned int fd, mode_t mode) {
	struct inode *o_inode;
	struct file *files;
	files = get_current_task()->files;
	if (fd >= NR_FILE || files[fd].used == 0) {
		//�ļ��Ѿ��ر���
		return -EBADF;
	}
	if (!(files[fd].f_inode)) {
		return -ENOENT;
	}

	if (files[fd].f_flags & O_RDONLY) {
		return -EROFS;
	}
	//��Ҫ���Ȩ�ޣ���û��

	o_inode = files[fd].f_inode;
	o_inode->i_type_mode &= 0xffff0000;
	o_inode->i_type_mode |= mode;

	o_inode->i_dirt = 1;
	return 0;
}
//�ı�ָ���ļ���Ȩ��
int sys_chmod(const char *filename, mode_t mode) {
	struct inode *o_inode;
	int32_t res;
	res = namei(filename, &o_inode);
	if (res < 0) {
		return -ENOENT;
	}

	//���Ȩ��
	//�ر��Ǽ�鵱ǰ�û���û��Ȩ�ޱ༭����ļ�
	o_inode->i_type_mode &= 0xffff0000;
	o_inode->i_type_mode |= mode;
	o_inode->i_dirt = 1;
	puti(o_inode);
	return 0;
}
int sys_fchown(unsigned int fd, int user, int group) {
	return -ENOSYS;
}
int sys_chown(const char *filename, int user, int group) {
	return -ENOSYS;
}

int32_t do_open(struct file *files, const char *path, int32_t flags,
		int32_t mode) {
	uint32_t i;
	int32_t res;
	struct inode *o_inode;
	//�ҵ�һ���յ�
	for (i = 0; i < NR_FILE; i++) {
		if (files[i].used == 0) {
			files[i].used = 1;
			break;
		}
	}
	//�Ƿ�û�ж�����ļ���������
	if (i >= NR_FILE) {
		errno = EMFILE;
		return -1;
	}
	DEBUG("fs", INFO, "��%s,flags:0x%x,mode:0x%x,fd:%d.", path, flags, mode, i);
	//���ļ�
	res = open_namei(path, flags, mode, &o_inode, NULL);
	if (res < 0) {
		files[i].used = 0;
		return res;
	}
	//�����ļ�������־
	files[i].f_flags = flags;
	//���ò����ĺ�����
	if ((!files[i].f_op) && o_inode->i_ops
			&& o_inode->i_ops->default_file_ops) {
		files[i].f_op = o_inode->i_ops->default_file_ops;
	}
	//�������������ļ�open����
	if (files[i].f_op && files[i].f_op->open) {
		if ((res = files[i].f_op->open(o_inode, &(files[i]))) < 0) {
			files[i].used = 0;
			puti(o_inode);
			return res;
		}
	}
	//ƫ��Ĭ��Ϊ0
	files[i].f_ofs = 0;
	files[i].f_inode = o_inode;
	//����Ȩ����ģʽ
	files[i].f_mode = o_inode->i_type_mode;

//    static int count;
//    if(count++>15) {
//        printk("%s %s fp:%d\n", __FUNCTION__, path,i);
//    }

	return i;
}
/**
 * ���ļ�
 * @param path
 * @param flags
 * @param mode
 * @return
 */
int32_t sys_open(const char *path, int32_t flags, int32_t mode) {
	return do_open(get_current_task()->files, path, flags, mode);
}

/**
 * ����һ���ļ�
 * @param pathname
 * @param mode
 * @return
 */
int sys_creat(const char *pathname, int size_mode) {
	DEBUG("fs", ERR, "�����ļ�:%s,size_mode:0x%x", pathname, size_mode);
	return sys_open(pathname, O_CREAT | O_WRONLY, size_mode);
}
/**
 * �ر�һ���ļ�
 * @param fp
 */
void file_close(int fp) {
	struct file *files;
	struct inode *inode;
	if (fp < 0 || fp >= NR_FILE) {
		DEBUG("fs", ERR, "�ļ�fd��%d����.", fp);
//        printk("%s fp.\n",__FUNCTION__ );
		return;
	}
	files = get_current_task()->files;
	if (files[fp].used == 0) {
		//�ļ��Ѿ��ر���
		return;
	}
	inode = files[fp].f_inode;

	if (files[fp].f_op && files[fp].f_op->release) {
		files[fp].f_op->release(inode, &files[fp]);
	}
	if (files[fp].f_op && files[fp].f_op->fsync) {
		files[fp].f_op->fsync(inode, &files[fp]);
	}

	files[fp].f_op = NULL;
	files[fp].used = 0;
	files[fp].f_ofs = 0;
	files[fp].f_inode = NULL;

	puti(inode);
	DEBUG("fs", INFO, "�ر��ļ���%d", fp);
}

int sys_fsync(int fp) {
	int res = -ENOSYS;
	struct file *files;
	struct inode *inode;
	if (fp < 0 || fp >= NR_FILE) {
		DEBUG("fs", ERR, "�ļ�fd��%d����.", fp);
		return -EBADF;
	}
	files = get_current_task()->files;
	if (files[fp].used == 0) {
		//�ļ��Ѿ��ر���
		return -EINVAL;
	}
	inode = files[fp].f_inode;
	if (files[fp].f_op && files[fp].f_op->fsync) {
		res = files[fp].f_op->fsync(inode, &files[fp]);
	}
	DEBUG("fs", INFO, "�ļ�ͬ����%d", fp);
	return res;
}
