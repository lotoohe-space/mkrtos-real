#pragma

#include "u_types.h"
#include "u_prot.h"

#define THREAD_CREATE_VM 0x1

typedef enum share_mem_type
{
    SHARE_MEM_CNT_BUDDY_CNT, //!< buddy分配的连续内存，最大4M，默认的内存分配方式
    SHARE_MEM_CNT_CMA_CNT,   //!< cma分配的连续内存
    SHARE_MEM_CNT_DPD,       //!< 离散内存
} share_mem_type_t;

msg_tag_t factory_create_irq_sender(obj_handler_t obj, vpage_t vpage);
msg_tag_t factory_create_ipc(obj_handler_t obj, vpage_t vpage);
msg_tag_t factory_create_thread(obj_handler_t obj, vpage_t vpage);
msg_tag_t factory_create_thread_vcpu(obj_handler_t obj, vpage_t vpage);
msg_tag_t factory_create_task(obj_handler_t obj, vpage_t vpage);
msg_tag_t facotry_create_share_mem(obj_handler_t obj, vpage_t vpage, share_mem_type_t mem_type, umword_t size);
msg_tag_t facotry_create_sema(obj_handler_t obj, vpage_t vpage, int cnt, int max);
