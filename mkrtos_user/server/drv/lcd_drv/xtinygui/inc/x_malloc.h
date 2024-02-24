/**
 * @file x_malloc.h
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#ifndef _X_MALLOC_H__
#define _X_MALLOC_H__

#include "x_types.h"

void *XMalloc(uint32_t size);
void XFree(void *mem);

#endif
