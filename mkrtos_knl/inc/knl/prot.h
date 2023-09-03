/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2023-08-14 09:47:54
 * @LastEditors: zhangzheng 1358745329@qq.com
 * @LastEditTime: 2023-08-14 13:04:22
 * @FilePath: /mkrtos-real/mkrtos_knl/inc/knl/prot.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include "types.h"
#include "err.h"

#define FACTORY_PROT 1
#define THREAD_PROT 2
#define TASK_PROT 3
#define LOG_PROT 4
#define IPC_PROT 5
#define MM_PROT 6

#define FACTORY_PORT_START FACTORY_PROT
#define FACTORY_FUNC_MAX (MM_PROT + 1)
#define FACTORY_PORT_END FACTORY_FUNC_MAX

enum msg_type
{
    MSG_NONE_TYPE = 0,
    MSG_DEF_TYPE = 1,
};
typedef union ipc_type
{
    union
    {
        uint8_t raw;
        struct
        {
            uint8_t type : 1;
            uint8_t msg_buf_len : 5;
            uint8_t map_buf_len : 2;
        };
    };
} ipc_type_t;

static inline ipc_type_t ipc_type_create(uint8_t raw)
{
    return (ipc_type_t){.raw = raw};
}

typedef struct msg_tag
{
    union
    {
        umword_t raw;
        struct
        {
            umword_t type : 4;                           //!< 系统调用时代表操作码
            umword_t type2 : 8;                          //!< log操作，存放打印的长度; 也可以用于标志消息的类型
            umword_t prot : (sizeof(umword_t) * 8) - 12; //!< 代码操作的系统调用类型，或者返回值时存放错误码
        };
    };
} msg_tag_t;

#define msg_tag_init(r) \
    ((msg_tag_t){.raw = (r)})

#define msg_tag_init3(t, t2, p) ((msg_tag_t){ \
    .type = t,                                \
    .type2 = t2,                              \
    .prot = p})

typedef union syscall_prot
{
    umword_t raw;
    struct
    {
        umword_t op : 6;                     //!< 操作的op
        umword_t prot : 6;                   //!< 通信的类型
        umword_t obj_inx : (WORD_BITS - 12); //!<
    };
} syscall_prot_t;

#define syscall_prot_create_raw(r) ((syscall_prot_t){.raw = (r)})

static inline syscall_prot_t syscall_prot_create(uint8_t op, uint8_t prot, obj_handler_t obj_inx)
{
    return (syscall_prot_t){
        .op = op,
        .prot = prot,
        .obj_inx = obj_inx,
    };
}
