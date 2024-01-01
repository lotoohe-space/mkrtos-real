#include "u_types.h"
#include "u_prot.h"

msg_tag_t share_mem_map(obj_handler_t obj, uint8_t attrs, umword_t *addr, umword_t *size);
msg_tag_t share_mem_unmap(obj_handler_t obj);
