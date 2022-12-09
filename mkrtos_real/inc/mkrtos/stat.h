//
// Created by Administrator on 2022/1/21.
//

#ifndef UNTITLED1_STAT_H
#define UNTITLED1_STAT_H

#include <sys/types.h>
struct old_stat {
	uint32_t	st_dev;
	unsigned long	st_ino;
	uint16_t	st_mode;
	uint16_t	st_nlink;
	uint16_t	st_uid;
	uint16_t	st_gid;
	uint32_t	st_rdev;
	unsigned long	st_size;
	unsigned long	st_blksize;
	unsigned long	st_blocks;
	time_t		st_atime;
	unsigned long	st_atime_nsec;
	time_t		st_mtime;
	unsigned long	st_mtime_nsec;
	time_t		st_ctime;
	unsigned long	st_ctime_nsec;
	unsigned long	__unused4;
	unsigned long	__unused5;
};
struct new_stat {
	unsigned long st_dev;
	unsigned short __pad1;
	unsigned long st_ino;
	unsigned long st_mode;
	unsigned short st_nlink;
	unsigned short st_uid;
	unsigned short st_gid;
	unsigned short st_rdev;
	unsigned short __pad2;
	unsigned long  st_size;
	unsigned long  st_blksize;
	unsigned long  st_blocks;
	unsigned long  st_atime;
	unsigned long  __unused1;
	unsigned long  st_mtime;
	unsigned long  __unused2;
	unsigned long  st_ctime;
	unsigned long  __unused3;
	unsigned long  __unused4;
	unsigned long  __unused5;
};


#define LX_S_IFMT	    00170000
#define LX_S_IFSOCK		0140000
#define LX_S_IFLNK		0120000
#define LX_S_IFREG		0100000
#define LX_S_IFBLK		0060000
#define LX_S_IFDIR		0040000
#define LX_S_IFCHR		0020000
#define LX_S_IFIFO		0010000
#define LX_S_ISUID		0004000
#define LX_S_ISGID		0002000
#define LX_S_ISVTX		0001000

#define LX_S_ISLNK(m)	(((m) & LX_S_IFMT) == LX_S_IFLNK)
#define LX_S_ISREG(m)	(((m) & LX_S_IFMT) == LX_S_IFREG)
#define LX_S_ISDIR(m)	(((m) & LX_S_IFMT) == LX_S_IFDIR)
#define LX_S_ISCHR(m)	(((m) & LX_S_IFMT) == LX_S_IFCHR)
#define LX_S_ISBLK(m)	(((m) & LX_S_IFMT) == LX_S_IFBLK)
#define LX_S_ISFIFO(m)	(((m) & LX_S_IFMT) == LX_S_IFIFO)
#define LX_S_ISSOCK(m)	(((m) & LX_S_IFMT) == LX_S_IFSOCK)

#define S_IFMT  0x0000ff00
#define S_IFSOCK (6<<8) //套接字文件
#define S_IFLNK	 (4<<8) //链接文件
#define S_IFREG  (0<<8) //正常文件
#define S_IFBLK  (3<<8) //块文件
#define S_IFDIR  (1<<8) //目录文件
#define S_IFCHR  (2<<8) //字符文件
#define S_IFIFO  (5<<8) //有名管道文件
#define S_IFSTRICT (0x80<<8)//严格模式
#define S_ISUID  0004000		        /* u+s可以让用户在执行这个二进制程序的时候，
								  * effective id变为这个文件的owner user
								  */
#define S_ISGID  0002000
#define S_ISVTX  0001000

#define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK) /*链接文件*/
#define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)  /* 是否一个常规文件 */
#define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)    /* 是否一个目录 */
#define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)  /* 是否一个字符文件 */
#define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)   /* 是否一个块文件 */
/* 判断是否有名管道文件，有名管道的名字存在于文件系统当中，而内容存在于内存当中 */
#define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)

#define S_IRWXU 00700     /* 用户自己的rwx权限 */
#define S_IRUSR 00400     /* 用户是否可读 */
#define S_IWUSR 00200     /* 用户是否可写 */
#define S_IXUSR 00100     /* 用户是否可执行 */

#define S_IRWXG 00070     /* 用户所在组的rwx权限 */
#define S_IRGRP 00040     /* 用户所在组的r权限 */
#define S_IWGRP 00020     /* 用户所在组的w权限 */
#define S_IXGRP 00010     /* 用户所在组的x权限 */

#define S_IRWXO 00007     /* 其他用户的rwx权限 */
#define S_IROTH 00004     /* 其他用户的r权限 */
#define S_IWOTH 00002     /* 其他用户的w权限 */
#define S_IXOTH 00001     /* 其他用户的x权限 */

#ifdef __KERNEL__
#define S_IRWXUGO	(S_IRWXU|S_IRWXG|S_IRWXO)
#define S_IALLUGO	(S_ISUID|S_ISGID|S_ISVTX|S_IRWXUGO)
#define S_IRUGO		(S_IRUSR|S_IRGRP|S_IROTH)
#define S_IWUGO		(S_IWUSR|S_IWGRP|S_IWOTH)
#define S_IXUGO		(S_IXUSR|S_IXGRP|S_IXOTH)
#endif

#endif //UNTITLED1_STAT_H
