/*
 * dev_object.c
 *
 *  Created on: 2022��7��23��
 *      Author: Administrator
 */

#include <errno.h>
#include <mkrtos/object.h>
#include <mkrtos.h>
#include <mkrtos/dev_object.h>

int dev_object_open(struct inode *inode,struct file * fd){
	int ret = -EIO;
	MKRTOS_ASSERT(inode==NULL);
	MKRTOS_ASSERT(fd==NULL);
	struct dev_object *dev;
	dev=inode->i_fs_priv_info;

	if(dev->f_ops && dev->f_ops->open){
		ret=dev->f_ops->open(inode,fd);
	}

	return ret;
}
int dev_object_read(struct inode* inode,struct file* fd, char* buf,int len){
	int ret = -EIO;
	MKRTOS_ASSERT(inode==NULL);
	MKRTOS_ASSERT(fd==NULL);
	struct dev_object *dev;

	if(!buf){
		return -EINVAL;
	}

	dev=inode->i_fs_priv_info;

	if(dev->f_ops && dev->f_ops->read){
		ret=dev->f_ops->read(inode,fd,buf,len);
	}

	return ret;
}
int dev_object_write(struct inode* inode,struct file* fd,char* buf,int len){
	int ret = -EIO;
	MKRTOS_ASSERT(inode==NULL);
	MKRTOS_ASSERT(fd==NULL);
	struct dev_object *dev;

	if(!buf){
		return -EINVAL;
	}

	dev=inode->i_fs_priv_info;

	if(dev->f_ops && dev->f_ops->write){
		ret=dev->f_ops->write(inode,fd,buf,len);
	}

	return ret;
}
int dev_object_lseek(struct inode * inode, struct file *fd, off_t off, int len){
	int ret = -EIO;
	MKRTOS_ASSERT(inode==NULL);
	MKRTOS_ASSERT(fd==NULL);
	struct dev_object *dev;

	dev=inode->i_fs_priv_info;

	if(dev->f_ops && dev->f_ops->lseek){
		ret=dev->f_ops->lseek(inode,fd,off,len);
	}

	return ret;
}
int dev_object_close(struct inode* inode,struct file* fd){
	int ret = -EIO;
	MKRTOS_ASSERT(inode==NULL);
	MKRTOS_ASSERT(fd==NULL);
	struct dev_object *dev;

	dev=inode->i_fs_priv_info;

	if(dev->f_ops && dev->f_ops->release){
		dev->f_ops->release(inode,fd);
		ret=0;
	}

	return ret;
}
