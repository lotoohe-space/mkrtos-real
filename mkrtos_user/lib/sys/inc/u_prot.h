#pragma once

#include "u_types.h"

#define FACTORY_PROT 1
#define THREAD_PROT 2
#define TASK_PROT 3
#define LOG_PROT 4
#define IPC_PROT 5
#define MM_PROT 6

#define THREAD_MAIN THREAD_PROT
#define TASK_THIS TASK_PROT

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

static inline ipc_type_t ipc_type_create_3(
    enum msg_type type, uint8_t msg_buf_len, uint8_t map_buf_len)
{
    return (ipc_type_t){
        .type = (uint8_t)type,
        .msg_buf_len = msg_buf_len,
        .map_buf_len = map_buf_len,
    };
}

typedef union msg_tag
{
    umword_t raw;
    struct
    {
        umword_t type : 4;
        umword_t type2 : 8;
        umword_t prot : WORD_BITS - 12;
    };
} msg_tag_t;

#define msg_tag_init(r) \
    ((msg_tag_t){.raw = (r)})

#define msg_tag_init3(t, t2, p) ((msg_tag_t){ \
    .type = t,                                \
    .type2 = t2,                              \
    .prot = p})
#define msg_tag_get_prot(tag) \
    ((int16_t)((tag).prot))

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

static inline syscall_prot_t syscall_prot_create(uint8_t op, uint8_t prot, obj_handler_t obj_inx)
{
    return (syscall_prot_t){
        .op = op,
        .prot = prot,
        .obj_inx = obj_inx,
    };
}
