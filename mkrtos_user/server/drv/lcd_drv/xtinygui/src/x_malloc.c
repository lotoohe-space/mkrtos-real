/**
 * @file x_malloc.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-02-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "x_malloc.h"
#include <stdlib.h>
#include <stdio.h>

void *XMalloc(uint32_t size)
{
	void *mem = mm_alloc_page(MM_PROT, size, 0);

	return mem;
}

void XFree(void *mem)
{
	// free(mem);
	mm_free_page(MM_PROT, mem, 0);
}