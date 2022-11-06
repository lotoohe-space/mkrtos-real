//
// Created by Administrator on 2022/1/10.
//

#include <mkrtos/fs.h>
#include <mkrtos/sched.h>
#include <errno.h>
#include <mkrtos/smem_knl.h>
#include <fcntl.h>
#include <string.h>
#include <mm.h>
#include "mkrtos/stat.h"

int follow_link(struct inode *dir, struct inode *inode, int flag, int mode,
		struct inode **res_inode) {
	//��Ŀ¼������Ҫ���ٵ�inodeΪNULL
	if (!dir || !inode) {
		puti(dir);
		puti(inode);
		*res_inode = NULL;
		return -ENOENT;
	}
	/* ���follow_linkΪ�գ����ͷ�dir��
	 * ͬʱ��res_inodeָ��inode,��һ��������ܾ���Ŀ¼�ķ�������
	 * ����ȡ��Ŀ¼��������ʱ����ؼ���ת��������Ŀ¼����
	 */
	if (!inode->i_ops || !inode->i_ops->follow_link) {
		puti(dir);
		*res_inode = inode;
		return 0;
	}
	return inode->i_ops->follow_link(dir, inode, flag, mode, res_inode);
}

/* ��dir��Ӧ��Ŀ¼���в����ļ���Ϊname,�ҳ���Ϊlen���ļ���Ŀ¼��inode
 */
int lookup(struct inode *dir, const char *name, int len, struct inode **result) {
	struct super_block *sb;
	int perm;

	*result = NULL;
	if (!dir)
		return -ENOENT;
//    perm = permission(dir,MAY_EXEC);
	if (len == 2 && name[0] == '.' && name[1] == '.') {
		//����鿴����..Ŀ¼
		if (dir == get_current_task()->root_inode) {
			//�����ǰ�Ǹ��ڵ㣬��ֱ�ӷ��ظ��ڵ�����ˣ���Ϊ���ڵ����һ��Ŀ¼�����Լ�
			*result = dir;
			return 0;
		} else if ((sb = dir->i_sb) && (dir == sb->root_inode)) {
			//�������super_block�ĸ�Ŀ¼
			sb = dir->i_sb;
			puti(dir);
			//��������Ϊsb�Ĺ��ص�inode
			dir = sb->s_covered;
			if (!dir) {
				return -ENOENT;
			}
			atomic_inc(&dir->i_used_count);
		}
	}
	/* �жϲ���һ��Ŀ¼��Ŀ¼Ҳ��һ��������ļ�
	 */
	if (!dir->i_ops || !dir->i_ops->lookup) {
		puti(dir);
		return -ENOTDIR;
	}
//    if (!perm) {
//        puti(dir);
//        return -EACCES;
//    }
	if (!len) {
		*result = dir;
		return 0;
	}
	return dir->i_ops->lookup(dir, name, len, result);
}
static int _namei(const char *pathname, struct inode *base, int follow_links,
		struct inode **res_inode) {
	const char *basename;
	int namelen, error;
	struct inode *inode;

	*res_inode = NULL;
	error = dir_namei(pathname, &namelen, &basename, base, &base);
	if (error) {
		return error;
	}
	atomic_inc(&(base->i_used_count));
	error = lookup(base, basename, namelen, &inode);
	if (error) {
		puti(base);
		return error;
	}
	/* ����������� */
	if (follow_links) {
		error = follow_link(base, inode, 0, 0, &inode);
		if (error) {
			return error;
		}
	} else {
		puti(base);
	}
	*res_inode = inode;
	return 0;
}

/* ���������� */
int lnamei(const char *pathname, struct inode **res_inode) {
	int error;
	error = _namei(pathname, NULL, 0, res_inode);
	return error;
}
int namei(const char *pathname, struct inode **res_inode) {
	int error;
	error = _namei(pathname, NULL, 1, res_inode);
	return error;
}

/**
 * ��ȡ·�����ڵ�inode
 * @param file_path ·��
 * @param p_inode �ļ���inode
 * @param dir Ŀ¼��inode
 * @param file_name �ļ�����
 * @return
 */
int32_t dir_namei(const char *pathname, int32_t *namelen, const char **name,
		struct inode *base, struct inode **res_inode) {
	char c;
	const char *thisname;
	int len, error;
	struct inode *inode;

	*res_inode = NULL;
	if (!base) {
		//���Ǹ�Ŀ¼��ʼɨ��
		base = PWD_INODE;
		if (base) {
			atomic_inc(&base->i_used_count);
		}
	}
	if ((c = *pathname) == '/' || (c = *pathname) == '\\') {
		// �� / ���� \��ʼ��·����˵���ǴӸ�Ŀ¼��ʼ��
		puti(base);
		base = ROOT_INODE;
		pathname++;
		if (base) {
			atomic_inc(&base->i_used_count);
		}
	}
	while (1) {
		thisname = pathname;
		//�ҵ�һ��·�����ߵ��ַ�����β
		for (len = 0; (c = *(pathname++)) && (c != '/') && c != '\0'; len++)
			;
		if (!c) {
			//�����ֱ���˳�
			break;
		}
		//�������ü�������������ӣ�������ڵ�ǰ���̱��л���ȥ֮�󣬰Ѹ�inode���ͷŵ���
		atomic_inc(&base->i_used_count);
		error = lookup(base, thisname, len, &inode);
		if (error) {
			puti(base);
			return error;
		}
		//Ŀ�������һ�������ļ�������Ҫ�ҵ������ļ���Ӧ��inode
		error = follow_link(base, inode, 0, 0, &base);
		if (error) {
			return error;
		}
	}
	if (!base->i_ops || !base->i_ops->lookup) {
		puti(base);
		return -ENOTDIR;
	}
	*name = thisname;
	*namelen = len;
	*res_inode = base;
	return 0;
}

int32_t open_namei(const char *file_path, int32_t flags, int32_t mode,
		struct inode **res_inode, struct inode *base_dir) {
	struct inode *ino;
	struct inode *dir;
	int32_t res = 0;
	int32_t f_len;
	char *file_name;
	//��Ŀ¼
	res = dir_namei(file_path, &f_len, &file_name, base_dir, &dir);
	if (res < 0) {
		return res;
	}
	//��һ��Ŀ¼
	if (f_len == 0) {
		if ((flags & O_WRONLY) || (flags & O_RDWR)) {
			//Ŀ¼������дģʽ�����ڶ�дģʽ
			puti(dir);
			return -EISDIR;
		}
		*res_inode = dir;
		return 0;
	}
	atomic_inc(&dir->i_used_count);
	if (flags & O_CREAT) {
		res = lookup(dir, file_name, f_len, &ino);
		if (res >= 0) {
			if (flags & O_EXCL) {
				puti(ino);
				res = -EEXIST;
			}
		} else if (!dir->i_ops || !dir->i_ops->create) {
			res = -EACCES;
		} else {
			atomic_inc(&dir->i_used_count);
			res = dir->i_ops->create(dir, file_name, sizeof(struct inode), mode,
					&ino);
			*res_inode = ino;
			puti(dir);
			return res;
		}
	} else {
		res = lookup(dir, file_name, f_len, &ino);
		if (res < 0) {
			puti(dir);
			return -ENOENT;
		}
	}
	if (flags & O_TRUNC) {
		if (ino->i_ops && ino->i_ops->truncate) {
			ino->i_ops->truncate(ino, 0);
		}
//        if ((error = notify_change(NOTIFY_SIZE, inode))) {
//            iput(inode);
//            return error;
//        }
		ino->i_dirt = 1;
	}
	*res_inode = ino;

	return res;
}

//�½�Ŀ¼
int sys_mkdir(const char *pathname, int mode) {
	struct inode *path_inode;
	struct inode *dir;
	const char *file_name;
	int32_t f_len;
	int32_t res;
	//kprint("%s 0x%x\n", pathname, mode);
	//�������·����inode
	if ((dir_namei(pathname, &f_len, &file_name, NULL, &path_inode)) < 0) {
		//Ŀ¼����
		return -ENOENT;
	}

	//������Ҫ���Ȩ��
	if (path_inode->i_ops && path_inode->i_ops->mkdir) {
		atomic_inc(&path_inode->i_used_count);
		res = path_inode->i_ops->mkdir(path_inode, file_name,
				mkrtos_strlen(file_name), mode);

		puti(path_inode);
		return res;
	}
	puti(path_inode);
	return -ENOSYS;
}
/**
 * ɾ��Ŀ¼
 * @param path_name
 * @param dir_name
 * @param name_len
 * @return
 */
int sys_rmdir(const char *pathname) {
	struct inode *path_inode;
	struct inode *dir;
	char *file_name;
	int32_t f_len;
	int32_t res;
	//�������·����inode
	if ((dir_namei(pathname, &f_len, &file_name, NULL, &path_inode)) < 0) {
		//Ŀ¼����
		return -ENOENT;
	}
	if (!S_ISDIR(path_inode->i_type_mode)) {
		puti(path_inode);
		return -ENOENT;
	}

	//������Ҫ���Ȩ��
	if (path_inode->i_ops && path_inode->i_ops->rmdir) {
		atomic_inc(&path_inode->i_used_count);
		res = path_inode->i_ops->rmdir(path_inode, file_name,
				mkrtos_strlen(file_name));
		if (res < 0) {
			puti(path_inode);
			return res;
		}
	}
	puti(path_inode);
	return -ENOSYS;
}
int do_link(struct inode *oldinode, const char *newname) {
	struct inode *dir;
	struct inode *p_dir;
	const char *basename;
	int error;
	int32_t f_len;
	error = dir_namei(newname, &f_len, &basename, NULL, &dir);
	if (error < 0) {
		//�ļ��Ѿ�������
		puti(oldinode);
		return error;
	}
	if (!mkrtos_strlen(newname)) {
		puti(oldinode);
		puti(dir);
		return -EPERM;
	}
//    if (IS_RDONLY(dir)) {
//        iput(oldinode);
//        iput(dir);
//        return -EROFS;
//    }
	if (dir->i_sb->s_dev_no != oldinode->i_sb->s_dev_no) {
		puti(dir);
		puti(oldinode);
		return -EXDEV;
	}
//    if (!permission(dir,MAY_WRITE | MAY_EXEC)) {
//        iput(dir);
//        iput(oldinode);
//        return -EACCES;
//    }
	if (!dir->i_ops || !dir->i_ops->link) {
		puti(dir);
		puti(oldinode);
		return -EPERM;
	}
//    down(&dir->i_sem);
	error = dir->i_ops->link(oldinode, dir, basename, mkrtos_strlen(basename));
//    up(&dir->i_sem);
	return error;
}

int sys_link(const char *oldname, const char *newname) {
	int error;
	struct inode *oldinode;
	error = namei(oldname, &oldinode);
	if (error < 0) {
		return error;
	}
	error = do_link(oldinode, newname);
	return error;
}
//ɾ���ļ�
int sys_unlink(const char *pathname) {
	struct inode *path_inode;
	int32_t res;
	int32_t f_len;
	char *file_name;

	//�������·����inode
	if ((dir_namei(pathname, &f_len, &file_name, NULL, &path_inode)) < 0) {
		return -ENOENT;
	}
	if (!S_ISDIR(path_inode->i_type_mode)) {
		puti(path_inode);
		return -ENOENT;
	}

	if (path_inode->i_ops && path_inode->i_ops->unlink) {
		atomic_inc(&path_inode->i_used_count);
		res = path_inode->i_ops->unlink(path_inode, file_name,
				mkrtos_strlen(file_name));
		puti(path_inode);
		return res;
	}
	puti(path_inode);

	return -ENOSYS;
}
static int do_symlink(const char *oldname, const char *newname) {
	struct inode *dir;
	struct inode *p_dir;
	char *basename;
	int32_t f_len;
	int error;

	error = dir_namei(newname, &f_len, &basename, NULL, &dir);
	if (error < 0) {
		return error;
	}
	if (!mkrtos_strlen(basename)) {
		puti(dir);
		return -ENOENT;
	}
//    if (IS_RDONLY(dir)) {
//        puti(dir);
//        return -EROFS;
//    }
//    if (!permission(dir,MAY_WRITE | MAY_EXEC)) {
//        iput(dir);
//        return -EACCES;
//    }
	if (!dir->i_ops || !dir->i_ops->symlink) {
		puti(dir);
		return -EPERM;
	}
//    down(&dir->i_sem);
	error = dir->i_ops->symlink(dir, basename, mkrtos_strlen(basename),
			oldname);
//    up(&dir->i_sem);
	return error;
}
int sys_symlink(const char *oldname, const char *newname) {
	int error;
	char *from, *to;

	//  error = getname(oldname,&from);
	from = oldname;
	to = newname;
	if (!error) {
		// error = getname(newname,&to);
		if (!error) {
			error = do_symlink(from, to);
			//    putname(to);
		}
		// putname(from);
	}
	return error;
}
int sys_rename(const char *oldname, const char *newname) {
	return -ENOSYS;
}
/**
 * �½�һ�������ļ�
 * @param filename
 * @param mode
 * @param dev
 * @return
 */
int sys_mknod(const char *filename, int mode, dev_t dev) {
	struct inode *path_inode;
	struct inode *dir;
	char *base_name;
	int32_t res;
	int32_t f_len;
	//�������·����inode
	if ((dir_namei(filename, &f_len, &base_name, NULL, &path_inode)) < 0) {
		return -ENOENT;
	}
	//�����ļ�ϵͳ���½������ļ�����
	if (path_inode->i_ops && path_inode->i_ops->mknod) {
		atomic_inc(&path_inode->i_used_count);
		int32_t res = path_inode->i_ops->mknod(path_inode, base_name,
				mkrtos_strlen(base_name), mode, dev);
		return res;
	}
	puti(path_inode);
	return -ENOSYS;
}

