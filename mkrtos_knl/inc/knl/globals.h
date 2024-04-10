/**
 * @file globals.h
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-11-19
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "kobject.h"
#include "mm.h"
#include "types.h"

extern char _sdata[];
extern char _edata[];
extern char _sbss[];
extern char _ebss[];
extern mword_t cpio_images;

mem_t *mm_get_global(void);
void global_reg_kobj(kobject_t *kobj, int inx);
kobject_t *global_get_kobj(int inx);
