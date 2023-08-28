#pragma once

#include "u_types.h"

#define FACTORY_PROT 1
#define THREAD_PROT 2
#define TASK_PROT 3
#define LOG_PROT 4
#define IPC_PROT 5
#define MM_PROT 6

typedef struct msg_tag
{
    union
    {
        umword_t raw;
        struct
        {
            umword_t type : 4;
            umword_t type2 : 8;
            umword_t prot : (sizeof(umword_t) * 8) - 12;
        };
    };
} msg_tag_t;

#define msg_tag_init(r) \
    ((msg_tag_t){.raw = (r)})

#define msg_tag_init3(t, t2, p) \
    msg_tag_init(((umword_t)(t)&0xf) | (((umword_t)(t2)&0xff) << 4) | (((umword_t)(p)) << 12))

#define msg_tag_get_prot(tag) \
    ((int16_t)((tag).prot))

