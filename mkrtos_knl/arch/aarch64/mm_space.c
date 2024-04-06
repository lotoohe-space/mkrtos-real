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
#include <early_boot.h>
#include <mm_page.h>
static umword_t global_asid = 1;
int mm_space_init(mm_space_t *mm_space, int is_knl)
{
    int ret = 0;

    page_entry_init(&mm_space->mem_dir);
    mm_space->asid = global_asid++;
    ret = task_vma_init(&mm_space->mem_vma);

    return ret;
}
bool_t mm_space_add(mm_space_t *m_space,
                    umword_t addr,
                    umword_t size,
                    uint8_t attrs)
{
    return 0;
}
void mm_space_del(mm_space_t *m_space, umword_t addr)
{
    /*TODO:*/
}
