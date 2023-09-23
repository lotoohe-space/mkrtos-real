#pragma once

#define NS_PROT 0x0001               //!< namespace的协议
#define NS_REGISTER_OP ((uint16_t)0) //!< ns注册
#define NS_QUERY_OP ((uint16_t)1)    //!< ns请求

#define FS_PROT 0x0002
#define FS_OPEN ((uint16_t)0)
#define FS_CLOSE ((uint16_t)1)
#define FS_READ ((uint16_t)2)
#define FS_WRITE ((uint16_t)3)
