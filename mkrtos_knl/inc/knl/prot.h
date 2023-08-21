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

#define FACTORY_PORT_START FACTORY_PROT
#define FACTORY_FUNC_MAX (IPC_PROT + 1)
#define FACTORY_PORT_END FACTORY_FUNC_MAX

typedef union msg_tag
{
    umword_t raw;
    union
    {
        umword_t type : 4;
        umword_t type2 : 6;
        umword_t prot : (sizeof(umword_t) * 8) - 12;
    };
} msg_tag_t;

#define msg_tag_init(r) \
    ((msg_tag_t){.raw = (r)})

#define msg_tag_init3(t, t2, p) \
    msg_tag_init(((umword_t)(t)&0xf) | (((umword_t)(t2)&0xf) << 4) | (((umword_t)(p)) << 8))
