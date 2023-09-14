#pragma

#include "u_types.h"
#include "u_prot.h"

msg_tag_t factory_create_irq_sender(obj_handler_t obj, vpage_t vpage);
msg_tag_t factory_create_ipc(obj_handler_t obj, vpage_t vpage);
msg_tag_t factory_create_thread(obj_handler_t obj, vpage_t vpage);
msg_tag_t factory_create_task(obj_handler_t obj, vpage_t vpage);