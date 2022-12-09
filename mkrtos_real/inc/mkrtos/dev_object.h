/*
 * dev_object.h
 *
 *  Created on: 2022年8月6日
 *      Author: Administrator
 */

#ifndef INC_MKRTOS_DEV_OBJECT_H_
#define INC_MKRTOS_DEV_OBJECT_H_
#include <mkrtos/fs.h>

#include <mkrtos/object.h>
struct dev_object{
	struct object parent;

	/**
	* @brief 操作函数
	*/
	struct file_operations *f_ops;
};

int dev_object_open(struct inode *inode,struct file * fd);
int dev_object_read(struct inode* inode,struct file* fd, char* buf,int len);
int dev_object_write(struct inode* inode,struct file* fd,char* buf,int len);
int dev_object_close(struct inode* inode,struct file* fd);

#endif /* INC_MKRTOS_DEV_OBJECT_H_ */
