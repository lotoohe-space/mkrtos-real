/**
 * @file prot.h
 * @author zhangzheng (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-16
 *
 * @copyright Copyright (c) 2023
 *
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
#define SYS_PROT 7
#define FUTEX_PROT 8
#define IRQ_PROT 9

#define FACTORY_PORT_START FACTORY_PROT
#define FACTORY_FUNC_MAX (IRQ_PROT + 1)
#define FACTORY_PORT_END FACTORY_FUNC_MAX

#define MSG_TAG_KNL_ERR 0x8
typedef struct msg_tag
{
    union
    {
        umword_t raw;
        struct
        {
            umword_t flags : 4;
            umword_t msg_buf_len : 5;
            umword_t map_buf_len : 2;
            umword_t prot : WORD_BITS - 12;
        };
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
        umword_t prot : 5;                   //!< 通信的类型
        umword_t self : 1;                   //!< 如果obj_inx指定为无效，则采用当前thread
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

typedef union vpage
{
    umword_t raw;
    struct
    {
        umword_t attrs : 4;               //!< 权限
        umword_t flags : 8;               //!< 保留
        umword_t addr : (WORD_BITS - 12); //!< 地址
    };
} vpage_t;

#define VPAGE_FLAGS_MAP 0x1 //!< 是否ipc时立刻映射操作

static inline vpage_t vpage_create_raw(umword_t raw)
{
    return (vpage_t){.raw = raw};
}
static inline vpage_t vpage_create3(umword_t attrs, umword_t flags, umword_t addr)
{
    return (vpage_t){.attrs = attrs, .flags = flags, .addr = addr};
}
static inline obj_handler_t vpage_get_obj_handler(vpage_t vpage)
{
    return vpage.addr;
}
static inline uint8_t vpage_get_attrs(vpage_t vpage)
{
    return vpage.attrs;
}
