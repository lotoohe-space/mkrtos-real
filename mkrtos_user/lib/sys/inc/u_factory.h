#pragma

#include "u_types.h"
#include "u_prot.h"

#define THREAD_CREATE_VM 0x1

msg_tag_t factory_create_irq_sender(obj_handler_t obj, vpage_t vpage);
msg_tag_t factory_create_ipc(obj_handler_t obj, vpage_t vpage);
msg_tag_t factory_create_thread(obj_handler_t obj, vpage_t vpage);
msg_tag_t factory_create_thread_vcpu(obj_handler_t obj, vpage_t vpage);
msg_tag_t factory_create_task(obj_handler_t obj, vpage_t vpage);
msg_tag_t facotry_create_share_mem(obj_handler_t obj, vpage_t vpage, umword_t size);
msg_tag_t facotry_create_sema(obj_handler_t obj, vpage_t vpage, int cnt, int max);
