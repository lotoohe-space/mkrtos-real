/**
 * @file namespace.h
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-11-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include "u_types.h"
#include "u_prot.h"

void namespace_init(void);
int namespace_register(const char *path, obj_handler_t hd);
int namespace_query(const char *path, obj_handler_t *hd);
int namespace_pre_alloc_map_fd(void);
void namespace_loop(void);
