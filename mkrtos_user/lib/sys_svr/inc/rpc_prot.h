#pragma once

#define NS_PROT 0x0001               //!< namespace的协议
#define NS_REGISTER_OP ((uint16_t)0) //!< ns注册
#define NS_QUERY_OP ((uint16_t)1)    //!< ns请求

#define FS_PROT 0x0002
#define FS_OPEN ((uint16_t)0)      //!< 打开文件
#define FS_CLOSE ((uint16_t)1)     //!< 关闭文件
#define FS_READ ((uint16_t)2)      //!< 读取文件
#define FS_WRITE ((uint16_t)3)     //!< 写入文件
#define FS_LSEEK ((uint16_t)4)     //!< 写入文件位置
#define FS_FTRUNCATE ((uint16_t)5) //!< 文件截断
#define FS_SYNC ((uint16_t)6)      //!< 文件同步
#define FS_OPENDIR ((uint16_t)7)   //!< 打开目录，应该使用open打开，保留该接口*
#define FS_CLOSEDIR ((uint16_t)8)  //!< 关闭目录，应该使用closedir，保留该接口*
#define FS_READDIR ((uint16_t)9)   //!< 读取目录
#define FS_MKDIR ((uint16_t)10)    //!< 新建目录
#define FS_UNLINK ((uint16_t)11)   //!< 删除目录或者文件
#define FS_RENAME ((uint16_t)12)   //!< 重命名
#define FS_STAT ((uint16_t)13)     //!< 获取文件状态
#define FS_CHMOD ((uint16_t)14)    //!< 改变权限 暂不实现*
#define FS_UTIME ((uint16_t)15)    //!< 修改时间 暂不实现*
#define FS_CHDIR ((uint16_t)16)    //!< 进入某个目录，可在客户端实现 暂不实现*
#define FS_CWDIR ((uint16_t)17)    //!< 获取当前目录，可在客户端实现 暂不实现*
#define FS_MOUNT ((uint16_t)18)    //!< 挂载节点 暂不实现*

#define DRV_PROT 0x0003
#define DRV_OPEN ((uint16_t)0)  //!< 打开设备
#define DRV_READ ((uint16_t)1)  //!< 读取设备
#define DRV_WRITE ((uint16_t)2) //!< 写入设备
#define DRV_CLOSE ((uint16_t)3) //!< 关闭设备
#define DRV_IOCTL ((uint16_t)4) //!< 控制设备

#define META_PROT 0x0004 //!< 元协议

#define PM_PROT 0x0005           //!< 进程管理协议
#define PM_RUN_APP ((uint16_t)0) //!< 启动应用程序
