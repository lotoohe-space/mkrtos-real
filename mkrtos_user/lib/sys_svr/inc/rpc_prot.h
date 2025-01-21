#pragma once

#define NS_PROT 0x0001               //!< namespace的协议
#define NS_REGISTER_OP ((umword_t)0) //!< ns注册
#define NS_QUERY_OP ((umword_t)1)    //!< ns请求

#define FS_PROT 0x0002
#define FS_OPEN ((umword_t)0)      //!< 打开文件
#define FS_CLOSE ((umword_t)1)     //!< 关闭文件
#define FS_READ ((umword_t)2)      //!< 读取文件
#define FS_WRITE ((umword_t)3)     //!< 写入文件
#define FS_READDIR ((umword_t)4)   //!< 读取目录
#define FS_LSEEK ((umword_t)5)     //!< 写入文件位置
#define FS_FTRUNCATE ((umword_t)6) //!< 文件截断
#define FS_FSTAT ((umword_t)7)     //!< 文件状态
#define FS_IOCTL ((umword_t)8)     //!< ioctl
#define FS_FCNTL ((umword_t)9)     //!< 文件控制
#define FS_FSYNC ((umword_t)10)    //!< 文件同步

#define FS_UNLINK ((umword_t)11)  //!< 删除目录或者文件
#define FS_SYMLINK ((umword_t)12) //!< 软链接
#define FS_MKDIR ((umword_t)13)   //!< 新建目录
#define FS_RMDIR ((umword_t)14)   //!< 删除目录
#define FS_RENAME ((umword_t)15)  //!< 重命名
#define FS_STAT ((umword_t)16)    //!< 获取文件状态
#define FS_READLINK ((umword_t)17)
#define FS_STATFS ((umword_t)18) //!< 文件系统状态

#define DRV_PROT 0x0003
#define DRV_OPEN ((umword_t)0)  //!< 打开设备
#define DRV_READ ((umword_t)1)  //!< 读取设备
#define DRV_WRITE ((umword_t)2) //!< 写入设备
#define DRV_CLOSE ((umword_t)3) //!< 关闭设备
#define DRV_IOCTL ((umword_t)4) //!< 控制设备

#define META_PROT 0x0004 //!< 元协议

#define PM_PROT 0x0005             //!< 进程管理协议
#define PM_RUN_APP ((umword_t)0)   //!< 启动应用程序
#define PM_KILL_TASK ((umword_t)1) //!< 删除进程
#define PM_WATCH_PID ((umword_t)2) //!< watch pid
#define PM_COPY_DATA ((umword_t)3) //!< copy 进程数据

#define CONS_PROT 0x0006          //!< console协议
#define CONS_WRITE ((umword_t)0)  //!< console删除
#define CONS_READ ((umword_t)1)   //!< console读
#define CONS_ACTIVE ((umword_t)2) //!< console激活

#define PM_SIG_PROT 0x0007          //!< pm信号协议
#define PM_SIG_NOTIFY ((umword_t)0) //!< 通知消息

#define BLK_DRV_PROT 0x0008
#define BLK_DRV_WRITE ((umword_t)0) //!< 块驱动写
#define BLK_DRV_READ ((umword_t)1)  //!< 块驱动读
#define BLK_DRV_MAP ((umword_t)2)   //!< 块驱动对象映射
