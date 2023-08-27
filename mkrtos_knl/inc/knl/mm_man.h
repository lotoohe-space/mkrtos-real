#pragma once
#include "mm_page.h"
void mm_man_dump(void);
void *mm_page_alloc_fault(mm_pages_t *mm, addr_t addr);