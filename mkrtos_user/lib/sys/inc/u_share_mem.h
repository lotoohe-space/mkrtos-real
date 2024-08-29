#include "u_types.h"
#include "u_prot.h"
#include "u_vmam.h"

msg_tag_t share_mem_map(obj_handler_t obj, vma_addr_t vaddr, umword_t *addr, umword_t *size);
msg_tag_t share_mem_unmap(obj_handler_t obj);
