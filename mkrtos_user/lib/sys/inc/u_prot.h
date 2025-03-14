#pragma once

#include "u_types.h"

enum kobj_prot
{
    FACTORY_PROT = 1,
    THREAD_PROT,
    TASK_PROT,
    LOG_PROT,
    IPC_PROT,
    SYS_PROT,
    FUTEX_PROT,
    IRQ_PROT,
    SHARE_MEM_PROT,
    VMA_PROT,  // 10
    SEMA_PROT, // 11
    MAX_PROT,  // 12
};

#define THREAD_MAIN THREAD_PROT
#define TASK_THIS TASK_PROT

//! flags in MSG_TAG
#define MSG_TAG_KNL_ERR 0x8  //!< 内核代表的错误
#define MSG_TAG_NO_RELAY 0x4 //!< 不进行回复

typedef union msg_tag
{
    umword_t raw;
    struct
    {
        umword_t flags : 4; // 3bit:代表错误由内核报告
        umword_t msg_buf_len : 7;
        umword_t map_buf_len : 5;
        umword_t prot : WORD_BITS - 16;
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
#if ARCH_WORD_SIZE == 64
#define msg_tag_get_prot(tag) \
    ((int)((tag).prot))
#else
#define msg_tag_get_prot(tag) \
    ((short)((tag).prot))
#endif
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
        umword_t flags : 8;               //!< 保留
        umword_t addr : (WORD_BITS - 12); //!< 地址
    };
} vpage_t;

#define VPAGE_FLAGS_MAP 0x1 //!< 是否ipc时立刻映射, 可以在ipc通信时忽略进行ipc，这样不会报错

static inline vpage_t vpage_create_raw(umword_t raw)
{
    return (vpage_t){.raw = raw};
}
static inline vpage_t vpage_create_raw3(umword_t attrs, umword_t flags, umword_t addr)
{
    return (vpage_t){.attrs = attrs, .flags = flags, .addr = addr};
}

/**
 * @brief 对象的类型
 *
 */
enum knl_obj_type
{
    BASE_KOBJ_TYPE,
    TASK_TYPE,
    THREAD_TYPE,
    IRQ_SENDER_TYPE,
    IPC_TYPE,
    LOG_TYPE,
    MM_TYPE,
    FACTORY_TYPE,
    SYS_TYPE,
    FUTEX_TYPE,
    SHARE_MEM_TYPE,
    VMAM_TYPE,
    SEMA_TYPE,
};
