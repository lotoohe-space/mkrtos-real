#pragma once

#include "u_types.h"
#include "u_rpc.h"
#include "rpc_prot.h"
#include "ns_types.h"

void ns_init(ns_t *ns);
int namespace_register(const char *path, obj_handler_t hd);
int namespace_query(const char *path, obj_handler_t *hd);
