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

typedef union msg_tag
{
    umword_t raw;
    struct
    {
        umword_t flags : 4;
        umword_t msg_buf_len : 5;
        umword_t map_buf_len : 2;
        umword_t prot : WORD_BITS - 12;
    };
} msg_tag_t;

#define msg_tag_init(r) \
    ((msg_tag_t){.raw = (r)})

#define msg_tag_init4(fg, msg_words, buf_words, p) ((msg_tag_t){ \
    .flags = (fg),                                               \
    .msg_buf_len = (msg_words),                                  \
    .map_buf_len = (buf_words),                                  \
    .prot = (p)})
#define msg_tag_get_prot(tag) \
    ((int16_t)((tag).prot))
#define msg_tag_get_val(tag) msg_tag_get_prot(tag)

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
