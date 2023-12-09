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
#include <u_slist.h>
#include <sys/types.h>
enum ns_op
{
    OP_REGISTER,
    OP_QUERY,
};
void namespace_init(obj_handler_t ipc_hd);
int namespace_register(const char *path, obj_handler_t hd, int type);
int namespace_query(const char *path, obj_handler_t *hd);
int namespace_pre_alloc_map_fd(void);
void namespace_loop(void);
void ns_node_del_by_pid(pid_t pid, int to_del);
