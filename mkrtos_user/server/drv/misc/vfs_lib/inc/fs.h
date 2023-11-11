#pragma once

#include <u_types.h>
#include <stdint.h>
#include <sys/types.h>

#define FS_FILE_NR 16 //!< 最大数量

typedef struct file
{
    off_t f_off;      //!< 文件读写偏移
    umword_t f_flags; //!< flags.
    void *priv_data;  //!< 私有数据
} file_t;

typedef struct inode
{
    mode_t i_mode;     //!< 权限信息
    size_t i_size;     //!< 文件大小
    ino_t i_no;        //!< inode编号
    size_t i_hlink;    //!< 硬连接数
    void *i_priv_data; //!< inode的私有数据
} inode_t;

typedef struct super_operations
{

    struct inode *(*alloc_inode)(struct inode *p_inode); //!< 申请指定文件系统的inode
    void (*free_inode)(struct inode *p_inode);           //!< 释放指定文件系统的inode

    int32_t (*read_inode)(struct inode *p_r_ino);
    int (*notify_change)(int flags, struct inode *);
    void (*write_inode)(struct inode *);
    void (*put_inode)(struct inode *);
    void (*put_super)(struct super_block *);
    void (*write_super)(struct super_block *);
    void (*statfs)(struct super_block *, struct statfs *);
    int (*remount_fs)(struct super_block *, int *, char *);
} super_operations_t;

typedef struct super_block
{
    struct inode *root_inode;   //!< 根节点
    super_operations_t *sb_ops; //!< super_block的操作
    umword_t s_magic;
} super_block_t;

/**
 * file对文件的操作
 */
struct file_operations
{
    int (*lseek)(struct inode *, struct file *, off_t, int);
    int (*read)(struct inode *, struct file *, char *, int);
    int (*write)(struct inode *, struct file *, char *, int);
    int (*readdir)(struct inode *, struct file *, struct dirent *, int);
    int (*select)(struct inode *, struct file *, int, uint32_t *);
    int (*poll)(struct inode *inode, struct file *fp, void *wait_tb,
                struct timeval *timeout);
    int (*ioctl)(struct inode *, struct file *, unsigned int, unsigned long);
    int (*mmap)(struct inode *inode, struct file *fp, unsigned long addr,
                size_t len, int mask, unsigned long off);
    int (*mumap)(struct inode *inode, void *start, size_t len);
    int (*open)(struct inode *inode, struct file *fp);
    void (*release)(struct inode *ino, struct file *f);
    int (*fsync)(struct inode *, struct file *);
};

/**
 * inode对应的操作
 */
struct inode_operations
{
    struct file_operations *default_file_ops;
    int (*create)(struct inode *dir, const char *name, int len, int mode,
                  struct inode **result);
    int (*lookup)(struct inode *dir, const char *file_name, int len,
                  struct inode **ret_inode);
    int (*link)(struct inode *, struct inode *, const char *, int);
    int (*unlink)(struct inode *, const char *, int);
    int (*symlink)(struct inode *, const char *, int, const char *);
    int (*mkdir)(struct inode *, const char *, int, int);
    int (*rmdir)(struct inode *, const char *, int);
    int (*mknod)(struct inode *, const char *, int, int, int);
    int (*rename)(struct inode *, const char *, int, struct inode *, const char *,
                  int);
    int (*readlink)(struct inode *, char *, int);
    int (*follow_link)(struct inode *, struct inode *, int, int, struct inode **);
    int (*bmap)(struct inode *, int);
    void (*truncate)(struct inode *, int);
    int (*permission)(struct inode *, int);
};
