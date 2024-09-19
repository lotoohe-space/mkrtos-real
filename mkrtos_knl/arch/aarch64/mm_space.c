/**
 * @file mm_space.c
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-09-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "types.h"
#include "util.h"
#include "mm_space.h"
#include "assert.h"
#include <mmu.h>
#include <mm_page.h>
static umword_t global_asid = 1;
int mm_space_init(mm_space_t *mm_space, int is_knl)
{
    int ret = 0;

    page_entry_init(&mm_space->mem_dir);
    ret = task_vma_init(&mm_space->mem_vma);
    mm_space->asid = global_asid++;

    return ret;
}
