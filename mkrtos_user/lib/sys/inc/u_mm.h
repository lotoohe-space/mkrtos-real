#pragma once

#include "u_types.h"
#include "u_prot.h"

enum region_rights
{
    REGION_PRIV = 1,
    REGION_RO = 2,
    REGION_RWX = 3,
};
#define MK_PAGE_SIZE 512
void *mm_alloc_page(obj_handler_t obj_inx, umword_t pnf_nr, uint8_t attrs);
void mm_free_page(obj_handler_t obj_inx, void *addr, umword_t pfn_nr);
msg_tag_t mm_align_alloc(obj_handler_t obj_inx, void *addr, umword_t size);
