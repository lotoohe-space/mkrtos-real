#pragma once

#include "u_types.h"

#define FACTORY_PROT 1
#define THREAD_PROT 2
#define TASK_PROT 3
#define LOG_PROT 4
#define IPC_PROT 5
#define MM_PROT 6
#define SYS_PROT 7
#define IRQ_PROT 8

#define THREAD_MAIN THREAD_PROT
#define TASK_THIS TASK_PROT


#define MSG_TAG_KNL_ERR 0x8

typedef union msg_tag
{
    umword_t raw;
    struct
    {
        umword_t flags : 4; // 3bit:代表错误由内核报告
        umword_t msg_buf_len : 5;
        umword_t map_buf_len : 2;
        umword_t prot : WORD_BITS - 12;
    };
} msg_tag_t;

#define msg_tag_init(r) \
    ((msg_tag_t){.raw = (r)})

#define msg_tag_is_knl_err(tag) (!!((tag).flags & MSG_TAG_KNL_ERR)) //!< 内核错误

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
        umword_t prot : 5;                   //!< 通信的类型
        umword_t self : 1;                   //!< 如果obj_inx指定为无效，则采用当前thread
        umword_t obj_inx : (WORD_BITS - 12); //!<
    };
} syscall_prot_t;

static inline syscall_prot_t syscall_prot_create(uint8_t op, uint8_t prot, obj_handler_t obj_inx)
{
    return (syscall_prot_t){
        .op = op,
        .prot = prot,
        .obj_inx = obj_inx,
        .self = 0,
    };
}
static inline syscall_prot_t syscall_prot_create4(uint8_t op, uint8_t prot, obj_handler_t obj_inx, uint8_t self)
{
    return (syscall_prot_t){
        .op = op,
        .prot = prot,
        .obj_inx = obj_inx,
        .self = self,
    };
}
typedef union vpage
{
    umword_t raw;
    struct
    {
        umword_t attrs : 4;               //!< 权限
        umword_t : 8;                     //!< 保留
        umword_t addr : (WORD_BITS - 12); //!< 地址
    };
} vpage_t;

static inline vpage_t vpage_create_raw(umword_t raw)
{
    return (vpage_t){.raw = raw};
}
static inline vpage_t vpage_create_raw3(umword_t attrs, umword_t resv, umword_t addr)
{
    return (vpage_t){.attrs = attrs, .addr = addr};
}
