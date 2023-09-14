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
#define SYS_PROT 7
#define IRQ_PROT 8

#define FACTORY_PORT_START FACTORY_PROT
#define FACTORY_FUNC_MAX (IRQ_PROT + 1)
#define FACTORY_PORT_END FACTORY_FUNC_MAX

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
        umword_t : 8;                     //!< 保留
        umword_t addr : (WORD_BITS - 12); //!< 地址
    };
} vpage_t;

static inline vpage_t vpage_create_raw(umword_t raw)
{
    return (vpage_t){.raw = raw};
}
static inline vpage_t vpage_create3(umword_t attrs, umword_t resv, umword_t addr)
{
    return (vpage_t){.attrs = attrs, .addr = addr};
}
static inline obj_handler_t vpage_get_obj_handler(vpage_t vpage)
{
    return vpage.addr;
}
static inline uint8_t vpage_get_attrs(vpage_t vpage)
{
    return vpage.attrs;
}
